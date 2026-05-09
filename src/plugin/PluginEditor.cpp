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
        message.setScrollbarsShown (true);
        message.setCaretVisible (false);
        message.setPopupMenuEnabled (false);
        message.setText (infoText, false);
        message.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xff202832));
        message.setColour (juce::TextEditor::textColourId, juce::Colour (0xffe8edf2));
        message.setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
        message.setColour (juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
        message.setFont (juce::FontOptions (14.0f));
        addAndMakeVisible (message);

        const auto height = juce::jlimit (126, 312, 70 + (infoText.length() / 54) * 18);
        setSize (430, height);
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
                         "Guitar AG makes a clean DI-style electric guitar tone from a physical model instead of samples.\n\n"
                         "Technical: each note is an independent modeled string voice with pickup readout, MPE expression, "
                         "whammy, feedback, and player-articulation layers. It is designed to feed external amp and cab sims.");

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
                         "Use Sustain to let held notes ring longer.\n\n"
                         "Technical: this relaxes late modal damping after the initial attack, so the string tail decays more slowly "
                         "without changing the note's picked transient.");

    configureLabel (stringAgeLabel, "String Age");
    configureSlider (stringAgeSlider, juce::Colour (0xff9ccf8a));
    configureInfoButton (stringAgeInfoButton,
                         "Use String Age to move from bright fresh strings toward a duller worn set.\n\n"
                         "Technical: higher values reduce upper partial strength and shorten high-frequency decay, while leaving "
                         "the lower string body more intact.");

    configureLabel (bridgeIntonationLabel, "Bridge Intonation");
    configureSlider (bridgeIntonationSlider, juce::Colour (0xffd1b371));
    configureInfoButton (bridgeIntonationInfoButton,
                         "Use Bridge Intonation to add a little imperfect guitar setup behavior.\n\n"
                         "Technical: 0% is ideal saddle compensation. Higher values apply deterministic per-string offsets where open "
                         "strings stay in tune but fretted notes drift more as the assigned fret rises.");

    configureLabel (pickupModelLabel, "Pickup Model");
    configureInfoButton (pickupModelInfoButton,
                         "Choose the pickup character before the guitar reaches the amp.\n\n"
                         "Technical: Single Coil reads one narrow position. Humbucker sums two nearby coils for a thicker smoother tone. "
                         "Singles OOP subtracts two wider-spaced single-coil positions for a nasal neck/middle-style response.");

    pickupModelBox.addItem ("Single Coil", 1);
    pickupModelBox.addItem ("Humbucker", 2);
    pickupModelBox.addItem ("Singles OOP", 3);
    pickupModelBox.setColour (juce::ComboBox::textColourId, juce::Colour (0xffe8edf2));
    pickupModelBox.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff202832));
    pickupModelBox.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff65717c));
    pickupModelBox.setColour (juce::ComboBox::arrowColourId, juce::Colour (0xffe8edf2));
    addAndMakeVisible (pickupModelBox);

    configureLabel (pickupPositionLabel, "Pickup Position");
    configureSlider (pickupPositionSlider, juce::Colour (0xff75d7d1));
    configureInfoButton (pickupPositionInfoButton,
                         "Move the virtual pickup from bridge brightness toward neck fullness.\n\n"
                         "Technical: the model reads string motion at fractional positions along the speaking length. Moving the readout "
                         "shifts harmonic peaks and nulls; the markers show useful 1/6, 1/5, 1/4, and 1/3 landmarks.");

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
                         "Use Fret Pressure to make fretted notes go a little sharp from over-gripping.\n\n"
                         "Technical: open strings are unaffected. Lower strings and higher frets receive more shift, modeling the extra stretch "
                         "from pressing between frets. The top of the range is intentionally obvious.");

    configureLabel (aftertouchBendLabel, "Aftertouch Bend");
    configureSlider (aftertouchBendSlider, juce::Colour (0xffffb36f));
    configureInfoButton (aftertouchBendInfoButton,
                         "Use key aftertouch to bend just the note you press into.\n\n"
                         "Technical: poly/key aftertouch targets the matching note and channel, then bends by this many semitones at full pressure. "
                         "Default is +2, 0 disables it, and negative values allow downward pressure bends.");

    configureLabel (neckSlideLabel, "Neck Slide");
    configureSlider (neckSlideSlider, juce::Colour (0xff9ccf8a));
    configureInfoButton (neckSlideInfoButton,
                         "Move all currently ringing strings up or down the neck together.\n\n"
                         "Technical: this is a global octave-range semitone offset intended for slide gestures and chord-shape moves. It layers "
                         "with MPE pitch bend, whammy, vibrato, and aftertouch bend, with the final pitch ratio clamped before modal phase steps "
                         "are calculated.");

    configureLabel (slideFretStepsLabel, "Fret Steps");
    configureSlider (slideFretStepsSlider, juce::Colour (0xffd6cf6f));
    configureInfoButton (slideFretStepsInfoButton,
                         "Make Neck Slide move like a fretted gliss instead of a smooth pitch bend.\n\n"
                         "Technical: higher values pull the slide lane toward semitone plateaus and add small fret-crossing contact ticks while "
                         "the slide is moving. 0% leaves the slide lane smooth.");

    configureLabel (lookaheadLabel, "Lookahead");
    configureInfoButton (lookaheadInfoButton,
                         "Use Lookahead when rendered playback needs finger noises before the note.\n\n"
                         "Technical: Off keeps live response. 150 ms and 250 ms delay note events internally and report plugin latency, creating "
                         "time for approach/release noises before the delayed musical note.");
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
                         "Add small fretting-hand approach and release noises around notes.\n\n"
                         "Technical: this layer uses short synthetic scrape/body bursts tied to string and fret assignment. It is most useful with "
                         "Lookahead enabled, because the approach noise can occur before the delayed note onset.");

    configureLabel (ampFeedbackLabel, "Amp Feedback");
    configureSlider (ampFeedbackSlider, juce::Colour (0xffe87070));
    configureInfoButton (ampFeedbackInfoButton,
                         "Add speaker-to-guitar feedback sustain and controlled howl.\n\n"
                         "Technical: low values mostly reinforce string sustain. Higher values feed the output through competing resonant bands, "
                         "then focus the return into the string most coupled to the winning band so one note can take over. New note attacks "
                         "temporarily duck the loop, then let feedback bloom into the sustain.");

    configureLabel (feedbackReturnLabel, "Return Clip");
    feedbackReturnDistortedButton.setButtonText ("Distorted Return");
    feedbackReturnDistortedButton.setColour (juce::ToggleButton::textColourId, juce::Colour (0xffd6dee7));
    feedbackReturnDistortedButton.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xffe87070));
    feedbackReturnDistortedButton.setColour (juce::ToggleButton::tickDisabledColourId, juce::Colour (0xff65717c));
    addAndMakeVisible (feedbackReturnDistortedButton);
    configureInfoButton (feedbackReturnInfoButton,
                         "Choose whether the feedback return comes back clean or clipped.\n\n"
                         "Technical: On is the default because the clipped return gives the resonator bank a more amp-like source and reduces "
                         "early harmonic chirp. Off uses a cleaner DI-like return that listens to the shaped output.");

    configureLabel (vibratoSpeedLabel, "Speed");
    configureSlider (vibratoSpeedSlider, juce::Colour (0xff82cfff));
    configureInfoButton (vibratoSpeedInfoButton,
                         "Set how fast the finger vibrato wobbles.\n\n"
                         "Technical: this is the per-voice vibrato LFO rate in Hz. If Mod Wheel To Speed is enabled, MIDI CC1 adds up to "
                         "about 6 Hz above this baseline.");

    configureLabel (vibratoDepthLabel, "Depth");
    configureSlider (vibratoDepthSlider, juce::Colour (0xffb9d982));
    configureInfoButton (vibratoDepthInfoButton,
                         "Set how wide the finger vibrato bends the pitch.\n\n"
                         "Technical: depth is measured in cents, where 100 cents is one semitone. If Mod Wheel To Depth is enabled, MIDI CC1 "
                         "adds up to about 55 cents above this baseline.");

    configureLabel (vibratoDelayLabel, "Delay");
    configureSlider (vibratoDelaySlider, juce::Colour (0xffe6c077));
    configureInfoButton (vibratoDelayInfoButton,
                         "Delay the vibrato so notes can start clean before wobbling.\n\n"
                         "Technical: each voice waits this long before vibrato begins, then ramps in over the same duration. At 0 ms, vibrato "
                         "starts immediately.");

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
    configureInfoButton (mpeEnabledInfoButton,
                         "Turn on MPE when the DAW is sending per-note expression.\n\n"
                         "Technical: member-channel pitch bend, pressure, and CC74 route only to voices on that MIDI channel. In lower-zone MPE, "
                         "channel 1 pitch wheel is treated as the master/global whammy source.");

    configureLabel (mpePitchBendRangeLabel, "Bend Range");
    configureSlider (mpePitchBendRangeSlider, juce::Colour (0xffb9d982));
    configureInfoButton (mpePitchBendRangeInfoButton,
                         "Match this to the DAW's MPE pitch-bend range.\n\n"
                         "Technical: this is the semitone range used for member-channel MPE pitch wheel. Default is +/-48 semitones for Bitwig. "
                         "If DAW and plugin ranges disagree, drawn note bends land at the wrong interval.");

    configureLabel (mpePressureAmountLabel, "Pressure Amt");
    configureSlider (mpePressureAmountSlider, juce::Colour (0xffffb36f));
    configureInfoButton (mpePressureAmountInfoButton,
                         "Set how strongly MPE pressure changes a held note.\n\n"
                         "Technical: channel pressure is stored per MIDI channel and applied only to matching active voices. Higher values add "
                         "sustain, level, and upper-mode intensity without changing other MPE member channels.");

    configureLabel (mpeTimbreAmountLabel, "CC74 Amount");
    configureSlider (mpeTimbreAmountSlider, juce::Colour (0xff75d7d1));
    configureInfoButton (mpeTimbreAmountInfoButton,
                         "Set how strongly MPE timbre brightens a held note.\n\n"
                         "Technical: MIDI CC74 is the common MPE timbre lane. Higher values lean the matching voice brighter and more bridge-like "
                         "without changing other MPE member channels.");

    whammyEnabledButton.setButtonText ("Pitch Wheel Whammy");
    whammyEnabledButton.setColour (juce::ToggleButton::textColourId, juce::Colour (0xffd6dee7));
    whammyEnabledButton.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xfff0a36e));
    whammyEnabledButton.setColour (juce::ToggleButton::tickDisabledColourId, juce::Colour (0xff65717c));
    addAndMakeVisible (whammyEnabledButton);
    configureInfoButton (whammyEnabledInfoButton,
                         "Let the MIDI pitch wheel act like a guitar tremolo arm.\n\n"
                         "Technical: in normal MIDI mode, pitch wheel bends all active strings. In MPE mode, lower-zone channel 1 pitch wheel "
                         "feeds global whammy while member-channel pitch wheels remain independent note bends.");

    configureLabel (whammyUpRangeLabel, "Up Range");
    configureSlider (whammyUpRangeSlider, juce::Colour (0xffffa56f));
    configureInfoButton (whammyUpRangeInfoButton,
                         "Set how far the whammy can bend upward.\n\n"
                         "Technical: this is the maximum positive pitch-wheel travel in semitones. Default is +6 semitones, before per-string "
                         "response variation from Whammy String Spread.");

    configureLabel (whammyDownRangeLabel, "Down Range");
    configureSlider (whammyDownRangeSlider, juce::Colour (0xff6fb1ff));
    configureInfoButton (whammyDownRangeInfoButton,
                         "Set how far the whammy can dive downward.\n\n"
                         "Technical: this is the maximum negative pitch-wheel travel in semitones. Default is -12 semitones, before per-string "
                         "response variation from Whammy String Spread.");

    configureLabel (whammySpreadLabel, "String Spread");
    configureSlider (whammySpreadSlider, juce::Colour (0xffd5a36f));
    configureInfoButton (whammySpreadInfoButton,
                         "Make whammy bends less perfectly identical across strings.\n\n"
                         "Technical: 0% behaves like a clean global pitch shifter. Higher values apply small string-dependent response differences, "
                         "approximating a tremolo bridge changing each string's tension by a slightly different interval.");

    configureLabel (legatoArticulationLabel, "Legato Artic");
    configureSlider (legatoArticulationSlider, juce::Colour (0xffd7a0ff));
    configureInfoButton (legatoArticulationInfoButton,
                         "Turn plain MIDI note changes into guitar legato gestures.\n\n"
                         "Technical: 0% keeps notes picked. Above 20%, eligible same-string phrases can become pull-offs, then hammer-ons. "
                         "Above 70%, fast higher-fret leaps can use right-hand tap excitation.");

    configureLabel (pickStiffnessLabel, "Pick Stiffness");
    configureSlider (pickStiffnessSlider, juce::Colour (0xffffc56f));
    configureInfoButton (pickStiffnessInfoButton,
                         "Shape the pick attack from soft release to harder snap.\n\n"
                         "Technical: higher values create a faster edge and stronger upper-mode excitation. Lower values soften the attack "
                         "like a flexible pick or gentler release.");

    configureLabel (pickTextureLabel, "Pick Texture");
    configureSlider (pickTextureSlider, juce::Colour (0xff7bd88f));
    configureInfoButton (pickTextureInfoButton,
                         "Add more surface character to the pick contact.\n\n"
                         "Technical: 0% is close to a smooth frictionless release. Around 50% is a normal plastic-pick snap. The top range adds "
                         "rougher, coin-like contact texture to the excitation.");

    configureLabel (palmMuteLabel, "Palm Mute");
    configureSlider (palmMuteSlider, juce::Colour (0xfff28b82));
    configureInfoButton (palmMuteInfoButton,
                         "Dampen the strings near the bridge for palm-muted notes.\n\n"
                         "Technical: higher values shorten modal decay and emphasize the pick/contact portion of the sound. The lower half of the "
                         "range is curved for finer light-mute control.");

    configureLabel (harmonicTouchLabel, "Harmonic Touch");
    configureSlider (harmonicTouchSlider, juce::Colour (0xffc39cff));
    configureInfoButton (harmonicTouchInfoButton,
                         "Use Harmonic Touch to favor natural-harmonic style notes.\n\n"
                         "Technical: 0-25% is off. 25-50% targets the 1/4 touch point, 50-75% targets 1/3, and 75-100% targets 1/2. Higher within "
                         "each band means a cleaner touch point and stronger harmonic masking.");

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
    neckSlideAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                              GuitarAgAudioProcessor::neckSlideParameterId,
                                                              neckSlideSlider);
    slideFretStepsAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                   GuitarAgAudioProcessor::slideFretStepsParameterId,
                                                                   slideFretStepsSlider);
    lookaheadAttachment = std::make_unique<ComboBoxAttachment> (audioProcessor.getValueTreeState(),
                                                               GuitarAgAudioProcessor::lookaheadParameterId,
                                                               lookaheadBox);
    fingerNoiseAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                               GuitarAgAudioProcessor::fingerNoiseParameterId,
                                                               fingerNoiseSlider);
    ampFeedbackAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                               GuitarAgAudioProcessor::ampFeedbackParameterId,
                                                               ampFeedbackSlider);
    feedbackReturnDistortedAttachment = std::make_unique<ButtonAttachment> (
        audioProcessor.getValueTreeState(),
        GuitarAgAudioProcessor::feedbackReturnDistortedParameterId,
        feedbackReturnDistortedButton);
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
    legatoArticulationAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                       GuitarAgAudioProcessor::legatoArticulationParameterId,
                                                                       legatoArticulationSlider);
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
    graphics.drawFittedText ("MPE member bends stay per-note; channel 1 wheel can drive whammy.",
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

        auto neckSlideBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (neckSlideBounds, neckSlideLabel, neckSlideInfoButton);
        neckSlideSlider.setBounds (neckSlideBounds);

        auto slideFretStepsBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (slideFretStepsBounds, slideFretStepsLabel, slideFretStepsInfoButton);
        slideFretStepsSlider.setBounds (slideFretStepsBounds);

        auto lookaheadBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (lookaheadBounds, lookaheadLabel, lookaheadInfoButton);
        lookaheadBox.setBounds (lookaheadBounds.reduced (0, 4));

        auto fingerNoiseBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (fingerNoiseBounds, fingerNoiseLabel, fingerNoiseInfoButton);
        fingerNoiseSlider.setBounds (fingerNoiseBounds);

        auto feedbackBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (feedbackBounds, ampFeedbackLabel, ampFeedbackInfoButton);
        ampFeedbackSlider.setBounds (feedbackBounds);

        auto feedbackReturnBounds = bounds.removeFromTop (34);
        layoutLabelAndInfo (feedbackReturnBounds, feedbackReturnLabel, feedbackReturnInfoButton);
        feedbackReturnDistortedButton.setBounds (feedbackReturnBounds.removeFromLeft (190));
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
        mpeEnabledInfoButton.setBounds (enableBounds.removeFromLeft (22).reduced (2, 6));

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
        whammyEnabledInfoButton.setBounds (enableBounds.removeFromLeft (22).reduced (2, 6));

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
        auto legatoBounds = bounds.removeFromTop (36);
        layoutLabelAndInfo (legatoBounds, legatoArticulationLabel, legatoArticulationInfoButton);
        legatoArticulationSlider.setBounds (legatoBounds);

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
                             static_cast<juce::Component*> (&neckSlideLabel),
                             static_cast<juce::Component*> (&neckSlideInfoButton),
                             static_cast<juce::Component*> (&neckSlideSlider),
                             static_cast<juce::Component*> (&slideFretStepsLabel),
                             static_cast<juce::Component*> (&slideFretStepsInfoButton),
                             static_cast<juce::Component*> (&slideFretStepsSlider),
                             static_cast<juce::Component*> (&lookaheadLabel),
                             static_cast<juce::Component*> (&lookaheadInfoButton),
                             static_cast<juce::Component*> (&lookaheadBox),
                             static_cast<juce::Component*> (&fingerNoiseLabel),
                             static_cast<juce::Component*> (&fingerNoiseInfoButton),
                             static_cast<juce::Component*> (&fingerNoiseSlider),
                             static_cast<juce::Component*> (&ampFeedbackLabel),
                             static_cast<juce::Component*> (&ampFeedbackInfoButton),
                             static_cast<juce::Component*> (&ampFeedbackSlider),
                             static_cast<juce::Component*> (&feedbackReturnLabel),
                             static_cast<juce::Component*> (&feedbackReturnInfoButton),
                             static_cast<juce::Component*> (&feedbackReturnDistortedButton) })
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
                             static_cast<juce::Component*> (&mpeEnabledInfoButton),
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
                             static_cast<juce::Component*> (&whammyEnabledInfoButton),
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

    for (auto* component : { static_cast<juce::Component*> (&legatoArticulationLabel),
                             static_cast<juce::Component*> (&legatoArticulationInfoButton),
                             static_cast<juce::Component*> (&legatoArticulationSlider),
                             static_cast<juce::Component*> (&pickStiffnessLabel),
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
    return 472;
}
