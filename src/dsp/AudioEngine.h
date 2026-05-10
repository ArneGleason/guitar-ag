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
    struct PerformanceStats
    {
        uint64_t renderedSamples = 0;
        uint64_t activeVoiceSamples = 0;
        uint64_t activeFingerNoiseSamples = 0;
        int maxActiveVoices = 0;
        int maxActiveFingerNoiseVoices = 0;

        void reset() noexcept { *this = {}; }

        [[nodiscard]] double getAverageActiveVoices() const noexcept
        {
            return renderedSamples > 0
                ? static_cast<double> (activeVoiceSamples) / static_cast<double> (renderedSamples)
                : 0.0;
        }

        [[nodiscard]] double getAverageActiveFingerNoiseVoices() const noexcept
        {
            return renderedSamples > 0
                ? static_cast<double> (activeFingerNoiseSamples) / static_cast<double> (renderedSamples)
                : 0.0;
        }
    };

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
    void setNeckSlideSemitones (float newNeckSlideSemitones) noexcept;
    void setSlideFretSteps (float newSlideFretSteps) noexcept;
    void setSlideLift (float newSlideLift) noexcept;
    void setLegatoArticulation (float newLegatoArticulation) noexcept;
    void setAmpFeedback (float newAmpFeedback) noexcept;
    void setFeedbackReturnDistorted (bool shouldDistort) noexcept;
    void setVibratoSpeed (float newVibratoSpeed) noexcept;
    void setVibratoDepth (float newVibratoDepth) noexcept;
    void setVibratoDelay (float newVibratoDelay) noexcept;
    void setVibratoModWheelSpeedEnabled (bool enabled) noexcept;
    void setVibratoModWheelDepthEnabled (bool enabled) noexcept;
    void setMpeEnabled (bool enabled) noexcept;
    void setMpePitchBendRange (float newMpePitchBendRange) noexcept;
    void setMpePressureAmount (float newMpePressureAmount) noexcept;
    void setMpeTimbreAmount (float newMpeTimbreAmount) noexcept;
    void setWhammyEnabled (bool enabled) noexcept;
    void setWhammyUpSemitones (float newWhammyUpSemitones) noexcept;
    void setWhammyDownSemitones (float newWhammyDownSemitones) noexcept;
    void setWhammySpread (float newWhammySpread) noexcept;
    void setAftertouchBendSemitones (float newAftertouchBendSemitones) noexcept;
    void setPickupPosition (float newPickupPosition) noexcept;
    void setPickupModel (int newPickupModel) noexcept;

    void render (juce::AudioBuffer<float>& audio, const juce::MidiBuffer& midi);
    void setPerformanceStats (PerformanceStats* stats) noexcept;
    [[nodiscard]] int getActiveVoiceCount() const noexcept;
    [[nodiscard]] int getActiveFingerNoiseVoiceCount() const noexcept;

