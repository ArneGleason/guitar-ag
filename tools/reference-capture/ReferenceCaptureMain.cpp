#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include <atomic>
#include <cmath>
#include <memory>
#include <vector>

namespace
{
constexpr int targetBitDepth = 24;
constexpr double targetPeakDb = -12.0;

juce::String isoTimestamp()
{
    return juce::Time::getCurrentTime().toISO8601 (true);
}

juce::String fileTimestamp()
{
    return juce::Time::getCurrentTime().formatted ("%Y%m%d-%H%M%S");
}

juce::String safeName (juce::String text)
{
    text = text.toLowerCase().retainCharacters (
        "abcdefghijklmnopqrstuvwxyz0123456789-_ ");
    text = text.replaceCharacter (' ', '-');

    while (text.contains ("--"))
        text = text.replace ("--", "-");

    text = text.trimCharactersAtStart ("-").trimCharactersAtEnd ("-");
    return text.isNotEmpty() ? text : "capture";
}

juce::String statusLabel (const juce::String& status)
{
    if (status == "approved")
        return "APPROVED";
    if (status == "rejected")
        return "REJECTED";
    return "candidate";
}

struct Take
{
    int number = 0;
    juce::File file;
    juce::String status { "candidate" };
    juce::String notes;
    juce::String recordedAt;
    int64_t samples = 0;
    double sampleRate = 0.0;
    float peak = 0.0f;
    double rms = 0.0;
    bool droppedAudio = false;
};

struct InventoryItem
{
    int order = 0;
    juce::String requestId;
    juce::String phaseId;
    juce::String phaseTitle;
    juce::String title;
    juce::String why;
    juce::File requestFile;
    int requiredApprovedTakes = 1;
};

class CaptureComponent final : public juce::Component,
                               private juce::AudioIODeviceCallback,
                               private juce::ListBoxModel,
                               private juce::Timer
{
public:
    explicit CaptureComponent (const juce::StringArray& commandLine)
        : deviceSelector (deviceManager, 1, 2, 0, 2, false, false, false, false),
          writerThread ("reference capture writer")
    {
        properties.setStorageParameters (makePropertiesOptions());

        std::unique_ptr<juce::XmlElement> savedState;
        if (auto* settings = properties.getUserSettings())
            savedState = settings->getXmlValue ("audioDeviceState");

        const auto deviceError = deviceManager.initialise (1, 2, savedState.get(), true);
        deviceManager.addAudioCallback (this);
        writerThread.startThread();
        formatManager.registerBasicFormats();

        addAndMakeVisible (deviceSelector);
        deviceSelector.setItemHeight (24);

        configureLabel (titleLabel, "Guitar AG Reference Capture", 24.0f, true);
        configureLabel (requestTitleLabel, "Manual capture", 18.0f, true);
        configureLabel (outputLabel, {}, 13.0f, false);
        configureLabel (statusLabelComponent, {}, 14.0f, true);
        configureLabel (meterLabel, "Input: -- dBFS", 15.0f, true);
        configureLabel (takeNotesLabel, "Selected-take notes", 13.0f, false);
        configureLabel (inventoryProgressLabel, "No capture inventory loaded", 14.0f, true);

        instructionsEditor.setMultiLine (true);
        instructionsEditor.setReadOnly (true);
        instructionsEditor.setWantsKeyboardFocus (false);
        instructionsEditor.setScrollbarsShown (true);
        instructionsEditor.setText (
            "Load a capture request, enable only the intended guitar input on the left, "
            "and record several takes. Keep the signal clean and leave at least 12 dB of headroom.");
        addAndMakeVisible (instructionsEditor);

        contextEditor.setMultiLine (true);
        contextEditor.setReadOnly (true);
        contextEditor.setWantsKeyboardFocus (false);
        contextEditor.setScrollbarsShown (true);
        contextEditor.setText ("Context will appear here when a request is loaded.");
        addAndMakeVisible (contextEditor);

        takeNotesEditor.setMultiLine (true);
        takeNotesEditor.setReturnKeyStartsNewLine (true);
        addAndMakeVisible (takeNotesEditor);

        configureButton (loadInventoryButton, "Load inventory...", [this] { chooseInventory(); });
        configureButton (loadRequestButton, "Load one request...", [this] { chooseRequest(); });
        configureButton (chooseFolderButton, "Capture folder...", [this] { chooseFolder(); });
        configureButton (recordButton, "Record take [Space]", [this]
        {
            startRecording();
            grabKeyboardFocus();
        });
        configureButton (stopButton, "Stop [Space]", [this]
        {
            stopRecording();
            grabKeyboardFocus();
        });
        configureButton (playButton, "Play selected", [this] { playSelected(); });
        configureButton (approveButton, "Approve", [this] { setSelectedStatus ("approved"); });
        configureButton (rejectButton, "Reject", [this] { setSelectedStatus ("rejected"); });
        configureButton (candidateButton, "Reset candidate", [this] { setSelectedStatus ("candidate"); });
        configureButton (saveNotesButton, "Save notes", [this] { saveSelectedNotes(); });

        inventoryCombo.setTextWhenNothingSelected ("Choose the next capture item");
        inventoryCombo.setWantsKeyboardFocus (false);
        inventoryCombo.onChange = [this]
        {
            if (updatingInventory)
                return;

            const auto index = inventoryCombo.getSelectedId() - 1;
            if (index >= 0 && index < static_cast<int> (inventoryItems.size()))
                loadRequest (inventoryItems[static_cast<size_t> (index)].requestFile);
        };
        addAndMakeVisible (inventoryCombo);

        takesList.setModel (this);
        takesList.setRowHeight (28);
        takesList.setWantsKeyboardFocus (false);
        addAndMakeVisible (takesList);

        setWantsKeyboardFocus (true);

        if (deviceError.isNotEmpty())
            setStatus ("Audio device error: " + deviceError, true);
        else
            setStatus ("Ready. Enable only the intended guitar input channel.", false);

        const auto inventoryArgument = findInventoryArgument (commandLine);
        const auto requestArgument = findRequestArgument (commandLine);
        if (inventoryArgument.isNotEmpty())
            loadInventory (juce::File (inventoryArgument));
        else if (requestArgument.isNotEmpty())
            loadRequest (juce::File (requestArgument));
        else
        {
            const auto defaultInventory = juce::File::getSpecialLocation (
                juce::File::userDocumentsDirectory)
                                              .getChildFile ("Guitar AG Reference Captures")
                                              .getChildFile ("capture-inventory.json");
            if (defaultInventory.existsAsFile())
                loadInventory (defaultInventory);
        }

        if (requestId.isEmpty())
            initialiseManualSession();

        setSize (1120, 760);
        startTimerHz (20);
        auto safeThis = juce::Component::SafePointer<CaptureComponent> (this);
        juce::MessageManager::callAsync ([safeThis]
        {
            if (safeThis != nullptr)
                safeThis->grabKeyboardFocus();
        });
    }

    ~CaptureComponent() override
    {
        stopTimer();
        stopRecording();
        stopPlayback();
        deviceManager.removeAudioCallback (this);

        if (auto* settings = properties.getUserSettings())
        {
            settings->setValue ("audioDeviceState", deviceManager.createStateXml().get());
            settings->saveIfNeeded();
        }

        writerThread.stopThread (3000);
        properties.closeFiles();
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour::fromRGB (22, 25, 29));
        g.setColour (juce::Colour::fromRGB (45, 50, 57));
        g.fillRect (devicePanelBounds.toFloat());
        g.setColour (juce::Colour::fromRGB (63, 69, 78));
        g.drawVerticalLine (devicePanelBounds.getRight(), 0.0f, static_cast<float> (getHeight()));
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced (14);
        titleLabel.setBounds (area.removeFromTop (34));
        area.removeFromTop (8);

        devicePanelBounds = area.removeFromLeft (330);
        deviceSelector.setBounds (devicePanelBounds.reduced (8));
        area.removeFromLeft (16);

        auto topButtons = area.removeFromTop (32);
        loadInventoryButton.setBounds (topButtons.removeFromLeft (132));
        topButtons.removeFromLeft (8);
        loadRequestButton.setBounds (topButtons.removeFromLeft (142));
        topButtons.removeFromLeft (8);
        chooseFolderButton.setBounds (topButtons.removeFromLeft (132));
        topButtons.removeFromLeft (8);
        outputLabel.setBounds (topButtons);

        area.removeFromTop (8);
        inventoryProgressLabel.setBounds (area.removeFromTop (24));
        inventoryCombo.setBounds (area.removeFromTop (32));
        area.removeFromTop (8);
        requestTitleLabel.setBounds (area.removeFromTop (28));
        instructionsEditor.setBounds (area.removeFromTop (88));
        area.removeFromTop (8);
        contextEditor.setBounds (area.removeFromTop (76));
        area.removeFromTop (10);

        auto transport = area.removeFromTop (36);
        recordButton.setBounds (transport.removeFromLeft (150));
        transport.removeFromLeft (8);
        stopButton.setBounds (transport.removeFromLeft (105));
        transport.removeFromLeft (8);
        playButton.setBounds (transport.removeFromLeft (112));
        transport.removeFromLeft (12);
        meterLabel.setBounds (transport.removeFromLeft (165));
        statusLabelComponent.setBounds (transport);

        area.removeFromTop (8);
        auto lower = area;
        auto listArea = lower.removeFromLeft (lower.getWidth() / 2);
        takesList.setBounds (listArea);
        lower.removeFromLeft (12);

        auto statusButtons = lower.removeFromTop (32);
        approveButton.setBounds (statusButtons.removeFromLeft (88));
        statusButtons.removeFromLeft (6);
        rejectButton.setBounds (statusButtons.removeFromLeft (78));
        statusButtons.removeFromLeft (6);
        candidateButton.setBounds (statusButtons.removeFromLeft (118));

        lower.removeFromTop (8);
        takeNotesLabel.setBounds (lower.removeFromTop (22));
        saveNotesButton.setBounds (lower.removeFromBottom (32).removeFromLeft (96));
        lower.removeFromBottom (8);
        takeNotesEditor.setBounds (lower);
    }

private:
    static juce::PropertiesFile::Options makePropertiesOptions()
    {
        juce::PropertiesFile::Options options;
        options.applicationName = "Guitar AG Reference Capture";
        options.filenameSuffix = ".settings";
        options.folderName = "GuitarAG";
        options.osxLibrarySubFolder = "Application Support";
        options.storageFormat = juce::PropertiesFile::storeAsXML;
        return options;
    }

