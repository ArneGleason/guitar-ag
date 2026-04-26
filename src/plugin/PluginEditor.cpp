#include "PluginEditor.h"

#include "BuildInfo.h"

GuitarAgAudioProcessorEditor::GuitarAgAudioProcessorEditor (GuitarAgAudioProcessor& processor)
    : AudioProcessorEditor (&processor),
      audioProcessor (processor)
{
    const auto configureLabel = [this] (juce::Label& label, const juce::String& text)
    {
        label.setText (text, juce::dontSendNotification);
        label.setColour (juce::Label::textColourId, juce::Colour (0xffcbd4dc));
        label.setJustificationType (juce::Justification::centredLeft);
        addAndMakeVisible (label);
    };

    const auto configureSlider = [this] (juce::Slider& slider, juce::Colour colour)
    {
        slider.setSliderStyle (juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 72, 24);
        slider.setColour (juce::Slider::trackColourId, colour);
        slider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffe8edf2));
        slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe8edf2));
        slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff202832));
        addAndMakeVisible (slider);
    };

    configureInfoButton (projectInfoButton,
                         "Guitar AG is a lightweight modeled electric-guitar instrument: no samples, no amp/cab suite, "
                         "and no claim of exact guitar emulation. The goal is a useful clean DI stand-in with real dynamics, "
                         "small footprint, and room for expressive MIDI/MPE behavior as the model grows.");

    configureSectionButton (setupSectionButton, "Setup");
    configureSectionButton (pickupSectionButton, "Pickup");
    configureSectionButton (performanceSectionButton, "Performance");
    configureSectionButton (articulationSectionButton, "Articulation");

    setupSectionButton.onClick = [this]
    {
        setupExpanded = ! setupExpanded;
        updateSectionVisibility();
    };

    pickupSectionButton.onClick = [this]
    {
        pickupExpanded = ! pickupExpanded;
        updateSectionVisibility();
    };

    performanceSectionButton.onClick = [this]
    {
        performanceExpanded = ! performanceExpanded;
        updateSectionVisibility();
    };

    articulationSectionButton.onClick = [this]
    {
        articulationExpanded = ! articulationExpanded;
        updateSectionVisibility();
    };

    configureLabel (sustainLabel, "Sustain");
    configureSlider (sustainSlider, juce::Colour (0xff6fb1ff));
    configureInfoButton (sustainInfoButton,
                         "0% keeps a shorter natural decay. 100% relaxes late-mode damping so held notes keep ringing longer.");

    configureLabel (stringAgeLabel, "String Age");
    configureSlider (stringAgeSlider, juce::Colour (0xff9ccf8a));
    configureInfoButton (stringAgeInfoButton,
                         "0% is a bright new-string response. 100% dulls upper partials and slightly shortens the high-frequency tail.");

    configureLabel (bridgeIntonationLabel, "Bridge Intonation");
    configureSlider (bridgeIntonationSlider, juce::Colour (0xffd1b371));
    configureInfoButton (bridgeIntonationInfoButton,
                         "0% is ideal saddle compensation. 100% applies a deterministic bad-setup offset: open strings stay in tune, "
                         "fretted notes drift more as the assigned fret gets higher.");

    configureLabel (pickupModelLabel, "Pickup Model");
    configureInfoButton (pickupModelInfoButton,
                         "Single Coil is narrow and bright. Humbucker sums two nearby coils for a thicker smoother tone. "
                         "Humbucker OOP subtracts the coils for a thinner notched tone.");

    pickupModelBox.addItem ("Single Coil", 1);
    pickupModelBox.addItem ("Humbucker", 2);
    pickupModelBox.addItem ("Humbucker OOP", 3);
    pickupModelBox.setColour (juce::ComboBox::textColourId, juce::Colour (0xffe8edf2));
    pickupModelBox.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff202832));
    pickupModelBox.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff65717c));
    pickupModelBox.setColour (juce::ComboBox::arrowColourId, juce::Colour (0xffe8edf2));
    addAndMakeVisible (pickupModelBox);

    configureLabel (pickupPositionLabel, "Pickup Position");
    configureSlider (pickupPositionSlider, juce::Colour (0xff75d7d1));
    configureInfoButton (pickupPositionInfoButton,
                         "0% reads near the bridge for brighter upper partials. 100% reads toward the neck for a fuller tone. "
                         "Markers show string-fraction landmarks where pickup position changes harmonic balance.");

    for (auto* marker : { &pickupSixthLabel, &pickupFifthLabel, &pickupQuarterLabel, &pickupThirdLabel })
    {
        marker->setColour (juce::Label::textColourId, juce::Colour (0xff9aa8b5));
        marker->setJustificationType (juce::Justification::centred);
        addAndMakeVisible (*marker);
    }

    pickupSixthLabel.setText ("1/6", juce::dontSendNotification);
    pickupFifthLabel.setText ("1/5", juce::dontSendNotification);
    pickupQuarterLabel.setText ("1/4", juce::dontSendNotification);
    pickupThirdLabel.setText ("1/3", juce::dontSendNotification);
    pickupSixthLabel.setTooltip ("Pickup landmark at one sixth of the speaking string length.");
    pickupFifthLabel.setTooltip ("Pickup landmark at one fifth of the speaking string length.");
    pickupQuarterLabel.setTooltip ("Pickup landmark at one quarter of the speaking string length.");
    pickupThirdLabel.setTooltip ("Pickup landmark at one third of the speaking string length.");

    configureLabel (fretPressureLabel, "Fret Pressure");
    configureSlider (fretPressureSlider, juce::Colour (0xffd7a0ff));
    configureInfoButton (fretPressureInfoButton,
                         "0% adds no fretting-pressure pitch error. 100% models over-gripping/mid-fret pressure by bending fretted notes sharp. "
                         "Open strings are unaffected; lower strings and higher frets receive more shift.");

    configureLabel (pickStiffnessLabel, "Pick Stiffness");
    configureSlider (pickStiffnessSlider, juce::Colour (0xffffc56f));
    configureInfoButton (pickStiffnessInfoButton,
                         "0% is a very soft/flexible release. 100% is a hard pick with a faster edge and stronger upper-mode excitation.");

    configureLabel (pickTextureLabel, "Pick Texture");
    configureSlider (pickTextureSlider, juce::Colour (0xff7bd88f));
    configureInfoButton (pickTextureInfoButton,
                         "0% is close to a frictionless smooth release. Around 50% is a normal plastic-pick snap. "
                         "The top range adds rougher coin-like contact texture.");

    configureLabel (palmMuteLabel, "Palm Mute");
    configureSlider (palmMuteSlider, juce::Colour (0xfff28b82));
    configureInfoButton (palmMuteInfoButton,
                         "0% leaves strings open. 100% applies heavy bridge-side palm damping, leaving mostly pick contact and a very short ring. "
                         "The lower half is curved for fine light-mute control.");

    configureLabel (harmonicTouchLabel, "Harmonic Touch");
    configureSlider (harmonicTouchSlider, juce::Colour (0xffc39cff));
    configureInfoButton (harmonicTouchInfoButton,
                         "0-25% is off. 25-50% targets the 1/4 touch harmonic, 50-75% targets 1/3, and 75-100% targets 1/2. "
                         "Higher within each band means a cleaner touch point.");

    for (auto* marker : { &harmonicQuarterLabel, &harmonicThirdLabel, &harmonicHalfLabel })
    {
        marker->setColour (juce::Label::textColourId, juce::Colour (0xff9aa8b5));
        marker->setJustificationType (juce::Justification::centred);
        addAndMakeVisible (*marker);
    }

    harmonicQuarterLabel.setText ("1/4", juce::dontSendNotification);
    harmonicThirdLabel.setText ("1/3", juce::dontSendNotification);
    harmonicHalfLabel.setText ("1/2", juce::dontSendNotification);
    harmonicQuarterLabel.setTooltip ("Natural harmonic band: touch near one quarter of the speaking string.");
    harmonicThirdLabel.setTooltip ("Natural harmonic band: touch near one third of the speaking string.");
    harmonicHalfLabel.setTooltip ("Natural harmonic band: touch near the midpoint for the octave harmonic.");

    sustainAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                            GuitarAgAudioProcessor::tailSustainParameterId,
                                                            sustainSlider);
    stringAgeAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                             GuitarAgAudioProcessor::stringAgeParameterId,
                                                             stringAgeSlider);
    bridgeIntonationAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                    GuitarAgAudioProcessor::bridgeIntonationParameterId,
                                                                    bridgeIntonationSlider);
    fretPressureAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                 GuitarAgAudioProcessor::fretPressureParameterId,
                                                                 fretPressureSlider);
    pickupModelAttachment = std::make_unique<ComboBoxAttachment> (audioProcessor.getValueTreeState(),
                                                                 GuitarAgAudioProcessor::pickupModelParameterId,
                                                                 pickupModelBox);
    pickupPositionAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                  GuitarAgAudioProcessor::pickupPositionParameterId,
                                                                  pickupPositionSlider);
    pickStiffnessAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                  GuitarAgAudioProcessor::pickStiffnessParameterId,
                                                                  pickStiffnessSlider);
    pickTextureAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                               GuitarAgAudioProcessor::pickTextureParameterId,
                                                               pickTextureSlider);
    palmMuteAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                            GuitarAgAudioProcessor::palmMuteParameterId,
                                                            palmMuteSlider);
    harmonicTouchAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                 GuitarAgAudioProcessor::harmonicTouchParameterId,
                                                                 harmonicTouchSlider);

    updateSectionVisibility();
}

