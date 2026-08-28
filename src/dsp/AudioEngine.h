#pragma once

#include "ElectricGuitarTone.h"
#include "FretboardMapper.h"
#include "StringVoice.h"

#if defined (GUITAR_AG_ENABLE_STATEFUL_ENGINE)
#include "StatefulStringVoice.h"
#endif

#include <array>
#include <atomic>
#include <cstdint>
#include <memory>

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

    struct PlayerFeelMeters
    {
        float cognitiveLoad = 0.0f;
        float dexterityLoad = 0.0f;
        float endurance = 0.0f;
    };

    static constexpr auto stringCount = 6;
    static constexpr auto maxDiagnosticEvents = 1000;

    enum class StringEngine
    {
        LegacyModal,
        StatefulWaveguide
    };

    struct StringStatus
    {
        int openNote = 40;
        int mapperNoteNumber = -1;
        int mapperChannel = -1;
        int mapperFret = -1;
        int voiceNoteNumber = -1;
        int voiceChannel = -1;
        int voiceFret = -1;
        bool mapperOccupied = false;
        bool voiceActive = false;
    };

    struct DiagnosticEvent
    {
        uint64_t sequence = 0;
        int type = 0;
        int64_t sample = 0;
        int blockSample = -1;
        int channel = -1;
        int hostNoteNumber = -1;
        int engineNoteNumber = -1;
        int velocity = -1;
        int controllerNumber = -1;
        int controllerValue = -1;
        int stringIndex = -1;
        int fret = -1;
        int preferredString = -1;
        int strumPreferredString = -1;
        int legatoSourceString = -1;
        int stolenString = -1;
        int stolenNoteNumber = -1;
        int mapperMaskBefore = 0;
        int mapperMaskAfter = 0;
        int voiceMaskBefore = 0;
        int voiceMaskAfter = 0;
        int dropSemitones = 0;
        int inputTransposeSemitones = 0;
        float neckSlideSemitones = 0.0f;
        float legatoArticulation = 0.0f;
    };

    void prepare (double sampleRate, int maximumBlockSize, int outputChannels);
    void reset();
    void setStringEngine (StringEngine newStringEngine) noexcept;
    void setStatefulRepickEnabled (bool shouldPreserveState) noexcept;
#if defined (GUITAR_AG_ENABLE_OFFLINE_ABLATION)
    void setLegacyOfflineLayerState (bool attackModesEnabled,
                                     bool pickTransientEnabled,
                                     bool contactLayerEnabled) noexcept;
    void setLegacyOfflineModalPickExcitationEnabled (bool shouldUseModalPickExcitation) noexcept;
    void setLegacyOfflineModalPickForceScale (float forceScale) noexcept;
    void setLegacyOfflineModalPickDirectMix (float directMix) noexcept;
    void setLegacyOfflinePickTextureDensity (float textureDensity) noexcept;
    void setLegacyOfflineRegisterEnvelopeAnchor (float anchorAmount) noexcept;
    void setLegacyOfflineRegisterDecayAnchor (float anchorAmount) noexcept;
    void setLegacyOfflineRegisterMetalRestoration (float restorationFactor) noexcept;
    void setLegacyOfflinePluckPosition (float normalizedPosition) noexcept;
    void setLegacyOfflineBodyDecayTimeScale (float timeScale) noexcept;
    void setLegacyOfflineRepickCrossfadeMilliseconds (float durationMilliseconds) noexcept;
#endif
    void setTailSustain (float newTailSustain) noexcept;
    void setInputTransposeSemitones (int newInputTransposeSemitones) noexcept;
    void setPickStiffness (float newPickStiffness) noexcept;
    void setPickTexture (float newPickTexture) noexcept;
    void setPickBite (float newPickBite) noexcept;
    void setPickStrokeMode (int newPickStrokeMode) noexcept;
    void setStrumSpeed (float newStrumSpeed) noexcept;
    void setStrumBalance (float newStrumBalance) noexcept;
    void setPlayerFeel (float newPlayerFeel) noexcept;
    void setPlayerFeelRecoverySeconds (float newPlayerFeelRecoverySeconds) noexcept;
    void resetPlayerFeel() noexcept;
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
    void setSlideSqueak (float newSlideSqueak) noexcept;
    void setSlideSqueakDown (float newSlideSqueakDown) noexcept;
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
    void panicReset() noexcept;

    void render (juce::AudioBuffer<float>& audio, const juce::MidiBuffer& midi);
    void setPerformanceStats (PerformanceStats* stats) noexcept;
    [[nodiscard]] PlayerFeelMeters getPlayerFeelMeters() const noexcept;
    [[nodiscard]] int getActiveVoiceCount() const noexcept;
    [[nodiscard]] int getActiveFingerNoiseVoiceCount() const noexcept;
    [[nodiscard]] std::array<StringStatus, stringCount> getStringStatuses() const noexcept;
    void copyDiagnosticEvents (std::array<DiagnosticEvent, maxDiagnosticEvents>& destination, int& eventCount) const noexcept;