    static juce::String findRequestArgument (const juce::StringArray& args)
    {
        for (int i = 0; i < args.size(); ++i)
        {
            if (args[i] == "--request" && i + 1 < args.size())
                return args[i + 1];
        }

        if (args.size() == 1 && args[0].endsWithIgnoreCase (".json"))
            return args[0];

        return {};
    }

    static juce::String findInventoryArgument (const juce::StringArray& args)
    {
        for (int i = 0; i < args.size(); ++i)
        {
            if (args[i] == "--inventory" && i + 1 < args.size())
                return args[i + 1];
        }

        return {};
    }

    void configureLabel (juce::Label& label,
                         const juce::String& text,
                         float fontHeight,
                         bool bright)
    {
        label.setText (text, juce::dontSendNotification);
        label.setFont (juce::FontOptions (fontHeight));
        label.setColour (juce::Label::textColourId,
                         bright ? juce::Colours::white : juce::Colours::lightgrey);
        addAndMakeVisible (label);
    }

    void configureButton (juce::TextButton& button,
                          const juce::String& text,
                          std::function<void()> action)
    {
        button.setButtonText (text);
        button.setWantsKeyboardFocus (false);
        button.onClick = std::move (action);
        addAndMakeVisible (button);
    }

    void initialiseManualSession()
    {
        requestId = "manual-" + fileTimestamp();
        requestVar = juce::var (new juce::DynamicObject());
        requestVar.getDynamicObject()->setProperty ("schema_version", 1);
        requestVar.getDynamicObject()->setProperty ("request_id", requestId);
        requestVar.getDynamicObject()->setProperty ("title", "Manual capture");
        requestVar.getDynamicObject()->setProperty (
            "instructions", instructionsEditor.getText());

        const auto documents = juce::File::getSpecialLocation (
            juce::File::userDocumentsDirectory);
        sessionDirectory = documents.getChildFile ("Guitar AG Reference Captures")
                                    .getChildFile ("sessions")
                                    .getChildFile (requestId);
        sessionDirectory.createDirectory();
        updateOutputLabel();
        writeManifest();
    }

