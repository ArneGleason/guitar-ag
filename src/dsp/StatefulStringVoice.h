#pragma once

#include "StringVoice.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace guitar_ag
{

class StatefulStringVoice
{
public:
    void prepare (double newSampleRate);
    void reset() noexcept;
    void setPreserveStateOnStart (bool shouldPreserve) noexcept { preserveStateOnStart = shouldPreserve; }

    void start (int midiNoteNumber,
                int midiChannel,
                float velocity,
                const FretboardAssignment& assignment,
                float pickStiffness,
                float pickTexture,
                float pickBite,
                float stringAge,
                float bridgeIntonation,
                float fretPressure,
                float pickupPosition,
                int pickupModel,
                PlayerGesture gesture,
                PickStrokeDirection strokeDirection,
                uint32_t attackSeed) noexcept;
    void release (int midiNoteNumber, int midiChannel) noexcept;

    [[nodiscard]] bool isActive() const noexcept { return active; }
    [[nodiscard]] int getNoteNumber() const noexcept { return noteNumber; }
    [[nodiscard]] int getChannel() const noexcept { return channel; }
    [[nodiscard]] int getStringIndex() const noexcept { return stringIndex; }
    [[nodiscard]] int getFret() const noexcept { return fret; }

    void setAftertouchPressure (int midiNoteNumber, int midiChannel, float pressure) noexcept;
    void setMpePitchBend (int midiChannel, float bend) noexcept;
    void setMpePressure (int midiChannel, float pressure) noexcept;
    void setMpeTimbre (int midiChannel, float timbre) noexcept;

    float renderSample (float tailSustain,
                        float palmMute,
                        float vibratoDepthCents,
                        float vibratoSpeedHz,
                        float vibratoDelaySeconds,
                        float whammySemitones,
                        float whammySpread,
                        float aftertouchBendSemitones,
                        float mpePressureAmount,
                        float mpeTimbreAmount,
                        float mpePitchBendRange,
                        float neckSlideSemitones,
                        float slideLift,
                        float ampFeedback,
                        float feedbackLoopSignal) noexcept;

private:
    static constexpr auto maxDelaySamples = 8192;

    [[nodiscard]] float readFractional (const std::array<float, maxDelaySamples>& buffer,
                                        float delaySamples) const noexcept;
    [[nodiscard]] float readPickup (const std::array<float, maxDelaySamples>& buffer,
                                    float loopDelaySamples,
                                    float position,
                                    float aperture) const noexcept;
    [[nodiscard]] float renderPlectrumContact (float localPrimary, float localSecondary) noexcept;
    [[nodiscard]] float nextRandomBipolar() noexcept;
    [[nodiscard]] float calculatePitchRatio (float vibratoDepthCents,
                                              float vibratoSpeedHz,
                                              float vibratoDelaySeconds,
                                              float whammySemitones,
                                              float whammySpread,
                                              float aftertouchBendSemitones,
                                              float mpePitchBendRange,
                                              float neckSlideSemitones) noexcept;
    void beginPlectrumContact (float velocity,
                               float pickStiffness,
                               float pickTexture,
                               float pickBite,
                               PlayerGesture gesture,
                               PickStrokeDirection strokeDirection,
                               uint32_t attackSeed) noexcept;

    std::array<float, maxDelaySamples> primaryDelay {};
    std::array<float, maxDelaySamples> secondaryDelay {};

    double sampleRate = 44100.0;
    int writeIndex = 0;
    int samplesSinceStart = 0;
    int noteNumber = -1;
    int channel = 0;
    int stringIndex = 0;
    int fret = 0;

    float baseFrequency = 110.0f;
    float currentPrimaryDelay = 400.0f;
    float currentSecondaryDelay = 401.0f;
    float primaryLoopLowState = 0.0f;
    float secondaryLoopLowState = 0.0f;
    float previousPrimaryPickup = 0.0f;
    float previousSecondaryPickup = 0.0f;
    float pickupFilterState1 = 0.0f;
    float pickupFilterState2 = 0.0f;
    float pickupDcInput = 0.0f;
    float pickupDcOutput = 0.0f;
    float pickupPosition = 0.18f;
    float pickupAperture = 0.018f;
    float woundAmount = 0.0f;
    float stringAge = 0.0f;
    float energy = 0.0f;
    float vibratoPhase = 0.0f;
    float aftertouchPressure = 0.0f;
    float aftertouchPressureTarget = 0.0f;
    float mpePitchBend = 0.0f;
    float mpePitchBendTarget = 0.0f;
    float mpePressure = 0.0f;
    float mpePressureTarget = 0.0f;
    float mpeTimbre = 0.0f;
    float mpeTimbreTarget = 0.0f;

    float contactTipPosition = 0.0f;
    float contactTravelPerSample = 0.0f;
    float contactReleaseDeflection = 0.02f;
    float contactStiffness = 0.5f;
    float contactDamping = 0.2f;
    float contactDrive = 0.1f;
    float contactPolarization = 0.3f;
    float previousContactRelative = 0.0f;
    float contactForceState = 0.0f;
    float contactForceCoefficient = 0.2f;
    int contactSamples = 0;
    int contactMaximumSamples = 1;
    float strokeSign = 1.0f;

    uint32_t randomState = 0x12345678u;
    int pickupModel = 0;
    bool active = false;
    bool releasing = false;
    bool contactActive = false;
    bool preserveStateOnStart = true;
};

} // namespace guitar_ag
