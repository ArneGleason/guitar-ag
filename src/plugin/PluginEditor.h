#pragma once

#include "PluginProcessor.h"

#include <juce_gui_basics/juce_gui_basics.h>

class GuitarAgAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit GuitarAgAudioProcessorEditor (GuitarAgAudioProcessor& processor);
    ~GuitarAgAudioProcessorEditor() override = default;

    void paint (juce::Graphics& graphics) override;
    void resized() override;

private:
    GuitarAgAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAgAudioProcessorEditor)
};