    void chooseRequest()
    {
        fileChooserOpen = true;
        requestChooser = std::make_unique<juce::FileChooser> (
            "Load a Guitar AG capture request", juce::File(), "*.json");
        auto safeThis = juce::Component::SafePointer<CaptureComponent> (this);
        requestChooser->launchAsync (juce::FileBrowserComponent::openMode
                                         | juce::FileBrowserComponent::canSelectFiles,
                                     [safeThis] (const juce::FileChooser& chooser)
                                     {
                                         if (safeThis != nullptr)
                                         {
                                             safeThis->fileChooserOpen = false;
                                             const auto result = chooser.getResult();
                                             if (result.existsAsFile())
                                                 safeThis->loadRequest (result);
                                         }
                                     });
    }

    void chooseInventory()
    {
        fileChooserOpen = true;
        inventoryChooser = std::make_unique<juce::FileChooser> (
            "Load a Guitar AG capture inventory", juce::File(), "*.json");
        auto safeThis = juce::Component::SafePointer<CaptureComponent> (this);
        inventoryChooser->launchAsync (juce::FileBrowserComponent::openMode
                                           | juce::FileBrowserComponent::canSelectFiles,
                                       [safeThis] (const juce::FileChooser& chooser)
                                       {
                                           if (safeThis != nullptr)
                                           {
                                               safeThis->fileChooserOpen = false;
                                               const auto result = chooser.getResult();
                                               if (result.existsAsFile())
                                                   safeThis->loadInventory (result);
                                           }
                                       });
    }

    void loadInventory (const juce::File& file)
    {
        if (isRecording())
        {
            setStatus ("Stop recording before loading an inventory.", true);
            return;
        }

        const auto parsed = juce::JSON::parse (file.loadFileAsString());
        auto* root = parsed.getDynamicObject();
        if (root == nullptr || static_cast<int> (root->getProperty ("schema_version")) != 1)
        {
            setStatus ("That file is not a schema-version 1 capture inventory.", true);
            return;
        }

        const auto itemValue = root->getProperty ("items");
        const auto* itemArray = itemValue.getArray();
        if (itemArray == nullptr || itemArray->isEmpty())
        {
            setStatus ("The inventory contains no capture items.", true);
            return;
        }

        std::vector<InventoryItem> loadedItems;
        for (const auto& value : *itemArray)
        {
            auto* itemObject = value.getDynamicObject();
            if (itemObject == nullptr)
                continue;

            InventoryItem item;
            item.order = static_cast<int> (itemObject->getProperty ("order"));
            item.requestId = itemObject->getProperty ("request_id").toString();
            item.phaseId = itemObject->getProperty ("phase_id").toString();
            item.phaseTitle = itemObject->getProperty ("phase_title").toString();
            item.title = itemObject->getProperty ("title").toString();
            item.why = itemObject->getProperty ("why").toString();
            item.requiredApprovedTakes = juce::jmax (
                1, static_cast<int> (itemObject->getProperty ("required_approved_takes")));

            const auto requestPath = itemObject->getProperty ("request_file").toString();
            item.requestFile = juce::File::isAbsolutePath (requestPath)
                                   ? juce::File (requestPath)
                                   : file.getParentDirectory().getChildFile (requestPath);

            if (item.requestId.isNotEmpty() && item.title.isNotEmpty()
                && item.requestFile.existsAsFile())
                loadedItems.push_back (std::move (item));
        }

        if (loadedItems.empty())
        {
            setStatus ("None of the inventory request files could be found.", true);
            return;
        }

        inventoryTitle = root->getProperty ("title").toString();
        inventoryInstructions = root->getProperty ("instructions").toString();
        inventoryItems = std::move (loadedItems);
        chooseFolderButton.setEnabled (false);
        refreshInventory (true);
        setStatus ("Inventory loaded. Complete Phase 0 and Phase 1, then stop for analysis.",
                   false);
    }

