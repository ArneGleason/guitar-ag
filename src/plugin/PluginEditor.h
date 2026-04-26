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
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    void configureSectionButton (juce::TextButton& button, const juce::String& title);
    void configureInfoButton (juce::TextButton& button, const juce::String& infoText);
    void showInfoPopover (juce::Component& source, const juce::String& infoText);
    void layoutLabelAndInfo (juce::Rectangle<int>& row, juce::Label& label, juce::TextButton& infoButton) noexcept;
    void updateSectionVisibility();
    [[nodiscard]] int getPreferredHeight() const noexcept;
    [[nodiscard]] juce::String getSectionTitle (const juce::String& title, bool expanded) const;

    GuitarAgAudioProcessor& audioProcessor;
    juce::TextButton projectInfoButton;
    juce::TextButton setupSectionButton;
    juce::TextButton pickupSectionButton;
    juce::TextButton performanceSectionButton;
    juce::TextButton vibratoSectionButton;
    juce::TextButton whammySectionButton;
    juce::TextButton articulationSectionButton;
    juce::Slider sustainSlider;
    juce::Slider stringAgeSlider;
    juce::Slider bridgeIntonationSlider;
    juce::Slider fretPressureSlider;
    juce::Slider aftertouchBendSlider;
    juce::Slider fingerNoiseSlider;
    juce::Slider vibratoSpeedSlider;
    juce::Slider vibratoDepthSlider;
    juce::Slider vibratoDelaySlider;
    juce::Slider whammyUpRangeSlider;
    juce::Slider whammyDownRangeSlider;
    juce::Slider whammySpreadSlider;
    juce::Slider pickupPositionSlider;
    juce::Slider pickStiffnessSlider;
    juce::Slider pickTextureSlider;
    juce::Slider palmMuteSlider;
    juce::Slider harmonicTouchSlider;
    juce::Label sustainLabel;
    juce::Label stringAgeLabel;
    juce::Label bridgeIntonationLabel;
    juce::Label fretPressureLabel;
    juce::Label aftertouchBendLabel;
    juce::Label lookaheadLabel;
    juce::Label fingerNoiseLabel;
    juce::Label vibratoSpeedLabel;
    juce::Label vibratoDepthLabel;
    juce::Label vibratoDelayLabel;
    juce::Label whammyUpRangeLabel;
    juce::Label whammyDownRangeLabel;
    juce::Label whammySpreadLabel;
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
    juce::TextButton aftertouchBendInfoButton;
    juce::TextButton lookaheadInfoButton;
    juce::TextButton fingerNoiseInfoButton;
    juce::TextButton vibratoSpeedInfoButton;
    juce::TextButton vibratoDepthInfoButton;
    juce::TextButton vibratoDelayInfoButton;
    juce::TextButton whammyUpRangeInfoButton;
    juce::TextButton whammyDownRangeInfoButton;
    juce::TextButton whammySpreadInfoButton;
    juce::TextButton pickupModelInfoButton;
    juce::TextButton pickupPositionInfoButton;
    juce::TextButton pickStiffnessInfoButton;
    juce::TextButton pickTextureInfoButton;
    juce::TextButton palmMuteInfoButton;
    juce::TextButton harmonicTouchInfoButton;
    juce::ComboBox lookaheadBox;
    juce::ComboBox pickupModelBox;
    juce::ToggleButton vibratoModWheelSpeedButton;
    juce::ToggleButton vibratoModWheelDepthButton;
    juce::ToggleButton whammyEnabledButton;
    std::unique_ptr<SliderAttachment> sustainAttachment;
    std::unique_ptr<SliderAttachment> stringAgeAttachment;
    std::unique_ptr<SliderAttachment> bridgeIntonationAttachment;
    std::unique_ptr<SliderAttachment> fretPressureAttachment;
    std::unique_ptr<SliderAttachment> aftertouchBendAttachment;
    std::unique_ptr<ComboBoxAttachment> lookaheadAttachment;
    std::unique_ptr<SliderAttachment> fingerNoiseAttachment;
    std::unique_ptr<SliderAttachment> vibratoSpeedAttachment;
    std::unique_ptr<SliderAttachment> vibratoDepthAttachment;
    std::unique_ptr<SliderAttachment> vibratoDelayAttachment;
    std::unique_ptr<ButtonAttachment> vibratoModWheelSpeedAttachment;
    std::unique_ptr<ButtonAttachment> vibratoModWheelDepthAttachment;
    std::unique_ptr<ButtonAttachment> whammyEnabledAttachment;
    std::unique_ptr<SliderAttachment> whammyUpRangeAttachment;
    std::unique_ptr<SliderAttachment> whammyDownRangeAttachment;
    std::unique_ptr<SliderAttachment> whammySpreadAttachment;
    std::unique_ptr<SliderAttachment> pickupPositionAttachment;
    std::unique_ptr<ComboBoxAttachment> pickupModelAttachment;
    std::unique_ptr<SliderAttachment> pickStiffnessAttachment;
    std::unique_ptr<SliderAttachment> pickTextureAttachment;
    std::unique_ptr<SliderAttachment> palmMuteAttachment;
    std::unique_ptr<SliderAttachment> harmonicTouchAttachment;
    bool setupExpanded = true;
    bool pickupExpanded = true;
    bool performanceExpanded = true;
    bool vibratoExpanded = true;
    bool whammyExpanded = true;
    bool articulationExpanded = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAgAudioProcessorEditor)
};
