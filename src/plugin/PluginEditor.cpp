#include "PluginEditor.h"

#include "BuildInfo.h"

namespace
{

class InfoPopoverContent final : public juce::Component
{
public:
    explicit InfoPopoverContent (juce::String infoText)
    {
        message.setMultiLine (true);
        message.setReadOnly (true);
        message.setScrollbarsShown (false);
        message.setCaretVisible (false);
        message.setPopupMenuEnabled (false);
        message.setText (infoText, false);
        message.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xff202832));
        message.setColour (juce::TextEditor::textColourId, juce::Colour (0xffe8edf2));
        message.setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
        message.setColour (juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
        message.setFont (juce::FontOptions (14.0f));
        addAndMakeVisible (message);

        const auto height = juce::jlimit (104, 230, 62 + (infoText.length() / 46) * 18);
        setSize (360, height);
    }

    void paint (juce::Graphics& graphics) override
    {
        graphics.setColour (juce::Colour (0xff202832));
        graphics.fillRoundedRectangle (getLocalBounds().toFloat(), 7.0f);
        graphics.setColour (juce::Colour (0xff65717c));
        graphics.drawRoundedRectangle (getLocalBounds().toFloat().reduced (0.5f), 7.0f, 1.0f);
    }

    void resized() override
    {
        message.setBounds (getLocalBounds().reduced (14, 12));
    }

private:
    juce::TextEditor message;
};

} // namespace