    int getApprovedTakeCount (const InventoryItem& item) const
    {
        const auto request = juce::JSON::parse (item.requestFile.loadFileAsString());
        auto* requestObject = request.getDynamicObject();
        if (requestObject == nullptr)
            return 0;

        const auto directoryPath = requestObject->getProperty ("session_directory").toString();
        if (directoryPath.isEmpty())
            return 0;

        const auto manifestFile = juce::File (directoryPath).getChildFile ("session.json");
        if (! manifestFile.existsAsFile())
            return 0;

        const auto manifest = juce::JSON::parse (manifestFile.loadFileAsString());
        auto* manifestObject = manifest.getDynamicObject();
        if (manifestObject == nullptr
            || manifestObject->getProperty ("request_id").toString() != item.requestId)
            return 0;

        const auto takeValue = manifestObject->getProperty ("takes");
        const auto* takeArray = takeValue.getArray();
        if (takeArray == nullptr)
            return 0;

        int approved = 0;
        for (const auto& takeValueItem : *takeArray)
            if (auto* takeObject = takeValueItem.getDynamicObject();
                takeObject != nullptr
                && takeObject->getProperty ("status").toString() == "approved")
                ++approved;
        return approved;
    }

    void refreshInventory (bool selectFirstIncomplete = false)
    {
        if (inventoryItems.empty())
            return;

        const auto previousSelection = inventoryCombo.getSelectedId();
        auto firstIncomplete = 0;
        auto completed = 0;
        juce::String activePhase;

        updatingInventory = true;
        inventoryCombo.clear (juce::dontSendNotification);
        for (int index = 0; index < static_cast<int> (inventoryItems.size()); ++index)
        {
            const auto& item = inventoryItems[static_cast<size_t> (index)];
            const auto approved = getApprovedTakeCount (item);
            const auto isComplete = approved >= item.requiredApprovedTakes;
            if (isComplete)
                ++completed;
            else if (firstIncomplete == 0)
            {
                firstIncomplete = index + 1;
                activePhase = item.phaseTitle;
            }

            const auto prefix = isComplete ? "[done] " : "[ ] ";
            inventoryCombo.addItem (prefix + juce::String (item.order) + ". " + item.title,
                                    index + 1);
        }

        auto selection = selectFirstIncomplete ? firstIncomplete : previousSelection;
        if (selection <= 0)
            selection = firstIncomplete > 0 ? firstIncomplete : 1;
        inventoryCombo.setSelectedId (selection, juce::dontSendNotification);
        updatingInventory = false;

        if (completed == static_cast<int> (inventoryItems.size()))
            activePhase = "Inventory complete";

        auto phaseCompleted = 0;
        auto phaseTotal = 0;
        if (activePhase != "Inventory complete")
        {
            for (const auto& item : inventoryItems)
            {
                if (item.phaseTitle != activePhase)
                    continue;

                ++phaseTotal;
                if (getApprovedTakeCount (item) >= item.requiredApprovedTakes)
                    ++phaseCompleted;
            }
        }

        const auto progressText = activePhase == "Inventory complete"
                                      ? "Inventory complete — " + juce::String (completed) + "/"
                                            + juce::String (inventoryItems.size())
                                      : activePhase + ": " + juce::String (phaseCompleted) + "/"
                                            + juce::String (phaseTotal) + " — overall "
                                            + juce::String (completed) + "/"
                                            + juce::String (inventoryItems.size());
        inventoryProgressLabel.setText (
            progressText,
            juce::dontSendNotification);
        inventoryProgressLabel.setTooltip (
            (inventoryTitle.isNotEmpty() ? inventoryTitle : "Capture inventory") + "\n"
            + inventoryInstructions);

        if (selectFirstIncomplete && selection > 0)
            loadRequest (inventoryItems[static_cast<size_t> (selection - 1)].requestFile);
    }

    void chooseFolder()
    {
        if (! inventoryItems.empty())
        {
            setStatus ("Inventory requests use fixed session folders so progress remains trackable.",
                       true);
            return;
        }

        if (! takes.empty())
        {
            setStatus ("Choose the session folder before recording the first take.", true);
            return;
        }

        folderChooser = std::make_unique<juce::FileChooser> (
            "Choose a folder for this capture session", sessionDirectory);
        fileChooserOpen = true;
        auto safeThis = juce::Component::SafePointer<CaptureComponent> (this);
        folderChooser->launchAsync (juce::FileBrowserComponent::openMode
                                        | juce::FileBrowserComponent::canSelectDirectories,
                                    [safeThis] (const juce::FileChooser& chooser)
                                    {
                                        if (safeThis != nullptr)
                                        {
                                            safeThis->fileChooserOpen = false;
                                            const auto result = chooser.getResult();
                                            if (result.isDirectory())
                                            {
                                                safeThis->sessionDirectory = result;
                                                safeThis->sessionDirectory.createDirectory();
                                                safeThis->updateOutputLabel();
                                                safeThis->writeRequestCopy();
                                                safeThis->writeManifest();
                                            }
                                        }
                                    });
    }