private:
    static constexpr auto maxVoices = 6;
    static constexpr auto maxScheduledMidiEvents = 256;
    static constexpr auto maxFingerNoiseVoices = 12;
    static constexpr auto feedbackResonatorCount = 8;

    void renderRange (juce::AudioBuffer<float>& audio, int startSample, int endSample) noexcept;
    void handleIncomingMidiMessage (const juce::MidiMessage& message);
    void handleMidiMessage (const juce::MidiMessage& message);
    void noteOn (int noteNumber, int channel, float velocity);
    void noteOff (int noteNumber, int channel);
    StringVoice& selectVoiceForAssignment (const FretboardAssignment& assignment) noexcept;
    void applyAftertouch (int noteNumber, int channel, float pressure) noexcept;
    void applyMpePitchBend (int channel, float bend) noexcept;
    void applyMpePressure (int channel, float pressure) noexcept;
    void applyMpeTimbre (int channel, float timbre) noexcept;
    void scheduleMidiMessage (const juce::MidiMessage& message, int64_t sampleTime) noexcept;
    void dispatchScheduledMidiEvents() noexcept;
    void clearScheduledMidiEvents() noexcept;
    void triggerFingerApproach (int noteNumber, int channel, float velocity) noexcept;
    void triggerFingerRelease (int noteNumber, int channel) noexcept;
    struct LegatoSource;
    [[nodiscard]] LegatoSource findLegatoSource (int noteNumber, int channel, float amount) const noexcept;
    void rememberArticulationNote (int noteNumber,
                                   int channel,
                                   const FretboardAssignment& assignment,
                                   PlayerGesture gesture) noexcept;
    void releaseArticulationNote (int noteNumber, int channel) noexcept;
    void releaseLegatoSource (const LegatoSource& source) noexcept;
    [[nodiscard]] static float getDeterministicGestureChance (int noteNumber,
                                                              int channel,
                                                              int sourceNoteNumber,
                                                              int64_t sampleTime) noexcept;
    void rememberFingerAssignment (int noteNumber, int channel, const FretboardAssignment& assignment) noexcept;
    FretboardAssignment findFingerAssignment (int noteNumber, int channel) const noexcept;
    void releaseFingerAssignment (int noteNumber, int channel) noexcept;
    void startFingerNoise (const FretboardAssignment& assignment, float intensity, bool releaseNoise) noexcept;
    float renderFingerNoiseSample() noexcept;
    static float nextFingerNoiseRandom (uint32_t& state) noexcept;
    void configureAmpFeedbackLoop() noexcept;
    void resetAmpFeedbackLoop() noexcept;
    void triggerFeedbackBloomDuck (float velocity, PlayerGesture gesture) noexcept;
    [[nodiscard]] float updateFeedbackBloom (float amount) noexcept;
    void updateAmpFeedbackLoop (float outputSample, float amount) noexcept;
    void updateFeedbackStringFocus (float amount, float loopFrequency, float loopAmount) noexcept;
    void recordPerformanceSample() noexcept;

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

    struct LegatoSource
    {
        int noteNumber = -1;
        int channel = -1;
        FretboardAssignment assignment {};
        PlayerGesture gesture = PlayerGesture::Picked;
        int destinationFret = -1;
        int64_t startSample = 0;
        int64_t releaseSample = 0;
        bool active = false;
        bool valid = false;
    };

    struct ArticulationNote
    {
        int noteNumber = -1;
        int channel = -1;
        FretboardAssignment assignment {};
        PlayerGesture gesture = PlayerGesture::Picked;
        int64_t startSample = 0;
        int64_t releaseSample = 0;
        bool active = false;
        bool valid = false;
    };

    struct FingerNoiseVoice
    {
        int samplesRemaining = 0;
        float amplitude = 0.0f;
        float decay = 1.0f;
        float previousNoise = 0.0f;
        float bodyState = 0.0f;
        float scrapeState = 0.0f;
        float stickImpulse = 0.0f;
        float stickImpulseDecay = 1.0f;
        float squeakAmount = 0.0f;
        float phase = 0.0f;
        float phaseStep = 0.0f;
        float woundAmount = 0.0f;
        int stickCountdown = 0;
        uint32_t randomState = 0x0badf00du;
    };

    std::array<StringVoice, maxVoices> voices;
    std::array<ScheduledMidiEvent, maxScheduledMidiEvents> scheduledMidiEvents {};
    std::array<FingerAssignment, maxVoices> fingerAssignments {};
    std::array<ArticulationNote, maxVoices> articulationNotes {};
    std::array<FingerNoiseVoice, maxFingerNoiseVoices> fingerNoiseVoices {};
    std::array<float, feedbackResonatorCount> feedbackResonatorCoefficient {};
    std::array<float, feedbackResonatorCount> feedbackResonatorRadiusSquared {};
    std::array<float, feedbackResonatorCount> feedbackResonatorState1 {};
    std::array<float, feedbackResonatorCount> feedbackResonatorState2 {};
    std::array<float, feedbackResonatorCount> feedbackResonatorEnvelope {};
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
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> neckSlideSemitones { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> slideFretSteps { 0.65f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> slideLift { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> legatoArticulation { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> ampFeedback { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> vibratoSpeed { 5.5f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> vibratoDepth { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> vibratoDelay { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> modWheel { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pitchWheel { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mpePitchBendRange { 48.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mpePressureAmount { 0.65f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mpeTimbreAmount { 0.65f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> whammyUpSemitones { 6.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> whammyDownSemitones { 12.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> whammySpread { 0.35f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> aftertouchBendSemitones { 2.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pickupPosition { 0.39f };
    double sampleRate = 44100.0;
    PerformanceStats* performanceStats = nullptr;
    std::array<float, 16> mpePitchBendByChannel {};
    int64_t timelineSample = 0;
    int lookaheadSamples = 0;
    int pickupModel = 0;
    int nextVoice = 0;
    int nextFingerNoiseVoice = 0;
    int feedbackDominantBand = 0;
    int feedbackDominantString = -1;
    int feedbackFocusUpdateCountdown = 0;
    float feedbackLoopFrequency = 0.0f;
    float feedbackLoopAmount = 0.0f;
    float feedbackLoopSignal = 0.0f;
    float feedbackLoopDominance = 0.0f;
    float feedbackStringFocus = 0.0f;
    float feedbackStringFocusTarget = 0.0f;
    float feedbackStringDominance = 0.0f;
    float feedbackBloom = 1.0f;
    int64_t lastFeedbackBloomDuckSample = -1000000000;
    bool vibratoModWheelSpeedEnabled = false;
    bool vibratoModWheelDepthEnabled = false;
    bool mpeEnabled = false;
    bool whammyEnabled = true;
    bool feedbackReturnDistorted = true;
    std::array<float, 16> mpePressureByChannel {};
    std::array<float, 16> mpeTimbreByChannel {};
};

} // namespace guitar_ag