GuitarAgAudioProcessorEditor::GuitarAgAudioProcessorEditor (GuitarAgAudioProcessor& processor)
    : AudioProcessorEditor (&processor),
      audioProcessor (processor)
{
    const auto configureLabel = [this] (juce::Label& label, const juce::String& text)
    {
        label.setText (text, juce::dontSendNotification);
        label.setColour (juce::Label::textColourId, juce::Colour (0xffd6dee7));
        label.setJustificationType (juce::Justification::centredLeft);
        addAndMakeVisible (label);
    };

    const auto configureSlider = [this] (juce::Slider& slider, juce::Colour colour)
    {
        slider.setSliderStyle (juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 70, 24);
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
    configureSectionButton (performanceSectionButton, "Perform");
    configureSectionButton (vibratoSectionButton, "Vibrato");
    configureSectionButton (mpeSectionButton, "MPE");
    configureSectionButton (whammySectionButton, "Whammy");
    configureSectionButton (articulationSectionButton, "Artic");

    setupSectionButton.onClick = [this]
    {
        setActivePage (0);
    };

    pickupSectionButton.onClick = [this]
    {
        setActivePage (1);
    };

    performanceSectionButton.onClick = [this]
    {
        setActivePage (2);
    };

    vibratoSectionButton.onClick = [this]
    {
        setActivePage (3);
    };

    mpeSectionButton.onClick = [this]
    {
        setActivePage (4);
    };

    whammySectionButton.onClick = [this]
    {
        setActivePage (5);
    };

    articulationSectionButton.onClick = [this]
    {
        setActivePage (6);
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
                         "Open strings are unaffected; lower strings and higher frets receive more shift. The top of the range is intentionally obvious.");

    configureLabel (aftertouchBendLabel, "Aftertouch Bend");
    configureSlider (aftertouchBendSlider, juce::Colour (0xffffb36f));
    configureInfoButton (aftertouchBendInfoButton,
                         "Poly/key aftertouch bends the matching active note by this many semitones at full pressure. "
                         "Default is +2 for a normal upward bend; 0 disables it. Negative values are allowed for experimental downward pressure bends.");

    configureLabel (lookaheadLabel, "Lookahead");
    configureInfoButton (lookaheadInfoButton,
                         "Off keeps live response. 150 ms and 250 ms delay note events internally, report matching plugin latency, "
                         "and create room for finger approach/release noise before the delayed note.");
    lookaheadBox.addItem ("Off", 1);
    lookaheadBox.addItem ("150 ms", 2);
    lookaheadBox.addItem ("250 ms", 3);
    lookaheadBox.setColour (juce::ComboBox::textColourId, juce::Colour (0xffe8edf2));
    lookaheadBox.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff202832));
    lookaheadBox.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff65717c));
    lookaheadBox.setColour (juce::ComboBox::arrowColourId, juce::Colour (0xffe8edf2));
    addAndMakeVisible (lookaheadBox);

    configureLabel (fingerNoiseLabel, "Finger Noise");
    configureSlider (fingerNoiseSlider, juce::Colour (0xffd5a36f));
    configureInfoButton (fingerNoiseInfoButton,
                         "0% disables the performance noise layer. 100% adds the strongest modeled finger approach and release noises. "
                         "The noise is most useful with Lookahead enabled, because it can happen before the delayed note onset.");

    configureLabel (vibratoSpeedLabel, "Speed");
    configureSlider (vibratoSpeedSlider, juce::Colour (0xff82cfff));
    configureInfoButton (vibratoSpeedInfoButton,
                         "Finger-vibrato LFO rate in Hz. If Mod Wheel To Speed is enabled, CC1 adds up to about 6 Hz above this baseline.");

    configureLabel (vibratoDepthLabel, "Depth");
    configureSlider (vibratoDepthSlider, juce::Colour (0xffb9d982));
    configureInfoButton (vibratoDepthInfoButton,
                         "Pitch depth in cents. If Mod Wheel To Depth is enabled, CC1 adds up to about 55 cents above this baseline.");

    configureLabel (vibratoDelayLabel, "Delay");
    configureSlider (vibratoDelaySlider, juce::Colour (0xffe6c077));
    configureInfoButton (vibratoDelayInfoButton,
                         "Wait time before vibrato starts on each note. After the wait, vibrato ramps in over the same duration. "
                         "At 0 ms the vibrato is immediate.");

    for (auto* toggle : { &vibratoModWheelSpeedButton, &vibratoModWheelDepthButton })
    {
        toggle->setColour (juce::ToggleButton::textColourId, juce::Colour (0xffd6dee7));
        toggle->setColour (juce::ToggleButton::tickColourId, juce::Colour (0xff82cfff));
        toggle->setColour (juce::ToggleButton::tickDisabledColourId, juce::Colour (0xff65717c));
        addAndMakeVisible (*toggle);
    }

    vibratoModWheelSpeedButton.setButtonText ("Mod Wheel To Speed");
    vibratoModWheelDepthButton.setButtonText ("Mod Wheel To Depth");

    mpeEnabledButton.setButtonText ("MPE Mode");
    mpeEnabledButton.setColour (juce::ToggleButton::textColourId, juce::Colour (0xffd6dee7));
    mpeEnabledButton.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xffb9d982));
    mpeEnabledButton.setColour (juce::ToggleButton::tickDisabledColourId, juce::Colour (0xff65717c));
    addAndMakeVisible (mpeEnabledButton);

    configureLabel (mpePitchBendRangeLabel, "Bend Range");
    configureSlider (mpePitchBendRangeSlider, juce::Colour (0xffb9d982));
    configureInfoButton (mpePitchBendRangeInfoButton,
                         "Expected MPE pitch-bend range in semitones. Default is +/-48 semitones to match Bitwig's MPE default. "
                         "Set this to the same value in the DAW so drawn note bends land at the intended interval.");

    configureLabel (mpePressureAmountLabel, "Pressure Amt");
    configureSlider (mpePressureAmountSlider, juce::Colour (0xffffb36f));
    configureInfoButton (mpePressureAmountInfoButton,
                         "Scales MIDI channel pressure for the matching active voice. Higher values add sustain, level, and upper-mode intensity "
                         "without changing other MPE member channels.");

    configureLabel (mpeTimbreAmountLabel, "CC74 Amount");
    configureSlider (mpeTimbreAmountSlider, juce::Colour (0xff75d7d1));
    configureInfoButton (mpeTimbreAmountInfoButton,
                         "Scales MIDI CC74/timbre for the matching active voice. Higher CC74 values lean the held note brighter and more bridge-like "
                         "without changing other MPE member channels.");

    whammyEnabledButton.setButtonText ("Pitch Wheel Whammy");
    whammyEnabledButton.setColour (juce::ToggleButton::textColourId, juce::Colour (0xffd6dee7));
    whammyEnabledButton.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xfff0a36e));
    whammyEnabledButton.setColour (juce::ToggleButton::tickDisabledColourId, juce::Colour (0xff65717c));
    addAndMakeVisible (whammyEnabledButton);

    configureLabel (whammyUpRangeLabel, "Up Range");
    configureSlider (whammyUpRangeSlider, juce::Colour (0xffffa56f));
    configureInfoButton (whammyUpRangeInfoButton,
                         "Maximum upward pitch-wheel bend in semitones. Default is +6 semitones.");

    configureLabel (whammyDownRangeLabel, "Down Range");
    configureSlider (whammyDownRangeSlider, juce::Colour (0xff6fb1ff));
    configureInfoButton (whammyDownRangeInfoButton,
                         "Maximum downward pitch-wheel bend in semitones. Default is -12 semitones.");

    configureLabel (whammySpreadLabel, "String Spread");
    configureSlider (whammySpreadSlider, juce::Colour (0xffd5a36f));
    configureInfoButton (whammySpreadInfoButton,
                         "0% makes pitch wheel act like a perfect pitch shifter. Higher values make strings respond by slightly different amounts, "
                         "like a tremolo bridge changing string tension mechanically.");

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
    aftertouchBendAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                   GuitarAgAudioProcessor::aftertouchBendParameterId,
                                                                   aftertouchBendSlider);
    lookaheadAttachment = std::make_unique<ComboBoxAttachment> (audioProcessor.getValueTreeState(),
                                                               GuitarAgAudioProcessor::lookaheadParameterId,
                                                               lookaheadBox);
    fingerNoiseAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                               GuitarAgAudioProcessor::fingerNoiseParameterId,
                                                               fingerNoiseSlider);
    vibratoSpeedAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                 GuitarAgAudioProcessor::vibratoSpeedParameterId,
                                                                 vibratoSpeedSlider);
    vibratoDepthAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                 GuitarAgAudioProcessor::vibratoDepthParameterId,
                                                                 vibratoDepthSlider);
    vibratoDelayAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                 GuitarAgAudioProcessor::vibratoDelayParameterId,
                                                                 vibratoDelaySlider);
    vibratoModWheelSpeedAttachment = std::make_unique<ButtonAttachment> (audioProcessor.getValueTreeState(),
                                                                        GuitarAgAudioProcessor::vibratoModWheelSpeedParameterId,
                                                                        vibratoModWheelSpeedButton);
    vibratoModWheelDepthAttachment = std::make_unique<ButtonAttachment> (audioProcessor.getValueTreeState(),
                                                                        GuitarAgAudioProcessor::vibratoModWheelDepthParameterId,
                                                                        vibratoModWheelDepthButton);
    mpeEnabledAttachment = std::make_unique<ButtonAttachment> (audioProcessor.getValueTreeState(),
                                                               GuitarAgAudioProcessor::mpeEnabledParameterId,
                                                               mpeEnabledButton);
    mpePitchBendRangeAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                      GuitarAgAudioProcessor::mpePitchBendRangeParameterId,
                                                                      mpePitchBendRangeSlider);
    mpePressureAmountAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                      GuitarAgAudioProcessor::mpePressureAmountParameterId,
                                                                      mpePressureAmountSlider);
    mpeTimbreAmountAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                    GuitarAgAudioProcessor::mpeTimbreAmountParameterId,
                                                                    mpeTimbreAmountSlider);
    whammyEnabledAttachment = std::make_unique<ButtonAttachment> (audioProcessor.getValueTreeState(),
                                                                  GuitarAgAudioProcessor::whammyEnabledParameterId,
                                                                  whammyEnabledButton);
    whammyUpRangeAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                  GuitarAgAudioProcessor::whammyUpRangeParameterId,
                                                                  whammyUpRangeSlider);
    whammyDownRangeAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                    GuitarAgAudioProcessor::whammyDownRangeParameterId,
                                                                    whammyDownRangeSlider);
    whammySpreadAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                 GuitarAgAudioProcessor::whammySpreadParameterId,
                                                                 whammySpreadSlider);
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

    graphics.fillAll (juce::Colour (0xff12171d));

    graphics.setColour (juce::Colour (0xff1b232d));
    graphics.fillRect (getLocalBounds().removeFromTop (90));
    graphics.setColour (juce::Colour (0xff283340));
    graphics.drawHorizontalLine (89, 24.0f, static_cast<float> (getWidth() - 24));

    auto bounds = getLocalBounds().reduced (24);
    graphics.setColour (juce::Colour (0xffe8edf2));
    graphics.setFont (juce::FontOptions (25.0f, juce::Font::bold));
    graphics.drawFittedText ("Guitar AG", bounds.removeFromTop (40), juce::Justification::centredLeft, 1);

    graphics.setColour (juce::Colour (0xff9aa8b5));
    graphics.setFont (juce::FontOptions (15.0f));
    graphics.drawFittedText ("Modeled clean-DI electric guitar voice", bounds.removeFromTop (28),
                             juce::Justification::centredLeft, 1);

    auto headerInfoBounds = getLocalBounds().reduced (24);
    headerInfoBounds.removeFromLeft (170);
    headerInfoBounds = headerInfoBounds.removeFromTop (64);

    const juce::String buildText = "v" JucePlugin_VersionString " / " GUITAR_AG_MODEL_LABEL " / " GUITAR_AG_GIT_COMMIT;
    graphics.setColour (juce::Colour (0xffc2ccd6));
    graphics.setFont (juce::FontOptions (12.0f));
    graphics.drawFittedText (buildText, headerInfoBounds.removeFromTop (20),
                             juce::Justification::centredRight, 1);

    graphics.setColour (juce::Colour (0xff7f8d99));
    graphics.drawFittedText ("MPE pitch bend, pressure, and CC74 route per member channel.",
                             headerInfoBounds.removeFromTop (18),
                             juce::Justification::centredRight,
                             1);
}

void GuitarAgAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (24);
    const auto titleBounds = bounds.removeFromTop (40);
    projectInfoButton.setBounds (titleBounds.getX() + 116, titleBounds.getY() + 8, 22, 22);
    bounds.removeFromTop (38);

    auto tabBounds = bounds.removeFromTop (32);
    auto distributeTab = [&tabBounds] (juce::Button& button, int remaining)
    {
        button.setBounds (tabBounds.removeFromLeft (tabBounds.getWidth() / remaining).reduced (2, 0));
    };

    distributeTab (setupSectionButton, 7);
    distributeTab (pickupSectionButton, 6);
    distributeTab (performanceSectionButton, 5);
    distributeTab (vibratoSectionButton, 4);
    distributeTab (mpeSectionButton, 3);
    distributeTab (whammySectionButton, 2);
    articulationSectionButton.setBounds (tabBounds.reduced (2, 0));

    bounds.removeFromTop (18);

    if (activePage == 0)
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

    if (activePage == 1)
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

    if (activePage == 2)
    {
        auto pressureBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (pressureBounds, fretPressureLabel, fretPressureInfoButton);
        fretPressureSlider.setBounds (pressureBounds);

        auto aftertouchBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (aftertouchBounds, aftertouchBendLabel, aftertouchBendInfoButton);
        aftertouchBendSlider.setBounds (aftertouchBounds);

        auto lookaheadBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (lookaheadBounds, lookaheadLabel, lookaheadInfoButton);
        lookaheadBox.setBounds (lookaheadBounds.reduced (0, 4));

        auto fingerNoiseBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (fingerNoiseBounds, fingerNoiseLabel, fingerNoiseInfoButton);
        fingerNoiseSlider.setBounds (fingerNoiseBounds);
    }

    if (activePage == 3)
    {
        auto speedBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (speedBounds, vibratoSpeedLabel, vibratoSpeedInfoButton);
        vibratoSpeedSlider.setBounds (speedBounds);

        auto depthBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (depthBounds, vibratoDepthLabel, vibratoDepthInfoButton);
        vibratoDepthSlider.setBounds (depthBounds);

        auto delayBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (delayBounds, vibratoDelayLabel, vibratoDelayInfoButton);
        vibratoDelaySlider.setBounds (delayBounds);

        auto modBounds = bounds.removeFromTop (34);
        modBounds.removeFromLeft (158);
        vibratoModWheelSpeedButton.setBounds (modBounds.removeFromLeft (170));
        vibratoModWheelDepthButton.setBounds (modBounds.removeFromLeft (170));
    }

    if (activePage == 4)
    {
        auto enableBounds = bounds.removeFromTop (34);
        enableBounds.removeFromLeft (158);
        mpeEnabledButton.setBounds (enableBounds.removeFromLeft (190));

        auto rangeBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (rangeBounds, mpePitchBendRangeLabel, mpePitchBendRangeInfoButton);
        mpePitchBendRangeSlider.setBounds (rangeBounds);

        auto pressureAmountBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (pressureAmountBounds, mpePressureAmountLabel, mpePressureAmountInfoButton);
        mpePressureAmountSlider.setBounds (pressureAmountBounds);

        auto timbreAmountBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (timbreAmountBounds, mpeTimbreAmountLabel, mpeTimbreAmountInfoButton);
        mpeTimbreAmountSlider.setBounds (timbreAmountBounds);
    }

    if (activePage == 5)
    {
        auto enableBounds = bounds.removeFromTop (34);
        enableBounds.removeFromLeft (158);
        whammyEnabledButton.setBounds (enableBounds.removeFromLeft (190));

        auto upBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (upBounds, whammyUpRangeLabel, whammyUpRangeInfoButton);
        whammyUpRangeSlider.setBounds (upBounds);

        auto downBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (downBounds, whammyDownRangeLabel, whammyDownRangeInfoButton);
        whammyDownRangeSlider.setBounds (downBounds);

        auto spreadBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (spreadBounds, whammySpreadLabel, whammySpreadInfoButton);
        whammySpreadSlider.setBounds (spreadBounds);
    }

    if (activePage == 6)
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
    button.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff222b36));
    button.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff39485a));
    button.setColour (juce::TextButton::textColourOffId, juce::Colour (0xffb7c4d0));
    button.setColour (juce::TextButton::textColourOnId, juce::Colour (0xffe8edf2));
    button.setClickingTogglesState (false);
    button.setTriggeredOnMouseDown (false);
    addAndMakeVisible (button);
}

