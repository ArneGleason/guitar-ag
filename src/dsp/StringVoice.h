#pragma once

#include "FretboardMapper.h"

#include <juce_audio_basics/juce_audio_basics.h>

#include <array>
#include <cstddef>
#include <cstdint>

namespace guitar_ag
{

enum class PlayerGesture
{
    Picked,
    HammerOn,
    PullOff,
    RightHandTap
};

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
                float pickTexture,
                float harmonicTouch,
                float stringAge,
                float bridgeIntonation,
                float fretPressure,
                float pickupPosition,
                int pickupModel,
                PlayerGesture gesture);
    void release (int midiNoteNumber, int midiChannel);

    [[nodiscard]] bool isActive() const noexcept { return active; }
    [[nodiscard]] int getNoteNumber() const noexcept { return noteNumber; }
    [[nodiscard]] int getChannel() const noexcept { return channel; }
    [[nodiscard]] int getStringIndex() const noexcept { return stringIndex; }
    [[nodiscard]] float getFeedbackCouplingScore (float loopFrequency) const noexcept;
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
                        float ampFeedback,
                        float feedbackLoopFrequency,
                        float feedbackLoopAmount,
                        float feedbackLoopSignal,
                        int feedbackFocusedString,
                        float feedbackStringFocus,
                        float aftertouchBendSemitones,
                        float mpePressureAmount,
                        float mpeTimbreAmount,
                        float mpePitchBendRange,
                        float neckSlideSemitones,
                        float slideFretSteps,
                        float slideLift,
                        float slideSqueakUp,
                        float slideSqueakDown) noexcept;

