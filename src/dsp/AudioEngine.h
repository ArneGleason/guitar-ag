#pragma once

#include "ElectricGuitarTone.h"
#include "FretboardMapper.h"
#include "StringVoice.h"

#include <array>
#include <cstdint>

namespace guitar_ag
{

class AudioEngine
{
public:
    void prepare (double sampleRate, int maximumBlockSize, int outputChannels);
    void reset();
    void setTailSustain (float newTailSustain) noexcept;
    void setPickStiffness (float newPickStiffness) noexcept;
    void setPickTexture (float newPickTexture) noexcept;
    void setPalmMute (float newPalmMute) noexcept;
    void setHarmonicTouch (float newHarmonicTouch) noexcept;
    void setStringAge (float newStringAge) noexcept;
    void setBridgeIntonation (float newBridgeIntonation) noexcept;
    void setFretPressure (float newFretPressure) noexcept;
    void setLookaheadSamples (int newLookaheadSamples) noexcept;
    void setFingerNoise (float newFingerNoise) noexcept;
    void setPickupPosition (float newPickupPosition) noexcept;
    void setPickupModel (int newPickupModel) noexcept;

    void render (juce::AudioBuffer<float>& audio, const juce::MidiBuffer& midi);

private:
    static constexpr auto maxVoices = 8;
    static constexpr auto maxScheduledMidiEvents = 256;
    static constexpr auto maxFingerNoiseVoices = 12;

    void renderRange (juce::AudioBuffer<float>& audio, int startSample, int endSample) noexcept;
    void handleIncomingMidiMessage (const juce::MidiMessage& message);
    void handleMidiMessage (const juce::MidiMessage& message);
    void noteOn (int noteNumber, int channel, float velocity);
    void noteOff (int noteNumber, int channel);
    void scheduleMidiMessage (const juce::MidiMessage& message, int64_t sampleTime) noexcept;
    void dispatchScheduledMidiEvents() noexcept;
    void clearScheduledMidiEvents() noexcept;
    void triggerFingerApproach (int noteNumber, int channel, float velocity) noexcept;
    void triggerFingerRelease (int noteNumber, int channel) noexcept;
    void rememberFingerAssignment (int noteNumber, int channel, const FretboardAssignment& assignment) noexcept;
    FretboardAssignment findFingerAssignment (int noteNumber, int channel) const noexcept;
    void releaseFingerAssignment (int noteNumber, int channel) noexcept;
    void startFingerNoise (const FretboardAssignment& assignment, float intensity, bool releaseNoise) noexcept;
    float renderFingerNoiseSample() noexcept;
    static float nextFingerNoiseRandom (uint32_t& state) noexcept;

    struct ScheduledMidiEvent
    {
        int64_t sampleTime = 0;
        juce::MidiMessage message;
        bool active = false;
    };

    struct FingerAssignment
    {
        int noteNumber = -1;
        int channel = -1;
        FretboardAssignment assignment {};
        bool active = false;
    };

    struct FingerNoiseVoice
    {
        int samplesRemaining = 0;
        float amplitude = 0.0f;
        float decay = 1.0f;
        float previousNoise = 0.0f;
        float bodyState = 0.0f;
        float phase = 0.0f;
        float phaseStep = 0.0f;
        float woundAmount = 0.0f;
        uint32_t randomState = 0x0badf00du;
    };

    std::array<StringVoice, maxVoices> voices;
    std::array<ScheduledMidiEvent, maxScheduledMidiEvents> scheduledMidiEvents {};
    std::array<FingerAssignment, maxVoices> fingerAssignments {};
    std::array<FingerNoiseVoice, maxFingerNoiseVoices> fingerNoiseVoices {};
    FretboardMapper fretboard;
    FretboardMapper fingerNoiseFretboard;
    ElectricGuitarTone tone;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> tailSustain { 1.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pickStiffness { 0.5f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pickTexture { 0.5f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> palmMute { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> harmonicTouch { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> stringAge { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> bridgeIntonation { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> fretPressure { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> fingerNoise { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pickupPosition { 0.39f };
    double sampleRate = 44100.0;
    int64_t timelineSample = 0;
    int lookaheadSamples = 0;
    int pickupModel = 0;
    int nextVoice = 0;
    int nextFingerNoiseVoice = 0;
};

} // namespace guitar_ag
