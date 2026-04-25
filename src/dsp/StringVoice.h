#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include <array>
#include <cstddef>
#include <cstdint>

namespace guitar_ag
{

class StringVoice
{
public:
    void prepare (double newSampleRate);
    void reset();

    void start (int midiNoteNumber, int midiChannel, float velocity);
    void release (int midiNoteNumber, int midiChannel);

    [[nodiscard]] bool isActive() const noexcept { return active; }

    float renderSample() noexcept;

private:
    static constexpr auto maxDelaySamples = 8192;
    static constexpr auto resonanceCount = 3;

    float nextNoiseSample() noexcept;
    void updateDamping() noexcept;
    void startLeftHandRelease() noexcept;
    float pluckShapeAt (float position, float pluckPosition) const noexcept;
    float readDelayLineAtOffset (int offset) const noexcept;
    void configureResonator (int index, float frequency, float radius) noexcept;
    bool isWoundOpenString (int midiNoteNumber) const noexcept;
    float processHarmonicDamping (float input) noexcept;
    float processMovingResonance (float input) noexcept;
    float processWoundInteraction (float inputSlope, float contactOutput) noexcept;
    float softClip (float value) const noexcept;

    std::array<float, maxDelaySamples> delayLine {};

    double sampleRate = 44100.0;
    int delayLength = 1;
    int writeIndex = 0;
    int pickupOffsetSamples = 1;
    int samplesSinceStart = 0;

    int noteNumber = -1;
    int channel = 0;

    float damping = 0.9965f;
    float baseDamping = 0.9965f;
    float releaseDamping = 0.985f;
    float lastOutput = 0.0f;
    float previousPickupSample = 0.0f;
    float energy = 0.0f;
    float outputGain = 0.42f;
    float pickTransient = 0.0f;
    float pickTransientDecay = 0.0f;
    float pickContact = 0.0f;
    float pickContactDecay = 0.0f;
    float previousContactNoise = 0.0f;
    int pickContactSamplesRemaining = 0;
    std::array<float, resonanceCount> resonanceCoefficient {};
    std::array<float, resonanceCount> resonanceRadiusSquared {};
    std::array<float, resonanceCount> resonanceState1 {};
    std::array<float, resonanceCount> resonanceState2 {};
    float resonanceEnvelope = 0.0f;
    float resonanceDecay = 0.0f;
    int resonanceMoveSamples = 1;
    float dampingTiltState = 0.0f;
    float highFeedbackGain = 1.0f;
    float highFeedbackGainTarget = 1.0f;
    float highFeedbackGainStep = 0.0f;
    int highFeedbackGainSamplesRemaining = 0;
    float woundInteractionEnvelope = 0.0f;
    float woundInteractionDecay = 0.0f;
    float woundPreviousNoise = 0.0f;
    float woundTextureState = 0.0f;
    int woundInteractionSamplesRemaining = 0;
    float leftHandDamping = 1.0f;
    float leftHandDampingTarget = 1.0f;
    float leftHandDampingStep = 0.0f;

    uint32_t randomState = 0x12345678u;

    bool active = false;
    bool releasing = false;
    bool woundString = false;
};

} // namespace guitar_ag