    void loadRequest (const juce::File& file)
    {
        if (isRecording())
        {
            setStatus ("Stop recording before loading another request.", true);
            return;
        }

        const auto parsed = juce::JSON::parse (file.loadFileAsString());
        auto* object = parsed.getDynamicObject();
        if (object == nullptr || static_cast<int> (object->getProperty ("schema_version")) != 1)
        {
            setStatus ("That file is not a schema-version 1 capture request.", true);
            return;
        }

        const auto newRequestId = object->getProperty ("request_id").toString();
        const auto newTitle = object->getProperty ("title").toString();
        const auto newInstructions = object->getProperty ("instructions").toString();

        if (newRequestId.isEmpty() || newTitle.isEmpty() || newInstructions.isEmpty())
        {
            setStatus ("Request must contain request_id, title, and instructions.", true);
            return;
        }

        requestVar = parsed;
        requestId = safeName (newRequestId);
        requestTitleLabel.setText (newTitle, juce::dontSendNotification);
        instructionsEditor.setText (newInstructions, false);

        const auto context = object->getProperty ("context");
        const auto researchReason = object->getProperty ("research_reason").toString();
        const auto requestedTakes = static_cast<int> (
            object->getProperty ("requested_take_count"));
        auto contextText = requestedTakes > 0
                               ? "Requested takes: " + juce::String (requestedTakes) + "\n"
                               : juce::String();
        if (researchReason.isNotEmpty())
            contextText += "Why: " + researchReason + "\n";
        contextText += context.isVoid() ? "No structured context supplied."
                                        : juce::JSON::toString (context, true);
        contextEditor.setText (contextText,
                               false);

        const auto requestedDirectory = object->getProperty ("session_directory").toString();
        if (requestedDirectory.isNotEmpty())
            sessionDirectory = juce::File (requestedDirectory);
        else
            sessionDirectory = file.getParentDirectory().getSiblingFile ("sessions")
                                   .getChildFile (requestId);

        sessionDirectory.createDirectory();
        loadExistingSession();
        updateOutputLabel();
        writeRequestCopy();
        writeManifest();
        takesList.updateContent();
        if (takes.empty())
        {
            selectedRow = -1;
            takeNotesEditor.clear();
            takesList.deselectAllRows();
            setStatus ("Request loaded. Record several takes and approve the useful ones.", false);
        }
        else
        {
            selectedRow = static_cast<int> (takes.size()) - 1;
            takesList.selectRow (selectedRow);
            setStatus ("Request resumed with " + juce::String (takes.size()) + " existing takes.",
                       false);
        }
        refreshInventory();
    }

    void loadExistingSession()
    {
        takes.clear();
        selectedRow = -1;

        const auto manifestFile = sessionDirectory.getChildFile ("session.json");
        if (! manifestFile.existsAsFile())
            return;

        const auto manifest = juce::JSON::parse (manifestFile.loadFileAsString());
        auto* root = manifest.getDynamicObject();
        if (root == nullptr || root->getProperty ("request_id").toString() != requestId)
            return;

        const auto takeValue = root->getProperty ("takes");
        const auto* takeArray = takeValue.getArray();
        if (takeArray == nullptr)
            return;

        for (const auto& value : *takeArray)
        {
            auto* item = value.getDynamicObject();
            if (item == nullptr)
                continue;

            Take take;
            take.number = static_cast<int> (item->getProperty ("take_number"));
            const auto fileName = item->getProperty ("file").toString();
            if (fileName.isEmpty() || fileName.containsAnyOf ("/\\")
                || ! fileName.endsWithIgnoreCase (".wav"))
                continue;

            take.file = sessionDirectory.getChildFile (fileName);
            take.status = item->getProperty ("status").toString();
            take.notes = item->getProperty ("notes").toString();
            take.recordedAt = item->getProperty ("recorded_at").toString();
            take.sampleRate = static_cast<double> (item->getProperty ("sample_rate_hz"));
            take.samples = static_cast<juce::int64> (item->getProperty ("samples"));
            take.peak = static_cast<float> (static_cast<double> (
                item->getProperty ("peak_linear")));
            take.rms = static_cast<double> (item->getProperty ("rms_linear"));
            take.droppedAudio = static_cast<bool> (item->getProperty ("dropped_audio"));

            if (take.number > 0)
                takes.push_back (std::move (take));
        }
    }

    void startRecording()
    {
        if (isRecording())
            return;

        stopPlayback();

        auto* device = deviceManager.getCurrentAudioDevice();
        if (device == nullptr || device->getActiveInputChannels().countNumberOfSetBits() < 1)
        {
            setStatus ("Enable a guitar input channel in Audio Settings first.", true);
            return;
        }

        sessionDirectory.createDirectory();
        auto nextNumber = 1;
        for (const auto& take : takes)
            nextNumber = juce::jmax (nextNumber, take.number + 1);
        auto destination = sessionDirectory.getChildFile (
            "take-" + juce::String (nextNumber).paddedLeft ('0', 3) + ".wav");

        while (destination.exists())
            destination = sessionDirectory.getChildFile (
                "take-" + juce::String (++nextNumber).paddedLeft ('0', 3) + ".wav");

        std::unique_ptr<juce::OutputStream> outputStream = destination.createOutputStream();
        if (outputStream == nullptr)
        {
            setStatus ("Could not create " + destination.getFullPathName(), true);
            return;
        }

        juce::WavAudioFormat format;
        auto options = juce::AudioFormatWriterOptions()
                           .withSampleRate (device->getCurrentSampleRate())
                           .withChannelLayout (juce::AudioChannelSet::mono())
                           .withBitsPerSample (targetBitDepth);
        auto formatWriter = format.createWriterFor (outputStream, options);
        if (formatWriter == nullptr)
        {
            setStatus ("Could not create a 24-bit mono WAV writer.", true);
            return;
        }

        {
            const juce::ScopedLock lock (writerLock);
            currentFile = destination;
            currentTakeNumber = nextNumber;
            currentSampleRate = device->getCurrentSampleRate();
            recordedSamples = 0;
            recordedSquareSum = 0.0;
            recordedPeak = 0.0f;
            droppedAudio.store (false);
            threadedWriter = std::make_unique<juce::AudioFormatWriter::ThreadedWriter> (
                formatWriter.release(), writerThread, 131072);
            activeWriter = threadedWriter.get();
        }

        recordButton.setEnabled (false);
        playButton.setEnabled (false);
        loadInventoryButton.setEnabled (false);
        loadRequestButton.setEnabled (false);
        chooseFolderButton.setEnabled (false);
        inventoryCombo.setEnabled (false);
        setStatus ("Recording take " + juce::String (currentTakeNumber)
                       + "... Press Space to stop.",
                   false);
    }