void GuitarAgAudioProcessorEditor::configureInfoButton (juce::TextButton& button, const juce::String& infoText)
{
    button.setButtonText ("i");
    button.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff2a3542));
    button.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff3a4a5c));
    button.setColour (juce::TextButton::textColourOffId, juce::Colour (0xffd6dee7));
    button.setColour (juce::TextButton::textColourOnId, juce::Colour (0xffe8edf2));
    button.setTriggeredOnMouseDown (false);
    button.onClick = [this, &button, infoText]
    {
        showInfoPopover (button, infoText);
    };
    addAndMakeVisible (button);
}

void GuitarAgAudioProcessorEditor::showInfoPopover (juce::Component& source, const juce::String& infoText)
{
    auto content = std::make_unique<InfoPopoverContent> (infoText);
    juce::CallOutBox::launchAsynchronously (std::move (content), source.getScreenBounds(), this);
}

void GuitarAgAudioProcessorEditor::layoutLabelAndInfo (juce::Rectangle<int>& row,
                                                       juce::Label& label,
                                                       juce::TextButton& infoButton) noexcept
{
    auto labelArea = row.removeFromLeft (158);
    label.setBounds (labelArea.removeFromLeft (130));
    infoButton.setBounds (labelArea.removeFromLeft (22).reduced (2, 7));
}