void GuitarAgAudioProcessorEditor::paint (juce::Graphics& graphics)
{
    juce::ignoreUnused (audioProcessor);

    graphics.fillAll (juce::Colour (0xff171a1f));

    auto bounds = getLocalBounds().reduced (24);
    graphics.setColour (juce::Colour (0xffe8edf2));
    graphics.setFont (juce::FontOptions (24.0f, juce::Font::bold));
    graphics.drawFittedText ("Guitar AG", bounds.removeFromTop (40), juce::Justification::centredLeft, 1);

    graphics.setColour (juce::Colour (0xff9aa8b5));
    graphics.setFont (juce::FontOptions (15.0f));
    graphics.drawFittedText ("MVP string voice: MIDI-triggered plucked model", bounds.removeFromTop (28),
                             juce::Justification::centredLeft, 1);
    bounds.removeFromTop (juce::jmax (0, getHeight() - 146));
    graphics.drawFittedText ("MPE routing is intentionally not implemented yet.", bounds,
                             juce::Justification::centredLeft, 2);

    graphics.setColour (juce::Colour (0xff65717c));
    graphics.setFont (juce::FontOptions (12.0f));
    const juce::String buildText = "v" JucePlugin_VersionString " / " GUITAR_AG_MODEL_LABEL " / " GUITAR_AG_GIT_COMMIT;
    graphics.drawFittedText (buildText, getLocalBounds().reduced (24).removeFromBottom (20),
                             juce::Justification::centredLeft, 1);
}

void GuitarAgAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (24);
    const auto titleBounds = bounds.removeFromTop (40);
    projectInfoButton.setBounds (titleBounds.getX() + 116, titleBounds.getY() + 8, 22, 22);
    bounds.removeFromTop (38);

    setupSectionButton.setBounds (bounds.removeFromTop (26));

    if (setupExpanded)
    {
        auto sustainBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (sustainBounds, sustainLabel, sustainInfoButton);
        sustainSlider.setBounds (sustainBounds);

        auto ageBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (ageBounds, stringAgeLabel, stringAgeInfoButton);
        stringAgeSlider.setBounds (ageBounds);

        auto intonationBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (intonationBounds, bridgeIntonationLabel, bridgeIntonationInfoButton);
        bridgeIntonationSlider.setBounds (intonationBounds);
    }

    bounds.removeFromTop (8);
    pickupSectionButton.setBounds (bounds.removeFromTop (26));

    if (pickupExpanded)
    {
        auto pickupModelBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (pickupModelBounds, pickupModelLabel, pickupModelInfoButton);
        pickupModelBox.setBounds (pickupModelBounds.reduced (0, 4));

        auto pickupPositionBounds = bounds.removeFromTop (54);
        layoutLabelAndInfo (pickupPositionBounds, pickupPositionLabel, pickupPositionInfoButton);
        const auto pickupSliderBounds = pickupPositionBounds.removeFromTop (34);
        pickupPositionSlider.setBounds (pickupSliderBounds);

        auto pickupMarkerTrackBounds = pickupSliderBounds;
        pickupMarkerTrackBounds.removeFromRight (78);
        const auto pickupMarkerY = pickupPositionBounds.getY() - 1;
        constexpr auto pickupMarkerWidth = 32;
        const auto pickupMarkerX = [&pickupMarkerTrackBounds] (float position)
        {
            const auto normalized = (position - 0.055f) / (0.335f - 0.055f);
            return pickupMarkerTrackBounds.getX()
                 + juce::roundToInt (juce::jlimit (0.0f, 1.0f, normalized)
                                      * static_cast<float> (pickupMarkerTrackBounds.getWidth()))
                 - pickupMarkerWidth / 2;
        };

        pickupSixthLabel.setBounds (pickupMarkerX (1.0f / 6.0f), pickupMarkerY, pickupMarkerWidth, 18);
        pickupFifthLabel.setBounds (pickupMarkerX (1.0f / 5.0f), pickupMarkerY, pickupMarkerWidth, 18);
        pickupQuarterLabel.setBounds (pickupMarkerX (1.0f / 4.0f), pickupMarkerY, pickupMarkerWidth, 18);
        pickupThirdLabel.setBounds (pickupMarkerX (1.0f / 3.0f), pickupMarkerY, pickupMarkerWidth, 18);
    }

    bounds.removeFromTop (8);
    performanceSectionButton.setBounds (bounds.removeFromTop (26));

    if (performanceExpanded)
    {
        auto pressureBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (pressureBounds, fretPressureLabel, fretPressureInfoButton);
        fretPressureSlider.setBounds (pressureBounds);
    }

    bounds.removeFromTop (8);
    articulationSectionButton.setBounds (bounds.removeFromTop (26));

    if (articulationExpanded)
    {
        auto stiffnessBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (stiffnessBounds, pickStiffnessLabel, pickStiffnessInfoButton);
        pickStiffnessSlider.setBounds (stiffnessBounds);

        auto textureBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (textureBounds, pickTextureLabel, pickTextureInfoButton);
        pickTextureSlider.setBounds (textureBounds);

        auto palmMuteBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (palmMuteBounds, palmMuteLabel, palmMuteInfoButton);
        palmMuteSlider.setBounds (palmMuteBounds);

        auto harmonicTouchBounds = bounds.removeFromTop (54);
        layoutLabelAndInfo (harmonicTouchBounds, harmonicTouchLabel, harmonicTouchInfoButton);
        const auto harmonicSliderBounds = harmonicTouchBounds.removeFromTop (34);
        harmonicTouchSlider.setBounds (harmonicSliderBounds);

        auto markerTrackBounds = harmonicSliderBounds;
        markerTrackBounds.removeFromRight (78);
        const auto markerY = harmonicTouchBounds.getY() - 1;
        constexpr auto markerWidth = 32;
        const auto markerX = [&markerTrackBounds] (float normalized)
        {
            return markerTrackBounds.getX()
                 + juce::roundToInt (normalized * static_cast<float> (markerTrackBounds.getWidth()))
                 - markerWidth / 2;
        };

        harmonicQuarterLabel.setBounds (markerX (0.50f), markerY, markerWidth, 18);
        harmonicThirdLabel.setBounds (markerX (0.75f), markerY, markerWidth, 18);
        harmonicHalfLabel.setBounds (markerX (1.0f), markerY, markerWidth, 18);
    }
}