    void stopRecording()
    {
        std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> writerToClose;
        Take completed;
        bool hadRecording = false;

        {
            const juce::ScopedLock lock (writerLock);
            if (activeWriter != nullptr)
            {
                hadRecording = true;
                activeWriter = nullptr;
                writerToClose = std::move (threadedWriter);

                completed.number = currentTakeNumber;
                completed.file = currentFile;
                completed.recordedAt = isoTimestamp();
                completed.samples = recordedSamples;
                completed.sampleRate = currentSampleRate;
                completed.peak = recordedPeak;
                completed.rms = recordedSamples > 0
                                  ? std::sqrt (recordedSquareSum
                                               / static_cast<double> (recordedSamples))
                                  : 0.0;
                completed.droppedAudio = droppedAudio.load();
            }
        }

        writerToClose.reset();

        if (! hadRecording)
            return;

        if (completed.samples == 0)
            completed.status = "rejected";

        takes.push_back (std::move (completed));
        selectedRow = static_cast<int> (takes.size()) - 1;
        takesList.updateContent();
        takesList.selectRow (selectedRow);
        writeManifest();

        recordButton.setEnabled (true);
        playButton.setEnabled (true);
        loadInventoryButton.setEnabled (true);
        loadRequestButton.setEnabled (true);
        chooseFolderButton.setEnabled (inventoryItems.empty());
        inventoryCombo.setEnabled (true);

        const auto& take = takes.back();
        const auto peakDb = juce::Decibels::gainToDecibels (take.peak, -100.0f);
        auto message = "Saved take " + juce::String (take.number)
                     + " (peak " + juce::String (peakDb, 1) + " dBFS).";
        if (take.droppedAudio)
            message += " Disk writer overflowed; reject and retake.";
        else if (peakDb > targetPeakDb)
            message += " Hotter than the -12 dBFS reference target.";
        if (take.sampleRate != 48000.0 && take.sampleRate != 96000.0)
            message += " Prefer 48 or 96 kHz for the reference set.";
        setStatus (message, take.droppedAudio);
    }

    bool isRecording() const
    {
        const juce::ScopedLock lock (writerLock);
        return activeWriter != nullptr;
    }

    void playSelected()
    {
        if (selectedRow < 0 || selectedRow >= static_cast<int> (takes.size()))
        {
            setStatus ("Select a take to play.", true);
            return;
        }

        stopPlayback();
        auto reader = std::unique_ptr<juce::AudioFormatReader> (
            formatManager.createReaderFor (takes[static_cast<size_t> (selectedRow)].file));
        if (reader == nullptr)
        {
            setStatus ("Could not open the selected WAV.", true);
            return;
        }

        readerSource = std::make_unique<juce::AudioFormatReaderSource> (reader.release(), true);
        transport.setSource (readerSource.get(), 0, nullptr,
                             readerSource->getAudioFormatReader()->sampleRate);
        transport.setPosition (0.0);
        transport.start();
        setStatus ("Playing take " + juce::String (takes[static_cast<size_t> (selectedRow)].number),
                   false);
    }

    void stopPlayback()
    {
        transport.stop();
        transport.setSource (nullptr);
        readerSource.reset();
    }

    void setSelectedStatus (const juce::String& status)
    {
        if (selectedRow < 0 || selectedRow >= static_cast<int> (takes.size()))
        {
            setStatus ("Select a take first.", true);
            return;
        }

        saveSelectedNotes();
        takes[static_cast<size_t> (selectedRow)].status = status;
        takesList.repaintRow (selectedRow);
        writeManifest();
        refreshInventory();
        setStatus ("Take " + juce::String (takes[static_cast<size_t> (selectedRow)].number)
                       + " is now " + status + ".",
                   false);
    }

    void saveSelectedNotes()
    {
        if (selectedRow < 0 || selectedRow >= static_cast<int> (takes.size()))
            return;

        takes[static_cast<size_t> (selectedRow)].notes = takeNotesEditor.getText();
        writeManifest();
        takesList.repaintRow (selectedRow);
    }

    void updateOutputLabel()
    {
        outputLabel.setText (sessionDirectory.getFullPathName(), juce::dontSendNotification);
        outputLabel.setTooltip (sessionDirectory.getFullPathName());
    }

    void writeRequestCopy()
    {
        if (! sessionDirectory.isDirectory())
            return;

        sessionDirectory.getChildFile ("request.json")
                        .replaceWithText (juce::JSON::toString (requestVar, true) + "\n");
    }

