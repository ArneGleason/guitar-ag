#include "PluginEditor.h"

#include "BuildInfo.h"

GuitarAgAudioProcessorEditor::GuitarAgAudioProcessorEditor (GuitarAgAudioProcessor& processor)
    : AudioProcessorEditor (&processor),
      audioProcessor (processor)
{
    configureSectionButton (setupSectionButton, "Setup");
    configureSectionButton (pickupSectionButton, "Pickup");
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

    articulationSectionButton.onClick = [this]
    {
        articulationExpanded = ! articulationExpanded;
        updateSectionVisibility();
    };

    sustainLabel.setText ("Sustain", juce::dontSendNotification);
    sustainLabel.setColour (juce::Label::textColourId, juce::Colour (0xffcbd4dc));
    sustainLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (sustainLabel);

    sustainSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    sustainSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 72, 24);
    sustainSlider.setColour (juce::Slider::trackColourId, juce::Colour (0xff6fb1ff));
    sustainSlider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffe8edf2));
    sustainSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe8edf2));
    sustainSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff202832));
    addAndMakeVisible (sustainSlider);

    stringAgeLabel.setText ("String Age", juce::dontSendNotification);
    stringAgeLabel.setColour (juce::Label::textColourId, juce::Colour (0xffcbd4dc));
    stringAgeLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (stringAgeLabel);

    stringAgeSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    stringAgeSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 72, 24);
    stringAgeSlider.setColour (juce::Slider::trackColourId, juce::Colour (0xff9ccf8a));
    stringAgeSlider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffe8edf2));
    stringAgeSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe8edf2));
    stringAgeSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff202832));
    addAndMakeVisible (stringAgeSlider);

    bridgeIntonationLabel.setText ("Bridge Intonation", juce::dontSendNotification);
    bridgeIntonationLabel.setColour (juce::Label::textColourId, juce::Colour (0xffcbd4dc));
    bridgeIntonationLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (bridgeIntonationLabel);

    bridgeIntonationSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    bridgeIntonationSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 72, 24);
    bridgeIntonationSlider.setColour (juce::Slider::trackColourId, juce::Colour (0xffd1b371));
    bridgeIntonationSlider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffe8edf2));
    bridgeIntonationSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe8edf2));
    bridgeIntonationSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff202832));
    addAndMakeVisible (bridgeIntonationSlider);

    pickupModelLabel.setText ("Pickup Model", juce::dontSendNotification);
    pickupModelLabel.setColour (juce::Label::textColourId, juce::Colour (0xffcbd4dc));
    pickupModelLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (pickupModelLabel);

    pickupModelBox.addItem ("Single Coil", 1);
    pickupModelBox.addItem ("Humbucker", 2);
    pickupModelBox.addItem ("Humbucker OOP", 3);
    pickupModelBox.setColour (juce::ComboBox::textColourId, juce::Colour (0xffe8edf2));
    pickupModelBox.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff202832));
    pickupModelBox.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff65717c));
    pickupModelBox.setColour (juce::ComboBox::arrowColourId, juce::Colour (0xffe8edf2));
    addAndMakeVisible (pickupModelBox);

    pickupPositionLabel.setText ("Pickup Position", juce::dontSendNotification);
    pickupPositionLabel.setColour (juce::Label::textColourId, juce::Colour (0xffcbd4dc));
    pickupPositionLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (pickupPositionLabel);

    pickupPositionSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    pickupPositionSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 72, 24);
    pickupPositionSlider.setColour (juce::Slider::trackColourId, juce::Colour (0xff75d7d1));
    pickupPositionSlider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffe8edf2));
    pickupPositionSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe8edf2));
    pickupPositionSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff202832));
    addAndMakeVisible (pickupPositionSlider);

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

    pickStiffnessLabel.setText ("Pick Stiffness", juce::dontSendNotification);
    pickStiffnessLabel.setColour (juce::Label::textColourId, juce::Colour (0xffcbd4dc));
    pickStiffnessLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (pickStiffnessLabel);

    pickStiffnessSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    pickStiffnessSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 72, 24);
    pickStiffnessSlider.setColour (juce::Slider::trackColourId, juce::Colour (0xffffc56f));
    pickStiffnessSlider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffe8edf2));
    pickStiffnessSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe8edf2));
    pickStiffnessSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff202832));
    addAndMakeVisible (pickStiffnessSlider);

    pickTextureLabel.setText ("Pick Texture", juce::dontSendNotification);
    pickTextureLabel.setColour (juce::Label::textColourId, juce::Colour (0xffcbd4dc));
    pickTextureLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (pickTextureLabel);

    pickTextureSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    pickTextureSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 72, 24);
    pickTextureSlider.setColour (juce::Slider::trackColourId, juce::Colour (0xff7bd88f));
    pickTextureSlider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffe8edf2));
    pickTextureSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe8edf2));
    pickTextureSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff202832));
    addAndMakeVisible (pickTextureSlider);

    palmMuteLabel.setText ("Palm Mute", juce::dontSendNotification);
    palmMuteLabel.setColour (juce::Label::textColourId, juce::Colour (0xffcbd4dc));
    palmMuteLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (palmMuteLabel);

    palmMuteSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    palmMuteSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 72, 24);
    palmMuteSlider.setColour (juce::Slider::trackColourId, juce::Colour (0xfff28b82));
    palmMuteSlider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffe8edf2));
    palmMuteSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe8edf2));
    palmMuteSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff202832));
    addAndMakeVisible (palmMuteSlider);

    harmonicTouchLabel.setText ("Harmonic Touch", juce::dontSendNotification);
    harmonicTouchLabel.setColour (juce::Label::textColourId, juce::Colour (0xffcbd4dc));
    harmonicTouchLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (harmonicTouchLabel);

    harmonicTouchSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    harmonicTouchSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 72, 24);
    harmonicTouchSlider.setColour (juce::Slider::trackColourId, juce::Colour (0xffc39cff));
    harmonicTouchSlider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffe8edf2));
    harmonicTouchSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe8edf2));
    harmonicTouchSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff202832));
    addAndMakeVisible (harmonicTouchSlider);

    for (auto* marker : { &harmonicQuarterLabel, &harmonicThirdLabel, &harmonicHalfLabel })
    {
        marker->setColour (juce::Label::textColourId, juce::Colour (0xff9aa8b5));
        marker->setJustificationType (juce::Justification::centred);
        addAndMakeVisible (*marker);
    }

    harmonicQuarterLabel.setText ("1/4", juce::dontSendNotification);
    harmonicThirdLabel.setText ("1/3", juce::dontSendNotification);
    harmonicHalfLabel.setText ("1/2", juce::dontSendNotification);

    sustainAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                            GuitarAgAudioProcessor::tailSustainParameterId,
                                                            sustainSlider);
    stringAgeAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                             GuitarAgAudioProcessor::stringAgeParameterId,
                                                             stringAgeSlider);
    bridgeIntonationAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                                    GuitarAgAudioProcessor::bridgeIntonationParameterId,
                                                                    bridgeIntonationSlider);
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
    bounds.removeFromTop (78);

    setupSectionButton.setBounds (bounds.removeFromTop (26));

    if (setupExpanded)
    {
    auto sustainBounds = bounds.removeFromTop (36);
    sustainLabel.setBounds (sustainBounds.removeFromLeft (120));
    sustainSlider.setBounds (sustainBounds);

    auto ageBounds = bounds.removeFromTop (36);
    stringAgeLabel.setBounds (ageBounds.removeFromLeft (120));
    stringAgeSlider.setBounds (ageBounds);

    auto intonationBounds = bounds.removeFromTop (36);
    bridgeIntonationLabel.setBounds (intonationBounds.removeFromLeft (120));
    bridgeIntonationSlider.setBounds (intonationBounds);
    }

    bounds.removeFromTop (8);
    pickupSectionButton.setBounds (bounds.removeFromTop (26));

    if (pickupExpanded)
    {
    auto pickupModelBounds = bounds.removeFromTop (36);
    pickupModelLabel.setBounds (pickupModelBounds.removeFromLeft (120));
    pickupModelBox.setBounds (pickupModelBounds.reduced (0, 4));

    auto pickupPositionBounds = bounds.removeFromTop (54);
    pickupPositionLabel.setBounds (pickupPositionBounds.removeFromLeft (120));
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
             + juce::roundToInt (juce::jlimit (0.0f, 1.0f, normalized) * static_cast<float> (pickupMarkerTrackBounds.getWidth()))
             - pickupMarkerWidth / 2;
    };

    pickupSixthLabel.setBounds (pickupMarkerX (1.0f / 6.0f), pickupMarkerY, pickupMarkerWidth, 18);
    pickupFifthLabel.setBounds (pickupMarkerX (1.0f / 5.0f), pickupMarkerY, pickupMarkerWidth, 18);
    pickupQuarterLabel.setBounds (pickupMarkerX (1.0f / 4.0f), pickupMarkerY, pickupMarkerWidth, 18);
    pickupThirdLabel.setBounds (pickupMarkerX (1.0f / 3.0f), pickupMarkerY, pickupMarkerWidth, 18);
    }

    bounds.removeFromTop (8);
    articulationSectionButton.setBounds (bounds.removeFromTop (26));

    if (articulationExpanded)
    {
    auto stiffnessBounds = bounds.removeFromTop (36);
    pickStiffnessLabel.setBounds (stiffnessBounds.removeFromLeft (120));
    pickStiffnessSlider.setBounds (stiffnessBounds);

    auto textureBounds = bounds.removeFromTop (36);
    pickTextureLabel.setBounds (textureBounds.removeFromLeft (120));
    pickTextureSlider.setBounds (textureBounds);

    auto palmMuteBounds = bounds.removeFromTop (36);
    palmMuteLabel.setBounds (palmMuteBounds.removeFromLeft (120));
    palmMuteSlider.setBounds (palmMuteBounds);

    auto harmonicTouchBounds = bounds.removeFromTop (54);
    harmonicTouchLabel.setBounds (harmonicTouchBounds.removeFromLeft (120));
    const auto harmonicSliderBounds = harmonicTouchBounds.removeFromTop (34);
    harmonicTouchSlider.setBounds (harmonicSliderBounds);

    auto markerTrackBounds = harmonicSliderBounds;
    markerTrackBounds.removeFromRight (78);
    const auto markerY = harmonicTouchBounds.getY() - 1;
    constexpr auto markerWidth = 32;
    const auto markerX = [&markerTrackBounds] (float normalized)
    {
        return markerTrackBounds.getX() + juce::roundToInt (normalized * static_cast<float> (markerTrackBounds.getWidth())) - markerWidth / 2;
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

void GuitarAgAudioProcessorEditor::updateSectionVisibility()
{
    setupSectionButton.setButtonText (getSectionTitle ("Setup", setupExpanded));
    pickupSectionButton.setButtonText (getSectionTitle ("Pickup", pickupExpanded));
    articulationSectionButton.setButtonText (getSectionTitle ("Articulation", articulationExpanded));

    for (auto* component : { static_cast<juce::Component*> (&sustainLabel),
                             static_cast<juce::Component*> (&sustainSlider),
                             static_cast<juce::Component*> (&stringAgeLabel),
                             static_cast<juce::Component*> (&stringAgeSlider),
                             static_cast<juce::Component*> (&bridgeIntonationLabel),
                             static_cast<juce::Component*> (&bridgeIntonationSlider) })
        component->setVisible (setupExpanded);

    for (auto* component : { static_cast<juce::Component*> (&pickupModelLabel),
                             static_cast<juce::Component*> (&pickupModelBox),
                             static_cast<juce::Component*> (&pickupPositionLabel),
                             static_cast<juce::Component*> (&pickupPositionSlider),
                             static_cast<juce::Component*> (&pickupSixthLabel),
                             static_cast<juce::Component*> (&pickupFifthLabel),
                             static_cast<juce::Component*> (&pickupQuarterLabel),
                             static_cast<juce::Component*> (&pickupThirdLabel) })
        component->setVisible (pickupExpanded);

    for (auto* component : { static_cast<juce::Component*> (&pickStiffnessLabel),
                             static_cast<juce::Component*> (&pickStiffnessSlider),
                             static_cast<juce::Component*> (&pickTextureLabel),
                             static_cast<juce::Component*> (&pickTextureSlider),
                             static_cast<juce::Component*> (&palmMuteLabel),
                             static_cast<juce::Component*> (&palmMuteSlider),
                             static_cast<juce::Component*> (&harmonicTouchLabel),
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
    auto controlsHeight = 78 + 26 + 8 + 26 + 8 + 26;

    if (setupExpanded)
        controlsHeight += 108;

    if (pickupExpanded)
        controlsHeight += 90;

    if (articulationExpanded)
        controlsHeight += 162;

    return controlsHeight + 72;
}

juce::String GuitarAgAudioProcessorEditor::getSectionTitle (const juce::String& title, bool expanded) const
{
    return juce::String (expanded ? "- " : "+ ") + title;
}
