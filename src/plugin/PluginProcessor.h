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
    std::atomic<float>* pickupPositionParameter = nullptr;
    std::atomic<float>* pickupModelParameter = nullptr;
    double currentSampleRate = 44100.0;
    int currentLatencySamples = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAgAudioProcessor)
};