private:
    static constexpr auto maxDelaySamples = 8192;
    static constexpr auto resonanceCount = 3;
    static constexpr auto modalCount = 96;
    static constexpr auto pitchControlUpdateInterval = 4;
    static constexpr auto feedbackControlUpdateInterval = 8;

    struct FeedbackRenderContext
    {
        bool feedbackActive = false;
        bool loopActive = false;
        float feedbackRise = 0.0f;
        float releaseScale = 1.0f;
        float feedbackEnergyGate = 0.0f;
        float loopEnergyGate = 0.0f;
        float localFeedbackScale = 1.0f;
        float loopAmount = 0.0f;
        float loopSignal = 0.0f;
        float loopStringScale = 1.0f;
    };

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
    float getHarmonicTouchMask (int harmonic, int harmonicDivision, float harmonicAccuracy) const noexcept;
    float getBridgeIntonationRatio (const FretboardAssignment& assignment, float bridgeIntonation) const noexcept;
    float getFretPressureRatio (const FretboardAssignment& assignment, float fretPressure) const noexcept;
    float getWhammyRatio (float whammySemitones, float whammySpread) const noexcept;
    float updatePitchRatio (float heldSeconds,
                            float vibratoDepthCents,
                            float vibratoSpeedHz,
                            float vibratoDelaySeconds,
                            float whammySemitones,
                            float whammySpread,
                            float aftertouchBendSemitones,
                            float mpePitchBendRange,
                            float neckSlideSemitones,
                            float slideFretSteps,
                            float slideLift) noexcept;
    void updateSlideFretContact (float neckSlideSemitones, float slideFretSteps, float slideLift) noexcept;
    void updatePitchStepCache (float pitchRatio) noexcept;
    void updateFeedbackWeightCache (bool feedbackHasAmount,
                                    bool loopActive,
                                    float feedbackAmount,
                                    float loopAmount,
                                    float feedbackFrequency,
                                    float loopFrequency) noexcept;
    float renderModalBank (float tailBlend,
                           float palmDecay,
                           float expressionPressure,
                           float expressionTimbre,
                           float slideLift,
                           const FeedbackRenderContext& feedback) noexcept;
    float renderPickTransient() noexcept;
    float renderContactLayer (float slideSqueakUp, float slideSqueakDown) noexcept;
    [[nodiscard]] static float getEffectiveSlideFretSteps (float slideFretSteps) noexcept;
    [[nodiscard]] static float getFretSteppedSlideSemitones (float neckSlideSemitones,
                                                             float slideFretSteps,
                                                             float slideLift) noexcept;
    [[nodiscard]] static float getSlideLiftRiseSeconds (float slideLift) noexcept;
    [[nodiscard]] static float fastContactSin (float phase) noexcept;
    [[nodiscard]] static float fastAbsSeventhPower (float value) noexcept;

    std::array<float, maxDelaySamples> delayLine {};
    std::array<float, maxDelaySamples> secondaryDelayLine {};
    std::array<float, modalCount> modalSine {};
    std::array<float, modalCount> modalCosine {};
    std::array<float, modalCount> modalSinStep {};
    std::array<float, modalCount> modalCosStep {};
    std::array<float, modalCount> modalPitchSinStep {};
    std::array<float, modalCount> modalPitchCosStep {};
    std::array<float, modalCount> modalPhaseStep {};
    std::array<float, modalCount> modalAmplitude {};
    std::array<float, modalCount> modalDecay {};
    std::array<float, modalCount> modalTailDampingScale {};
    std::array<float, modalCount> modalModePosition {};
    std::array<float, modalCount> modalHighWeight {};
    std::array<float, modalCount> modalLowWeight {};
    std::array<float, modalCount> modalPalmWeight {};
    std::array<float, modalCount> modalFrequency {};
    std::array<float, modalCount> modalFeedbackWeight {};
    std::array<float, modalCount> modalLoopWeight {};

    double sampleRate = 44100.0;
    int delayLength = 1;
    int writeIndex = 0;
    int pickupOffsetSamples = 1;
    int secondaryPickupOffsetSamples = 1;
    int pickupApertureSamples = 1;
    int samplesSinceStart = 0;
    int activeModalCount = 0;

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
    float pickGrindAmount = 0.0f;
    float pickGrindDecay = 0.0f;
    float pickGrindPhase = 0.0f;
    float pickGrindPhaseStep = 0.0f;
    float pickSlipImpulse = 0.0f;
    float pickSlipDecay = 0.0f;
    int pickSlipCountdown = 0;
    float pickCoinAmount = 0.0f;
    float pickCoinDecay = 0.0f;
    float pickCoinPhase = 0.0f;
    float pickCoinPhaseStep = 0.0f;
    float pickCoinImpulse = 0.0f;
    float pickCoinImpulseDecay = 0.0f;
    int pickCoinCountdown = 0;
    float pickHeavyAmount = 0.0f;
    float pickHeavyDecay = 0.0f;
    float pickHeavyPhase = 0.0f;
    float pickHeavyPhaseStep = 0.0f;
    float pickHeavyState = 0.0f;
    float pickHeavyRaspState = 0.0f;
    float pickHeavyBodyState = 0.0f;
    float pickHeavyChoke = 0.0f;
    int pickContactSamplesRemaining = 0;
    float fingerImpact = 0.0f;
    float fingerImpactDecay = 0.0f;
    float fingerImpactPhase = 0.0f;
    float fingerImpactPhaseStep = 0.0f;
    float pullOffSnap = 0.0f;
    float pullOffSnapDecay = 0.0f;
    float previousGestureNoise = 0.0f;
    float slideFretContact = 0.0f;
    float slideFretContactDecay = 0.0f;
    float slideFretScrape = 0.0f;
    float slideFretScrapeDecay = 0.0f;
    float slideFretContactPhase = 0.0f;
    float slideFretContactPhaseStep = 0.0f;
    float previousSlideFretNoise = 0.0f;
    float slideFretScrapeState = 0.0f;
    float slideFretBodyState = 0.0f;
    float slideFretImpulse = 0.0f;
    float slideFretImpulseDecay = 1.0f;
    float slideFretMotionDrive = 0.0f;
    float slideFretMotionDriveDecay = 1.0f;
    float slideFretSpeed = 0.0f;
    float slideFretDirection = 1.0f;
    int slideFretSlipCountdown = 0;
    float previousNeckSlideSemitones = 0.0f;
    float slideMotionActivity = 0.0f;
    float slideMotionActivityDecay = 0.0f;
    float slideLiftEnvelope = 0.0f;
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
    float vibratoPhase = 0.0f;
    float aftertouchPressure = 0.0f;
    float aftertouchPressureTarget = 0.0f;
    float mpePitchBend = 0.0f;
    float mpePitchBendTarget = 0.0f;
    float cachedPitchRatio = 1.0f;
    float cachedFeedbackDrive = 0.0f;
    float cachedFeedbackHowl = 0.0f;
    float mpePressure = 0.0f;
    float mpePressureTarget = 0.0f;
    float mpeTimbre = 0.0f;
    float mpeTimbreTarget = 0.0f;
    int pitchControlSamplesUntilUpdate = 0;
    int feedbackControlSamplesUntilUpdate = 0;
    int previousSlideFret = 0;

    uint32_t randomState = 0x12345678u;

    bool active = false;
    bool releasing = false;
    bool woundString = false;
    bool useCachedPitchSteps = false;
    bool slideFretStateInitialized = false;
};

} // namespace guitar_ag
