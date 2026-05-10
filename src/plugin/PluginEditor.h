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
    void configureDisclosureButton (juce::TextButton& button);
    void configureInfoButton (juce::TextButton& button, const juce::String& infoText);
    void showInfoPopover (juce::Component& source, const juce::String& infoText);
    void layoutLabelAndInfo (juce::Rectangle<int>& row, juce::Label& label, juce::TextButton& infoButton) noexcept;
    void layoutLabelInfoDisclosure (juce::Rectangle<int>& row,
                                    juce::Label& label,
                                    juce::TextButton& infoButton,
                                    juce::TextButton& disclosureButton) noexcept;
    void setActivePage (int pageIndex);
    void updateSectionVisibility();
    void updateDisclosureButtons();
    [[nodiscard]] int getPreferredHeight() const noexcept;

    GuitarAgAudioProcessor& audioProcessor;
    juce::TextButton projectInfoButton;
    juce::TextButton setupSectionButton;
    juce::TextButton pickupSectionButton;
    juce::TextButton performanceSectionButton;
    juce::TextButton vibratoSectionButton;
    juce::TextButton mpeSectionButton;
    juce::TextButton whammySectionButton;
    juce::TextButton articulationSectionButton;
    juce::TextButton slideTweaksButton;
    juce::TextButton fingerNoiseTweaksButton;
    juce::TextButton feedbackTweaksButton;
    juce::Slider sustainSlider;
    juce::Slider stringAgeSlider;
    juce::Slider bridgeIntonationSlider;
    juce::Slider fretPressureSlider;
    juce::Slider aftertouchBendSlider;
    juce::Slider neckSlideSlider;
    juce::Slider slideFretStepsSlider;
    juce::Slider slideLiftSlider;
    juce::Slider slideSqueakSlider;
    juce::Slider slideSqueakDownSlider;
    juce::Slider fingerNoiseSlider;
    juce::Slider ampFeedbackSlider;
    juce::Slider vibratoSpeedSlider;
    juce::Slider vibratoDepthSlider;
    juce::Slider vibratoDelaySlider;
    juce::Slider mpePitchBendRangeSlider;
    juce::Slider mpePressureAmountSlider;
    juce::Slider mpeTimbreAmountSlider;
    juce::Slider whammyUpRangeSlider;
    juce::Slider whammyDownRangeSlider;
    juce::Slider whammySpreadSlider;
    juce::Slider pickupPositionSlider;
    juce::Slider legatoArticulationSlider;
    juce::Slider pickStiffnessSlider;
    juce::Slider pickTextureSlider;
    juce::Slider pickBiteSlider;
    juce::ComboBox pickStrokeBox;
    juce::Slider palmMuteSlider;
    juce::Slider harmonicTouchSlider;
    juce::Label sustainLabel;
    juce::Label stringAgeLabel;
    juce::Label bridgeIntonationLabel;
    juce::Label fretPressureLabel;
    juce::Label aftertouchBendLabel;
    juce::Label neckSlideLabel;
    juce::Label slideFretStepsLabel;
    juce::Label slideLiftLabel;
    juce::Label slideSqueakLabel;
    juce::Label slideSqueakDownLabel;
    juce::Label lookaheadLabel;
    juce::Label fingerNoiseLabel;
    juce::Label ampFeedbackLabel;
    juce::Label feedbackReturnLabel;
    juce::Label vibratoSpeedLabel;
    juce::Label vibratoDepthLabel;
    juce::Label vibratoDelayLabel;
    juce::Label mpePitchBendRangeLabel;
    juce::Label mpePressureAmountLabel;
    juce::Label mpeTimbreAmountLabel;
    juce::Label whammyUpRangeLabel;
    juce::Label whammyDownRangeLabel;
    juce::Label whammySpreadLabel;
    juce::Label pickupModelLabel;
    juce::Label pickupPositionLabel;
    juce::Label pickupSixthLabel;
    juce::Label pickupFifthLabel;
    juce::Label pickupQuarterLabel;
    juce::Label pickupThirdLabel;
    juce::Label legatoArticulationLabel;
    juce::Label pickStiffnessLabel;
    juce::Label pickTextureLabel;
    juce::Label pickBiteLabel;
    juce::Label pickStrokeLabel;
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
    juce::TextButton neckSlideInfoButton;
    juce::TextButton slideFretStepsInfoButton;
    juce::TextButton slideLiftInfoButton;
    juce::TextButton slideSqueakInfoButton;
    juce::TextButton slideSqueakDownInfoButton;
    juce::TextButton lookaheadInfoButton;
    juce::TextButton fingerNoiseInfoButton;
    juce::TextButton ampFeedbackInfoButton;
    juce::TextButton feedbackReturnInfoButton;
    juce::TextButton vibratoSpeedInfoButton;
    juce::TextButton vibratoDepthInfoButton;
    juce::TextButton vibratoDelayInfoButton;
    juce::TextButton mpeEnabledInfoButton;
    juce::TextButton mpePitchBendRangeInfoButton;
    juce::TextButton mpePressureAmountInfoButton;
    juce::TextButton mpeTimbreAmountInfoButton;
    juce::TextButton whammyEnabledInfoButton;
    juce::TextButton whammyUpRangeInfoButton;
    juce::TextButton whammyDownRangeInfoButton;
    juce::TextButton whammySpreadInfoButton;
    juce::TextButton pickupModelInfoButton;
    juce::TextButton pickupPositionInfoButton;
    juce::TextButton legatoArticulationInfoButton;
    juce::TextButton pickStiffnessInfoButton;
    juce::TextButton pickTextureInfoButton;
    juce::TextButton pickBiteInfoButton;
    juce::TextButton pickStrokeInfoButton;
    juce::TextButton palmMuteInfoButton;
    juce::TextButton harmonicTouchInfoButton;
    juce::ComboBox lookaheadBox;
    juce::ComboBox pickupModelBox;
    juce::ToggleButton vibratoModWheelSpeedButton;
    juce::ToggleButton vibratoModWheelDepthButton;
    juce::ToggleButton feedbackReturnDistortedButton;
    juce::ToggleButton mpeEnabledButton;
    juce::ToggleButton whammyEnabledButton;
    std::unique_ptr<SliderAttachment> sustainAttachment;
    std::unique_ptr<SliderAttachment> stringAgeAttachment;
    std::unique_ptr<SliderAttachment> bridgeIntonationAttachment;
    std::unique_ptr<SliderAttachment> fretPressureAttachment;
    std::unique_ptr<SliderAttachment> aftertouchBendAttachment;
    std::unique_ptr<SliderAttachment> neckSlideAttachment;
    std::unique_ptr<SliderAttachment> slideFretStepsAttachment;
    std::unique_ptr<SliderAttachment> slideLiftAttachment;
    std::unique_ptr<SliderAttachment> slideSqueakAttachment;
    std::unique_ptr<SliderAttachment> slideSqueakDownAttachment;
    std::unique_ptr<ComboBoxAttachment> lookaheadAttachment;
    std::unique_ptr<SliderAttachment> fingerNoiseAttachment;
    std::unique_ptr<SliderAttachment> ampFeedbackAttachment;
    std::unique_ptr<ButtonAttachment> feedbackReturnDistortedAttachment;
    std::unique_ptr<SliderAttachment> vibratoSpeedAttachment;
    std::unique_ptr<SliderAttachment> vibratoDepthAttachment;
    std::unique_ptr<SliderAttachment> vibratoDelayAttachment;
    std::unique_ptr<ButtonAttachment> vibratoModWheelSpeedAttachment;
    std::unique_ptr<ButtonAttachment> vibratoModWheelDepthAttachment;
    std::unique_ptr<ButtonAttachment> mpeEnabledAttachment;
    std::unique_ptr<SliderAttachment> mpePitchBendRangeAttachment;
    std::unique_ptr<SliderAttachment> mpePressureAmountAttachment;
    std::unique_ptr<SliderAttachment> mpeTimbreAmountAttachment;
    std::unique_ptr<ButtonAttachment> whammyEnabledAttachment;
    std::unique_ptr<SliderAttachment> whammyUpRangeAttachment;
    std::unique_ptr<SliderAttachment> whammyDownRangeAttachment;
    std::unique_ptr<SliderAttachment> whammySpreadAttachment;
    std::unique_ptr<SliderAttachment> pickupPositionAttachment;
    std::unique_ptr<ComboBoxAttachment> pickupModelAttachment;
    std::unique_ptr<SliderAttachment> legatoArticulationAttachment;
    std::unique_ptr<SliderAttachment> pickStiffnessAttachment;
    std::unique_ptr<SliderAttachment> pickTextureAttachment;
    std::unique_ptr<SliderAttachment> pickBiteAttachment;
    std::unique_ptr<ComboBoxAttachment> pickStrokeAttachment;
    std::unique_ptr<SliderAttachment> palmMuteAttachment;
    std::unique_ptr<SliderAttachment> harmonicTouchAttachment;
    int activePage = 0;
    bool slideTweaksOpen = true;
    bool fingerNoiseTweaksOpen = false;
    bool feedbackTweaksOpen = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAgAudioProcessorEditor)
};