private:
    static constexpr auto maxVoices = stringCount;
    static constexpr auto maxScheduledMidiEvents = 256;
    static constexpr auto maxIncomingMidiGroup = 32;
    static constexpr auto maxFingerNoiseVoices = 12;
    static constexpr auto feedbackResonatorCount = 8;

    struct IncomingMidiGroup;
    struct LegatoSource;
    struct PlayerFeelResult;

    void renderRange (juce::AudioBuffer<float>& audio, int startSample, int endSample) noexcept;
    [[nodiscard]] juce::MidiMessage transposeIncomingMidiMessage (const juce::MidiMessage& message) const noexcept;
    void handleIncomingMidiGroup (const IncomingMidiGroup& group);
    [[nodiscard]] bool handleAutoStrumGroup (const IncomingMidiGroup& group);
    void handleIncomingMidiMessage (const juce::MidiMessage& message);
    void releaseAllNotes() noexcept;
    void resetMidiControllers() noexcept;
    void reconcileFretboardOccupancy() noexcept;
    void handleIncomingNoteOn (const juce::MidiMessage& message,
                               int additionalDelaySamples = 0,
                               int preferredStringIndex = -1);
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
    void rememberPendingStrumAssignment (int noteNumber, int channel, int64_t sampleTime, int stringIndex) noexcept;
    [[nodiscard]] int consumePendingStrumString (int noteNumber, int channel) noexcept;
    void triggerFingerApproach (int noteNumber, int channel, float velocity) noexcept;
    void triggerFingerRelease (int noteNumber, int channel) noexcept;
    [[nodiscard]] LegatoSource findLegatoSource (int noteNumber, int channel, float amount) const noexcept;
    void rememberArticulationNote (int noteNumber,
                                   int channel,
                                   const FretboardAssignment& assignment,
                                   PlayerGesture gesture) noexcept;
    void releaseArticulationNote (int noteNumber, int channel) noexcept;
    void releaseLegatoSource (const LegatoSource& source) noexcept;
    [[nodiscard]] PickStrokeDirection resolvePickStrokeDirection (PlayerGesture gesture,
                                                                  const FretboardAssignment& assignment) noexcept;
    [[nodiscard]] uint32_t makePickAttackSeed (int noteNumber,
                                               int channel,
                                               const FretboardAssignment& assignment,
                                               PlayerGesture gesture,
                                               PickStrokeDirection strokeDirection) noexcept;
    [[nodiscard]] static uint32_t mixPickAttackSeed (uint32_t value) noexcept;
    [[nodiscard]] static float getDeterministicGestureChance (int noteNumber,
                                                              int channel,
                                                              int sourceNoteNumber,
                                                              int64_t sampleTime) noexcept;
    [[nodiscard]] PlayerFeelResult processPlayerFeelNoteOn (const juce::MidiMessage& message,
                                                            int additionalDelaySamples = 0,
                                                            int preferredStringIndex = -1) noexcept;
    void decayPlayerFeelLoads (int64_t sampleTime) noexcept;
    void releasePlayerFeelNote (int noteNumber, int channel) noexcept;
    [[nodiscard]] float getPlayerFeelNoise (uint32_t salt, int64_t sampleTime) const noexcept;
    [[nodiscard]] static int getDirectionSign (int value) noexcept;
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
    void recordIncomingMidiDiagnostic (const juce::MidiMessage& hostMessage,
                                       const juce::MidiMessage& engineMessage,
                                       int blockSample,
                                       int64_t absoluteSample) noexcept;
    void recordDiagnosticEvent (DiagnosticEvent event) noexcept;
    [[nodiscard]] int getMapperOccupancyMask() const noexcept;
    [[nodiscard]] int getVoiceOccupancyMask() const noexcept;

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

    struct PlayerFeelResult
    {
        juce::MidiMessage message;
        int delaySamples = 0;
    };

    struct IncomingMidiGroup
    {
        std::array<juce::MidiMessage, maxIncomingMidiGroup> messages {};
        int count = 0;
    };

    struct AutoStrumNote
    {
        juce::MidiMessage message;
        FretboardAssignment assignment {};
        int originalIndex = 0;
        bool active = false;
    };

    struct PendingStrumAssignment
    {
        int64_t sampleTime = 0;
        int noteNumber = -1;
        int channel = -1;
        int stringIndex = -1;
        bool active = false;
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

    struct DiagnosticEventSlot
    {
        std::atomic<uint64_t> sequence { 0 };
        std::atomic<int> type { 0 };
        std::atomic<int64_t> sample { 0 };
        std::atomic<int> blockSample { -1 };
        std::atomic<int> channel { -1 };
        std::atomic<int> hostNoteNumber { -1 };
        std::atomic<int> engineNoteNumber { -1 };
        std::atomic<int> velocity { -1 };
        std::atomic<int> controllerNumber { -1 };
        std::atomic<int> controllerValue { -1 };
        std::atomic<int> stringIndex { -1 };
        std::atomic<int> fret { -1 };
        std::atomic<int> preferredString { -1 };
        std::atomic<int> strumPreferredString { -1 };
        std::atomic<int> legatoSourceString { -1 };
        std::atomic<int> stolenString { -1 };
        std::atomic<int> stolenNoteNumber { -1 };
        std::atomic<int> mapperMaskBefore { 0 };
        std::atomic<int> mapperMaskAfter { 0 };
        std::atomic<int> voiceMaskBefore { 0 };
        std::atomic<int> voiceMaskAfter { 0 };
        std::atomic<int> dropSemitones { 0 };
        std::atomic<int> inputTransposeSemitones { 0 };
        std::atomic<float> neckSlideSemitones { 0.0f };
        std::atomic<float> legatoArticulation { 0.0f };
    };

    std::array<StringVoice, maxVoices> voices;
#if defined (GUITAR_AG_ENABLE_OFFLINE_ABLATION)
    std::unique_ptr<std::array<StringVoice, stringCount>> offlineRestartTailVoices;
    float legacyOfflineRepickCrossfadeMilliseconds = 0.0f;
#endif
#if defined (GUITAR_AG_ENABLE_STATEFUL_ENGINE)
    std::array<StatefulStringVoice, maxVoices> statefulVoices;
#endif
    std::array<ScheduledMidiEvent, maxScheduledMidiEvents> scheduledMidiEvents {};
    std::array<PendingStrumAssignment, maxScheduledMidiEvents> pendingStrumAssignments {};
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
    FretboardMapper playerFeelFretboard;
    std::array<DiagnosticEventSlot, maxDiagnosticEvents> diagnosticEvents {};
    std::atomic<uint64_t> diagnosticSequence { 0 };
    ElectricGuitarTone tone;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> tailSustain { 1.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pickStiffness { 0.5f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pickTexture { 0.25f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pickBite { 0.5f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> strumSpeed { 0.10f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> strumBalance { -0.13f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> playerFeel { 0.5f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> playerFeelRecoverySeconds { 2.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> palmMute { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> harmonicTouch { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> stringAge { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> bridgeIntonation { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> fretPressure { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> fingerNoise { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> neckSlideSemitones { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> slideFretSteps { 0.65f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> slideLift { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> slideSqueak { 0.20f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> slideSqueakDown { 0.20f };
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
    enum class PickStrokeMode
    {
        Down = 0,
        Up = 1,
        Alternate = 2
    };

    double sampleRate = 44100.0;
    PerformanceStats* performanceStats = nullptr;
    std::array<float, 16> mpePitchBendByChannel {};
    int64_t timelineSample = 0;
    int lookaheadSamples = 0;
    int pickupModel = 0;
    int inputTransposeSemitones = 0;
    PickStrokeMode pickStrokeMode = PickStrokeMode::Alternate;
    int nextVoice = 0;
    int nextFingerNoiseVoice = 0;
    int lastPickedStringIndex = -1;
    int playerFeelLastStringIndex = -1;
    int playerFeelLastFret = -1;
    int playerFeelLastNoteNumber = -1;
    int playerFeelLastTravelSign = 0;
    int64_t playerFeelLastEventSample = -1;
    int64_t playerFeelLastLoadDecaySample = -1;
    uint32_t pickAttackCounter = 0;
    uint32_t playerFeelEventCounter = 0;
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
    bool nextAlternatePickDown = true;
    StringEngine stringEngine = StringEngine::LegacyModal;
    float playerFeelCognitiveLoad = 0.0f;
    float playerFeelDexterityLoad = 0.0f;
    float playerFeelEndurance = 0.0f;
    PickStrokeDirection lastPickStrokeDirection = PickStrokeDirection::Up;
    std::array<float, 16> mpePressureByChannel {};
    std::array<float, 16> mpeTimbreByChannel {};
};

} // namespace guitar_ag