void GuitarAgAudioProcessorEditor::setActivePage (int pageIndex)
{
    activePage = juce::jlimit (0, 6, pageIndex);
    updateSectionVisibility();
}

void GuitarAgAudioProcessorEditor::updateSectionVisibility()
{
    setupSectionButton.setToggleState (activePage == 0, juce::dontSendNotification);
    pickupSectionButton.setToggleState (activePage == 1, juce::dontSendNotification);
    performanceSectionButton.setToggleState (activePage == 2, juce::dontSendNotification);
    vibratoSectionButton.setToggleState (activePage == 3, juce::dontSendNotification);
    mpeSectionButton.setToggleState (activePage == 4, juce::dontSendNotification);
    whammySectionButton.setToggleState (activePage == 5, juce::dontSendNotification);
    articulationSectionButton.setToggleState (activePage == 6, juce::dontSendNotification);

    for (auto* component : { static_cast<juce::Component*> (&sustainLabel),
                             static_cast<juce::Component*> (&sustainInfoButton),
                             static_cast<juce::Component*> (&sustainSlider),
                             static_cast<juce::Component*> (&stringAgeLabel),
                             static_cast<juce::Component*> (&stringAgeInfoButton),
                             static_cast<juce::Component*> (&stringAgeSlider),
                             static_cast<juce::Component*> (&bridgeIntonationLabel),
                             static_cast<juce::Component*> (&bridgeIntonationInfoButton),
                             static_cast<juce::Component*> (&bridgeIntonationSlider) })
        component->setVisible (activePage == 0);

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
        component->setVisible (activePage == 1);

    for (auto* component : { static_cast<juce::Component*> (&fretPressureLabel),
                             static_cast<juce::Component*> (&fretPressureInfoButton),
                             static_cast<juce::Component*> (&fretPressureSlider),
                             static_cast<juce::Component*> (&aftertouchBendLabel),
                             static_cast<juce::Component*> (&aftertouchBendInfoButton),
                             static_cast<juce::Component*> (&aftertouchBendSlider),
                             static_cast<juce::Component*> (&lookaheadLabel),
                             static_cast<juce::Component*> (&lookaheadInfoButton),
                             static_cast<juce::Component*> (&lookaheadBox),
                             static_cast<juce::Component*> (&fingerNoiseLabel),
                             static_cast<juce::Component*> (&fingerNoiseInfoButton),
                             static_cast<juce::Component*> (&fingerNoiseSlider) })
        component->setVisible (activePage == 2);

    for (auto* component : { static_cast<juce::Component*> (&vibratoSpeedLabel),
                             static_cast<juce::Component*> (&vibratoSpeedInfoButton),
                             static_cast<juce::Component*> (&vibratoSpeedSlider),
                             static_cast<juce::Component*> (&vibratoDepthLabel),
                             static_cast<juce::Component*> (&vibratoDepthInfoButton),
                             static_cast<juce::Component*> (&vibratoDepthSlider),
                             static_cast<juce::Component*> (&vibratoDelayLabel),
                             static_cast<juce::Component*> (&vibratoDelayInfoButton),
                             static_cast<juce::Component*> (&vibratoDelaySlider),
                             static_cast<juce::Component*> (&vibratoModWheelSpeedButton),
                             static_cast<juce::Component*> (&vibratoModWheelDepthButton) })
        component->setVisible (activePage == 3);

    for (auto* component : { static_cast<juce::Component*> (&mpeEnabledButton),
                             static_cast<juce::Component*> (&mpePitchBendRangeLabel),
                             static_cast<juce::Component*> (&mpePitchBendRangeInfoButton),
                             static_cast<juce::Component*> (&mpePitchBendRangeSlider),
                             static_cast<juce::Component*> (&mpePressureAmountLabel),
                             static_cast<juce::Component*> (&mpePressureAmountInfoButton),
                             static_cast<juce::Component*> (&mpePressureAmountSlider),
                             static_cast<juce::Component*> (&mpeTimbreAmountLabel),
                             static_cast<juce::Component*> (&mpeTimbreAmountInfoButton),
                             static_cast<juce::Component*> (&mpeTimbreAmountSlider) })
        component->setVisible (activePage == 4);

    for (auto* component : { static_cast<juce::Component*> (&whammyEnabledButton),
                             static_cast<juce::Component*> (&whammyUpRangeLabel),
                             static_cast<juce::Component*> (&whammyUpRangeInfoButton),
                             static_cast<juce::Component*> (&whammyUpRangeSlider),
                             static_cast<juce::Component*> (&whammyDownRangeLabel),
                             static_cast<juce::Component*> (&whammyDownRangeInfoButton),
                             static_cast<juce::Component*> (&whammyDownRangeSlider),
                             static_cast<juce::Component*> (&whammySpreadLabel),
                             static_cast<juce::Component*> (&whammySpreadInfoButton),
                             static_cast<juce::Component*> (&whammySpreadSlider) })
        component->setVisible (activePage == 5);

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
        component->setVisible (activePage == 6);

    setSize (560, getPreferredHeight());
    resized();
    repaint();
}

int GuitarAgAudioProcessorEditor::getPreferredHeight() const noexcept
{
    return 360;
}