void GuitarAgAudioProcessorEditor::configureSectionButton (juce::TextButton& button, const juce::String& title)
{
    button.setButtonText (title);
    button.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff202832));
    button.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff283340));
    button.setColour (juce::TextButton::textColourOffId, juce::Colour (0xffe8edf2));
    button.setColour (juce::TextButton::textColourOnId, juce::Colour (0xffe8edf2));
    button.setTriggeredOnMouseDown (false);
    addAndMakeVisible (button);
}

void GuitarAgAudioProcessorEditor::configureInfoButton (juce::TextButton& button, const juce::String& tooltip)
{
    button.setButtonText ("i");
    button.setTooltip (tooltip);
    button.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff283340));
    button.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff334151));
    button.setColour (juce::TextButton::textColourOffId, juce::Colour (0xffcbd4dc));
    button.setColour (juce::TextButton::textColourOnId, juce::Colour (0xffe8edf2));
    button.setTriggeredOnMouseDown (false);
    addAndMakeVisible (button);
}

void GuitarAgAudioProcessorEditor::layoutLabelAndInfo (juce::Rectangle<int>& row,
                                                       juce::Label& label,
                                                       juce::TextButton& infoButton) noexcept
{
    auto labelArea = row.removeFromLeft (134);
    label.setBounds (labelArea.removeFromLeft (108));
    infoButton.setBounds (labelArea.removeFromLeft (22).reduced (2, 8));
}