    void writeManifest()
    {
        if (! sessionDirectory.isDirectory())
            return;

        auto root = std::make_unique<juce::DynamicObject>();
        root->setProperty ("schema_version", 1);
        root->setProperty ("request_id", requestId);
        root->setProperty ("updated_at", isoTimestamp());
        root->setProperty ("request", requestVar);

        auto audio = std::make_unique<juce::DynamicObject>();
        if (auto* device = deviceManager.getCurrentAudioDevice())
        {
            audio->setProperty ("device_type", deviceManager.getCurrentAudioDeviceType());
            audio->setProperty ("device_name", device->getName());
            audio->setProperty ("sample_rate_hz", device->getCurrentSampleRate());
            audio->setProperty ("buffer_size_samples", device->getCurrentBufferSizeSamples());
            audio->setProperty ("active_input_channels",
                                device->getActiveInputChannels().countNumberOfSetBits());
        }
        audio->setProperty ("captured_channel", "first enabled input");
        audio->setProperty ("channels", 1);
        audio->setProperty ("bit_depth", targetBitDepth);
        root->setProperty ("audio_setup", juce::var (audio.release()));

        juce::Array<juce::var> takeArray;
        for (const auto& take : takes)
        {
            auto item = std::make_unique<juce::DynamicObject>();
            item->setProperty ("take_number", take.number);
            item->setProperty ("file", take.file.getFileName());
            item->setProperty ("status", take.status);
            item->setProperty ("notes", take.notes);
            item->setProperty ("recorded_at", take.recordedAt);
            item->setProperty ("sample_rate_hz", take.sampleRate);
            item->setProperty ("samples", static_cast<juce::int64> (take.samples));
            item->setProperty ("duration_seconds",
                               take.sampleRate > 0.0 ? take.samples / take.sampleRate : 0.0);
            item->setProperty ("peak_linear", take.peak);
            item->setProperty ("peak_dbfs",
                               juce::Decibels::gainToDecibels (take.peak, -100.0f));
            item->setProperty ("rms_linear", take.rms);
            item->setProperty ("rms_dbfs",
                               juce::Decibels::gainToDecibels (take.rms, -100.0));
            item->setProperty ("dropped_audio", take.droppedAudio);
            takeArray.add (juce::var (item.release()));
        }
        root->setProperty ("takes", takeArray);

        const juce::var manifest (root.release());
        sessionDirectory.getChildFile ("session.json")
                        .replaceWithText (juce::JSON::toString (manifest, true) + "\n");
    }

    void setStatus (const juce::String& message, bool isError)
    {
        statusLabelComponent.setColour (
            juce::Label::textColourId,
            isError ? juce::Colours::salmon : juce::Colours::lightgreen);
        statusLabelComponent.setText (message, juce::dontSendNotification);
        statusLabelComponent.setTooltip (message);
    }

    int getNumRows() override
    {
        return static_cast<int> (takes.size());
    }

    void paintListBoxItem (int rowNumber,
                           juce::Graphics& g,
                           int width,
                           int height,
                           bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll (juce::Colour::fromRGB (53, 76, 99));

        if (rowNumber < 0 || rowNumber >= static_cast<int> (takes.size()))
            return;

        const auto& take = takes[static_cast<size_t> (rowNumber)];
        const auto peakDb = juce::Decibels::gainToDecibels (take.peak, -100.0f);
        const auto missing = take.file.existsAsFile() ? juce::String()
                                                      : juce::String ("  [MISSING WAV]");
        g.setColour (take.status == "approved" ? juce::Colours::lightgreen
                                               : take.status == "rejected" ? juce::Colours::salmon
                                                                            : juce::Colours::white);
        g.setFont (juce::FontOptions (14.0f));
        g.drawText ("Take " + juce::String (take.number) + "  [" + statusLabel (take.status)
                        + "]  peak " + juce::String (peakDb, 1) + " dBFS" + missing,
                    8, 0, width - 16, height, juce::Justification::centredLeft, true);
    }

    void selectedRowsChanged (int lastRowSelected) override
    {
        selectedRow = lastRowSelected;
        if (selectedRow >= 0 && selectedRow < static_cast<int> (takes.size()))
            takeNotesEditor.setText (takes[static_cast<size_t> (selectedRow)].notes, false);
        else
            takeNotesEditor.clear();
    }

    void timerCallback() override
    {
        updateSpaceShortcut();

        const auto peak = inputPeak.exchange (0.0f);
        const auto db = juce::Decibels::gainToDecibels (peak, -100.0f);
        meterLabel.setText ("Input: " + juce::String (db, 1) + " dBFS",
                            juce::dontSendNotification);
        meterLabel.setColour (juce::Label::textColourId,
                              db > targetPeakDb ? juce::Colours::orange
                                                : juce::Colours::lightgreen);

        if (! transport.isPlaying() && readerSource != nullptr)
            stopPlayback();
    }

    void updateSpaceShortcut()
    {
        const auto spaceIsDown = juce::KeyPress::isKeyCurrentlyDown (
            juce::KeyPress::spaceKey);

        if (! juce::Process::isForegroundProcess())
        {
            spaceWasDown = spaceIsDown;
            return;
        }

        if (spaceIsDown && ! spaceWasDown)
        {
            auto* focused = juce::Component::getCurrentlyFocusedComponent();
            const auto editingNotes = focused != nullptr
                                      && (focused == &takeNotesEditor
                                          || takeNotesEditor.isParentOf (focused));
            const auto editingDevice = focused != nullptr
                                       && (focused == &deviceSelector
                                           || deviceSelector.isParentOf (focused));

            if (! editingNotes && ! editingDevice && ! fileChooserOpen)
            {
                if (isRecording())
                    stopRecording();
                else
                    startRecording();
            }
        }

        spaceWasDown = spaceIsDown;
    }

    void audioDeviceAboutToStart (juce::AudioIODevice* device) override
    {
        const auto outputChannels = juce::jmax (
            1, device->getActiveOutputChannels().countNumberOfSetBits());
        playbackBuffer.setSize (outputChannels,
                                juce::jmax (8192, device->getCurrentBufferSizeSamples() * 2),
                                false, false, true);
        transport.prepareToPlay (device->getCurrentBufferSizeSamples(),
                                 device->getCurrentSampleRate());
    }

