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

    sustainAttachment = std::make_unique<SliderAttachment> (audioProcessor.getValueTreeState(),
                                                            GuitarAgAudioProcessor::tailSustainParameterId,
                                                            sustainSlider);

    setSize (460, 220);
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
    bounds.removeFromTop (58);
    graphics.drawFittedText ("MPE routing and pickup modeling are intentionally not implemented yet.", bounds,
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

    auto sustainBounds = bounds.removeFromTop (42);
    sustainLabel.setBounds (sustainBounds.removeFromLeft (82));
    sustainSlider.setBounds (sustainBounds);
}
