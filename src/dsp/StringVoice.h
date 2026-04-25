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

    float nextNoiseSample() noexcept;
    void updateDamping() noexcept;
    void startLeftHandRelease() noexcept;
    float pluckShapeAt (float position, float pluckPosition) const noexcept;

    std::array<float, maxDelaySamples> delayLine {};

    double sampleRate = 44100.0;
    int delayLength = 1;
    int writeIndex = 0;
    int samplesSinceStart = 0;

    int noteNumber = -1;
    int channel = 0;

    float damping = 0.9965f;
    float baseDamping = 0.9965f;
    float releaseDamping = 0.985f;
    float lastOutput = 0.0f;
    float energy = 0.0f;
    float outputGain = 0.35f;
    float pickTransient = 0.0f;
    float pickTransientDecay = 0.0f;
    float leftHandDamping = 1.0f;
    float leftHandDampingTarget = 1.0f;
    float leftHandDampingStep = 0.0f;

    uint32_t randomState = 0x12345678u;

    bool active = false;
    bool releasing = false;
};

} // namespace guitar_ag
