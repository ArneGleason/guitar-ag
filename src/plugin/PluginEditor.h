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
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    GuitarAgAudioProcessor& audioProcessor;
    juce::Slider sustainSlider;
    juce::Slider stringAgeSlider;
    juce::Slider pickStiffnessSlider;
    juce::Slider pickTextureSlider;
    juce::Slider palmMuteSlider;
    juce::Slider harmonicTouchSlider;
    juce::Label sustainLabel;
    juce::Label stringAgeLabel;
    juce::Label pickStiffnessLabel;
    juce::Label pickTextureLabel;
    juce::Label palmMuteLabel;
    juce::Label harmonicTouchLabel;
    juce::Label harmonicQuarterLabel;
    juce::Label harmonicThirdLabel;
    juce::Label harmonicHalfLabel;
    std::unique_ptr<SliderAttachment> sustainAttachment;
    std::unique_ptr<SliderAttachment> stringAgeAttachment;
    std::unique_ptr<SliderAttachment> pickStiffnessAttachment;
    std::unique_ptr<SliderAttachment> pickTextureAttachment;
    std::unique_ptr<SliderAttachment> palmMuteAttachment;
    std::unique_ptr<SliderAttachment> harmonicTouchAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAgAudioProcessorEditor)
};
