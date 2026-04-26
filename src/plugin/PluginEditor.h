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
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    void configureSectionButton (juce::TextButton& button, const juce::String& title);
    void configureInfoButton (juce::TextButton& button, const juce::String& tooltip);
    void layoutLabelAndInfo (juce::Rectangle<int>& row, juce::Label& label, juce::TextButton& infoButton) noexcept;
    void updateSectionVisibility();
    [[nodiscard]] int getPreferredHeight() const noexcept;
    [[nodiscard]] juce::String getSectionTitle (const juce::String& title, bool expanded) const;

    GuitarAgAudioProcessor& audioProcessor;
    juce::TooltipWindow tooltipWindow { this, 700 };
    juce::TextButton projectInfoButton;
    juce::TextButton setupSectionButton;
    juce::TextButton pickupSectionButton;
    juce::TextButton performanceSectionButton;
    juce::TextButton articulationSectionButton;
    juce::Slider sustainSlider;
    juce::Slider stringAgeSlider;
    juce::Slider bridgeIntonationSlider;
    juce::Slider fretPressureSlider;
    juce::Slider pickupPositionSlider;
    juce::Slider pickStiffnessSlider;
    juce::Slider pickTextureSlider;
    juce::Slider palmMuteSlider;
    juce::Slider harmonicTouchSlider;
    juce::Label sustainLabel;
    juce::Label stringAgeLabel;
    juce::Label bridgeIntonationLabel;
    juce::Label fretPressureLabel;
    juce::Label pickupModelLabel;
    juce::Label pickupPositionLabel;
    juce::Label pickupSixthLabel;
    juce::Label pickupFifthLabel;
    juce::Label pickupQuarterLabel;
    juce::Label pickupThirdLabel;
    juce::Label pickStiffnessLabel;
    juce::Label pickTextureLabel;
    juce::Label palmMuteLabel;
    juce::Label harmonicTouchLabel;
    juce::Label harmonicQuarterLabel;
    juce::Label harmonicThirdLabel;
    juce::Label harmonicHalfLabel;
    juce::TextButton sustainInfoButton;
    juce::TextButton stringAgeInfoButton;
    juce::TextButton bridgeIntonationInfoButton;
    juce::TextButton fretPressureInfoButton;
    juce::TextButton pickupModelInfoButton;
    juce::TextButton pickupPositionInfoButton;
    juce::TextButton pickStiffnessInfoButton;
    juce::TextButton pickTextureInfoButton;
    juce::TextButton palmMuteInfoButton;
    juce::TextButton harmonicTouchInfoButton;
    juce::ComboBox pickupModelBox;
    std::unique_ptr<SliderAttachment> sustainAttachment;
    std::unique_ptr<SliderAttachment> stringAgeAttachment;
    std::unique_ptr<SliderAttachment> bridgeIntonationAttachment;
    std::unique_ptr<SliderAttachment> fretPressureAttachment;
    std::unique_ptr<SliderAttachment> pickupPositionAttachment;
    std::unique_ptr<ComboBoxAttachment> pickupModelAttachment;
    std::unique_ptr<SliderAttachment> pickStiffnessAttachment;
    std::unique_ptr<SliderAttachment> pickTextureAttachment;
    std::unique_ptr<SliderAttachment> palmMuteAttachment;
    std::unique_ptr<SliderAttachment> harmonicTouchAttachment;
    bool setupExpanded = true;
    bool pickupExpanded = true;
    bool performanceExpanded = true;
    bool articulationExpanded = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAgAudioProcessorEditor)
};
