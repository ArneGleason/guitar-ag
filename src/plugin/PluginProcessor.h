#pragma once

#include "../dsp/AudioEngine.h"

#include <juce_audio_processors/juce_audio_processors.h>

class GuitarAgAudioProcessor final : public juce::AudioProcessor
{
public:
    static constexpr auto tailSustainParameterId = "tailSustain";
    static constexpr auto pickStiffnessParameterId = "pickStiffness";
    static constexpr auto pickTextureParameterId = "pickTexture";
    static constexpr auto palmMuteParameterId = "palmMute";
    static constexpr auto harmonicTouchParameterId = "harmonicTouch";
    static constexpr auto stringAgeParameterId = "stringAge";
    static constexpr auto bridgeIntonationParameterId = "bridgeIntonation";
    static constexpr auto fretPressureParameterId = "fretPressure";
    static constexpr auto lookaheadParameterId = "lookahead";
    static constexpr auto fingerNoiseParameterId = "fingerNoise";
    static constexpr auto legatoArticulationParameterId = "legatoArticulation";
    static constexpr auto ampFeedbackParameterId = "ampFeedback";
    static constexpr auto feedbackReturnDistortedParameterId = "feedbackReturnDistorted";
    static constexpr auto vibratoSpeedParameterId = "vibratoSpeed";
    static constexpr auto vibratoDepthParameterId = "vibratoDepth";
    static constexpr auto vibratoDelayParameterId = "vibratoDelay";
    static constexpr auto vibratoModWheelSpeedParameterId = "vibratoModWheelSpeed";
    static constexpr auto vibratoModWheelDepthParameterId = "vibratoModWheelDepth";
    static constexpr auto mpeEnabledParameterId = "mpeEnabled";
    static constexpr auto mpePitchBendRangeParameterId = "mpePitchBendRange";
    static constexpr auto mpePressureAmountParameterId = "mpePressureAmount";
    static constexpr auto mpeTimbreAmountParameterId = "mpeTimbreAmount";
    static constexpr auto whammyEnabledParameterId = "whammyEnabled";
    static constexpr auto whammyUpRangeParameterId = "whammyUpRange";
    static constexpr auto whammyDownRangeParameterId = "whammyDownRange";
    static constexpr auto whammySpreadParameterId = "whammySpread";
    static constexpr auto aftertouchBendParameterId = "aftertouchBend";
    static constexpr auto neckSlideParameterId = "neckSlide";
    static constexpr auto pickupPositionParameterId = "pickupPosition";
    static constexpr auto pickupModelParameterId = "pickupModel";

    GuitarAgAudioProcessor();
    ~GuitarAgAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getValueTreeState() noexcept { return parameters; }

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    [[nodiscard]] int getLookaheadSamples() const noexcept;

    guitar_ag::AudioEngine audioEngine;
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* tailSustainParameter = nullptr;
    std::atomic<float>* pickStiffnessParameter = nullptr;
    std::atomic<float>* pickTextureParameter = nullptr;
    std::atomic<float>* palmMuteParameter = nullptr;
    std::atomic<float>* harmonicTouchParameter = nullptr;
    std::atomic<float>* stringAgeParameter = nullptr;
    std::atomic<float>* bridgeIntonationParameter = nullptr;
    std::atomic<float>* fretPressureParameter = nullptr;
    std::atomic<float>* lookaheadParameter = nullptr;
    std::atomic<float>* fingerNoiseParameter = nullptr;
    std::atomic<float>* legatoArticulationParameter = nullptr;
    std::atomic<float>* ampFeedbackParameter = nullptr;
    std::atomic<float>* feedbackReturnDistortedParameter = nullptr;
    std::atomic<float>* vibratoSpeedParameter = nullptr;
    std::atomic<float>* vibratoDepthParameter = nullptr;
    std::atomic<float>* vibratoDelayParameter = nullptr;
    std::atomic<float>* vibratoModWheelSpeedParameter = nullptr;
    std::atomic<float>* vibratoModWheelDepthParameter = nullptr;
    std::atomic<float>* mpeEnabledParameter = nullptr;
    std::atomic<float>* mpePitchBendRangeParameter = nullptr;
    std::atomic<float>* mpePressureAmountParameter = nullptr;
    std::atomic<float>* mpeTimbreAmountParameter = nullptr;
    std::atomic<float>* whammyEnabledParameter = nullptr;
    std::atomic<float>* whammyUpRangeParameter = nullptr;
    std::atomic<float>* whammyDownRangeParameter = nullptr;
    std::atomic<float>* whammySpreadParameter = nullptr;
    std::atomic<float>* aftertouchBendParameter = nullptr;
    std::atomic<float>* neckSlideParameter = nullptr;
    std::atomic<float>* pickupPositionParameter = nullptr;
    std::atomic<float>* pickupModelParameter = nullptr;
    double currentSampleRate = 44100.0;
    int currentLatencySamples = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAgAudioProcessor)
};