void GuitarAgAudioProcessorEditor::updateSectionVisibility()
{
    setupSectionButton.setButtonText (getSectionTitle ("Setup", setupExpanded));
    pickupSectionButton.setButtonText (getSectionTitle ("Pickup", pickupExpanded));
    performanceSectionButton.setButtonText (getSectionTitle ("Performance", performanceExpanded));
    articulationSectionButton.setButtonText (getSectionTitle ("Articulation", articulationExpanded));

    for (auto* component : { static_cast<juce::Component*> (&sustainLabel),
                             static_cast<juce::Component*> (&sustainInfoButton),
                             static_cast<juce::Component*> (&sustainSlider),
                             static_cast<juce::Component*> (&stringAgeLabel),
                             static_cast<juce::Component*> (&stringAgeInfoButton),
                             static_cast<juce::Component*> (&stringAgeSlider),
                             static_cast<juce::Component*> (&bridgeIntonationLabel),
                             static_cast<juce::Component*> (&bridgeIntonationInfoButton),
                             static_cast<juce::Component*> (&bridgeIntonationSlider) })
        component->setVisible (setupExpanded);

    for (auto* component : { static_cast<juce::Component*> (&pickupModelLabel),
                             static_cast<juce::Component*> (&pickupModelInfoButton),
                             static_cast<juce::Component*> (&pickupModelBox),
                             static_cast<juce::Component*> (&pickupPositionLabel),
                             static_cast<juce::Component*> (&pickupPositionInfoButton),
                             static_cast<juce::Component*> (&pickupPositionSlider),
                             static_cast<juce::Component*> (&pickupSixthLabel),
                             static_cast<juce::Component*> (&pickupFifthLabel),
                             static_cast<juce::Component*> (&pickupQuarterLabel),
                             static_cast<juce::Component*> (&pickupThirdLabel) })
        component->setVisible (pickupExpanded);

    for (auto* component : { static_cast<juce::Component*> (&fretPressureLabel),
                             static_cast<juce::Component*> (&fretPressureInfoButton),
                             static_cast<juce::Component*> (&fretPressureSlider) })
        component->setVisible (performanceExpanded);

    for (auto* component : { static_cast<juce::Component*> (&pickStiffnessLabel),
                             static_cast<juce::Component*> (&pickStiffnessInfoButton),
                             static_cast<juce::Component*> (&pickStiffnessSlider),
                             static_cast<juce::Component*> (&pickTextureLabel),
                             static_cast<juce::Component*> (&pickTextureInfoButton),
                             static_cast<juce::Component*> (&pickTextureSlider),
                             static_cast<juce::Component*> (&palmMuteLabel),
                             static_cast<juce::Component*> (&palmMuteInfoButton),
                             static_cast<juce::Component*> (&palmMuteSlider),
                             static_cast<juce::Component*> (&harmonicTouchLabel),
                             static_cast<juce::Component*> (&harmonicTouchInfoButton),
                             static_cast<juce::Component*> (&harmonicTouchSlider),
                             static_cast<juce::Component*> (&harmonicQuarterLabel),
                             static_cast<juce::Component*> (&harmonicThirdLabel),
                             static_cast<juce::Component*> (&harmonicHalfLabel) })
        component->setVisible (articulationExpanded);

    setSize (500, getPreferredHeight());
    resized();
    repaint();
}

int GuitarAgAudioProcessorEditor::getPreferredHeight() const noexcept
{
    auto controlsHeight = 78 + 26 + 8 + 26 + 8 + 26 + 8 + 26;

    if (setupExpanded)
        controlsHeight += 108;

    if (pickupExpanded)
        controlsHeight += 90;

    if (performanceExpanded)
        controlsHeight += 36;

    if (articulationExpanded)
        controlsHeight += 162;

    return controlsHeight + 72;
}

juce::String GuitarAgAudioProcessorEditor::getSectionTitle (const juce::String& title, bool expanded) const
{
    return juce::String (expanded ? "- " : "+ ") + title;
}