    void audioDeviceStopped() override
    {
        transport.releaseResources();
    }

    void audioDeviceIOCallbackWithContext (
        const float* const* inputChannelData,
        int numInputChannels,
        float* const* outputChannelData,
        int numOutputChannels,
        int numSamples,
        const juce::AudioIODeviceCallbackContext&) override
    {
        const float* input = numInputChannels > 0 ? inputChannelData[0] : nullptr;
        if (input != nullptr)
        {
            float blockPeak = 0.0f;
            for (int sample = 0; sample < numSamples; ++sample)
                blockPeak = juce::jmax (blockPeak, std::abs (input[sample]));

            auto oldPeak = inputPeak.load();
            while (blockPeak > oldPeak && ! inputPeak.compare_exchange_weak (oldPeak, blockPeak))
            {
            }

            const juce::ScopedLock lock (writerLock);
            if (activeWriter != nullptr)
            {
                const float* channels[] { input };
                if (! activeWriter->write (channels, numSamples))
                    droppedAudio.store (true);

                for (int sample = 0; sample < numSamples; ++sample)
                {
                    const auto value = input[sample];
                    recordedPeak = juce::jmax (recordedPeak, std::abs (value));
                    recordedSquareSum += static_cast<double> (value) * value;
                }
                recordedSamples += numSamples;
            }
        }

        for (int channel = 0; channel < numOutputChannels; ++channel)
            if (outputChannelData[channel] != nullptr)
                juce::FloatVectorOperations::clear (outputChannelData[channel], numSamples);

        if (numOutputChannels > 0 && numSamples <= playbackBuffer.getNumSamples())
        {
            const auto channelsToRender = juce::jmin (numOutputChannels,
                                                      playbackBuffer.getNumChannels());
            playbackBuffer.clear();
            juce::AudioSourceChannelInfo info (&playbackBuffer, 0, numSamples);
            transport.getNextAudioBlock (info);

            for (int channel = 0; channel < channelsToRender; ++channel)
                if (outputChannelData[channel] != nullptr)
                    juce::FloatVectorOperations::copy (outputChannelData[channel],
                                                       playbackBuffer.getReadPointer (channel),
                                                       numSamples);
        }
    }

    juce::ApplicationProperties properties;
    juce::AudioDeviceManager deviceManager;
    juce::AudioDeviceSelectorComponent deviceSelector;
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transport;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioBuffer<float> playbackBuffer;

    juce::TimeSliceThread writerThread;
    mutable juce::CriticalSection writerLock;
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter;
    juce::AudioFormatWriter::ThreadedWriter* activeWriter = nullptr;

    std::atomic<float> inputPeak { 0.0f };
    std::atomic<bool> droppedAudio { false };
    int64_t recordedSamples = 0;
    double recordedSquareSum = 0.0;
    float recordedPeak = 0.0f;
    double currentSampleRate = 0.0;
    int currentTakeNumber = 0;
    juce::File currentFile;

    juce::var requestVar;
    juce::String requestId;
    juce::File sessionDirectory;
    std::vector<Take> takes;
    int selectedRow = -1;
    juce::String inventoryTitle;
    juce::String inventoryInstructions;
    std::vector<InventoryItem> inventoryItems;
    bool updatingInventory = false;
    bool spaceWasDown = false;
    bool fileChooserOpen = false;

    juce::Rectangle<int> devicePanelBounds;
    juce::Label titleLabel;
    juce::Label requestTitleLabel;
    juce::Label outputLabel;
    juce::Label statusLabelComponent;
    juce::Label meterLabel;
    juce::Label takeNotesLabel;
    juce::Label inventoryProgressLabel;
    juce::TextEditor instructionsEditor;
    juce::TextEditor contextEditor;
    juce::TextEditor takeNotesEditor;
    juce::TextButton loadRequestButton;
    juce::TextButton loadInventoryButton;
    juce::TextButton chooseFolderButton;
    juce::TextButton recordButton;
    juce::TextButton stopButton;
    juce::TextButton playButton;
    juce::TextButton approveButton;
    juce::TextButton rejectButton;
    juce::TextButton candidateButton;
    juce::TextButton saveNotesButton;
    juce::ListBox takesList;
    juce::ComboBox inventoryCombo;
    std::unique_ptr<juce::FileChooser> requestChooser;
    std::unique_ptr<juce::FileChooser> inventoryChooser;
    std::unique_ptr<juce::FileChooser> folderChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CaptureComponent)
};

class CaptureWindow final : public juce::DocumentWindow
{
public:
    explicit CaptureWindow (const juce::StringArray& commandLine)
        : DocumentWindow ("Guitar AG Reference Capture",
                          juce::Colour::fromRGB (22, 25, 29),
                          DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar (true);
        setResizable (true, true);
        setResizeLimits (940, 650, 1800, 1200);
        setContentOwned (new CaptureComponent (commandLine), true);
        centreWithSize (getWidth(), getHeight());
        setVisible (true);
    }

    void closeButtonPressed() override
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
};

class CaptureApplication final : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override
    {
        return "Guitar AG Reference Capture";
    }

    const juce::String getApplicationVersion() override
    {
        return "0.1.0";
    }

    bool moreThanOneInstanceAllowed() override
    {
        return false;
    }

    void initialise (const juce::String&) override
    {
        window = std::make_unique<CaptureWindow> (getCommandLineParameterArray());
    }

    void shutdown() override
    {
        window.reset();
    }

    void systemRequestedQuit() override
    {
        quit();
    }

private:
    std::unique_ptr<CaptureWindow> window;
};
}

START_JUCE_APPLICATION (CaptureApplication)
