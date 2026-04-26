#include "PluginEditor.h"

#include "BuildInfo.h"

GuitarAgAudioProcessorEditor::GuitarAgAudioProcessorEditor (GuitarAgAudioProcessor& processor)
    : AudioProcessorEditor (&processor),
      audioProcessor (processor)
{
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

    sustainAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                            GuitarAgAudioProcessor::tailSustainParameterId,
                                                            sustainSlider);
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

    setSize (500, 372);
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
    bounds.removeFromTop (204);
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

    auto sustainBounds = bounds.removeFromTop (36);
    sustainLabel.setBounds (sustainBounds.removeFromLeft (120));
    sustainSlider.setBounds (sustainBounds);

    auto stiffnessBounds = bounds.removeFromTop (36);
    pickStiffnessLabel.setBounds (stiffnessBounds.removeFromLeft (120));
    pickStiffnessSlider.setBounds (stiffnessBounds);

    auto textureBounds = bounds.removeFromTop (36);
    pickTextureLabel.setBounds (textureBounds.removeFromLeft (120));
    pickTextureSlider.setBounds (textureBounds);

    auto palmMuteBounds = bounds.removeFromTop (36);
    palmMuteLabel.setBounds (palmMuteBounds.removeFromLeft (120));
    palmMuteSlider.setBounds (palmMuteBounds);

    auto harmonicTouchBounds = bounds.removeFromTop (36);
    harmonicTouchLabel.setBounds (harmonicTouchBounds.removeFromLeft (120));
    harmonicTouchSlider.setBounds (harmonicTouchBounds);
}
