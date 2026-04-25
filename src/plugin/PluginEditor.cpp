#include "PluginEditor.h"

GuitarAgAudioProcessorEditor::GuitarAgAudioProcessorEditor (GuitarAgAudioProcessor& processor)
    : AudioProcessorEditor (&processor),
      audioProcessor (processor)
{
    setSize (420, 180);
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
    graphics.drawFittedText ("MVP plugin shell: MIDI-triggered test tone", bounds.removeFromTop (28),
                             juce::Justification::centredLeft, 1);
    graphics.drawFittedText ("String model and MPE routing are intentionally not implemented yet.", bounds,
                             juce::Justification::centredLeft, 2);
}

void GuitarAgAudioProcessorEditor::resized()
{
}
