#pragma once

#include "FretboardMapper.h"

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

    void start (int midiNoteNumber,
                int midiChannel,
                float velocity,
                const FretboardAssignment& assignment,
                float pickStiffness,
                float pickTexture);
    void release (int midiNoteNumber, int midiChannel);

    [[nodiscard]] bool isActive() const noexcept { return active; }
    [[nodiscard]] int getNoteNumber() const noexcept { return noteNumber; }
    [[nodiscard]] int getChannel() const noexcept { return channel; }

    float renderSample (float tailSustain) noexcept;

private:
    static constexpr auto maxDelaySamples = 8192;
    static constexpr auto resonanceCount = 3;
    static constexpr auto modalCount = 96;

    float nextNoiseSample() noexcept;
    void updateDamping() noexcept;
    void startLeftHandRelease() noexcept;
    float pluckShapeAt (float position, float pluckPosition) const noexcept;
    float readDelayLineAtOffset (int offset) const noexcept;
    float readSecondaryDelayLineAtOffset (int offset) const noexcept;
    void configureResonator (int index, float frequency, float radius) noexcept;
    void updateHighFrequencyFeedback() noexcept;
    float applyVelocityCurve (float velocity) const noexcept;
    float processHarmonicDamping (float input, float& state, float highGain, float splitCoefficient) noexcept;
    float processMovingResonance (float input) noexcept;
    float softClip (float value) const noexcept;
    void configureMode (int index, float frequency, float amplitude, float decay, float phase, float tailDampingScale = 1.0f) noexcept;

    std::array<float, maxDelaySamples> delayLine {};
    std::array<float, maxDelaySamples> secondaryDelayLine {};
    std::array<float, modalCount> modalSine {};
    std::array<float, modalCount> modalCosine {};
    std::array<float, modalCount> modalSinStep {};
    std::array<float, modalCount> modalCosStep {};
    std::array<float, modalCount> modalAmplitude {};
    std::array<float, modalCount> modalDecay {};
    std::array<float, modalCount> modalTailDampingScale {};

    double sampleRate = 44100.0;
    int delayLength = 1;
    int writeIndex = 0;
    int pickupOffsetSamples = 1;
    int secondaryPickupOffsetSamples = 1;
    int pickupApertureSamples = 1;
    int samplesSinceStart = 0;

    int noteNumber = -1;
    int channel = 0;
    int stringIndex = 0;
    int fret = 0;
    float woundAmount = 0.0f;

    float damping = 0.9965f;
    float baseDamping = 0.9965f;
    float releaseDamping = 0.985f;
    float lastOutput = 0.0f;
    float lastSecondaryOutput = 0.0f;
    float previousPickupSample = 0.0f;
    float previousSecondaryPickupSample = 0.0f;
    float energy = 0.0f;
    float outputGain = 0.48f;
    float pickTransient = 0.0f;
    float pickTransientDecay = 0.0f;
    float pickContact = 0.0f;
    float pickContactDecay = 0.0f;
    float previousContactNoise = 0.0f;
    float pickContactRing = 0.0f;
    float pickContactRingDecay = 0.0f;
    float pickContactPhase = 0.0f;
    float pickContactPhaseStep = 0.0f;
    int pickContactSamplesRemaining = 0;
    float attackRampSeconds = 0.0025f;
    float modalReleaseDecay = 1.0f;
    std::array<float, resonanceCount> resonanceCoefficient {};
    std::array<float, resonanceCount> resonanceRadiusSquared {};
    std::array<float, resonanceCount> resonanceState1 {};
    std::array<float, resonanceCount> resonanceState2 {};
    float resonanceEnvelope = 0.0f;
    float resonanceDecay = 0.0f;
    int resonanceMoveSamples = 1;
    float dampingTiltState = 0.0f;
    float secondaryDampingTiltState = 0.0f;
    float highFeedbackGain = 1.0f;
    float highFeedbackGainTarget = 1.0f;
    float highFeedbackGainStep = 0.0f;
    int highFeedbackGainSamplesRemaining = 0;
    float secondaryHighFeedbackGain = 1.0f;
    float secondaryHighFeedbackGainTarget = 1.0f;
    float secondaryHighFeedbackGainStep = 0.0f;
    float leftHandDamping = 1.0f;
    float leftHandDampingTarget = 1.0f;
    float leftHandDampingStep = 0.0f;

    uint32_t randomState = 0x12345678u;

    bool active = false;
    bool releasing = false;
    bool woundString = false;
};

} // namespace guitar_ag
