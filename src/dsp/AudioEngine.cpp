#include "AudioEngine.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace guitar_ag
{

namespace
{
constexpr std::array<float, 8> feedbackBandFrequencies {
    110.0f, 164.8f, 246.9f, 392.0f, 659.3f, 987.8f, 1480.0f, 2349.3f
};

constexpr std::array<float, 8> feedbackBandBias {
    0.34f, 0.48f, 0.66f, 0.86f, 1.08f, 1.20f, 1.10f, 0.82f
};

constexpr auto lowerMpeMasterChannel = 1;

bool shouldDelayForLookahead (const juce::MidiMessage& message) noexcept
{
    if (message.isNoteOnOrOff()
        || message.isAftertouch()
        || message.isChannelPressure()
        || message.isPitchWheel())
        return true;

    if (message.isController())
    {
        const auto controllerNumber = message.getControllerNumber();
        return controllerNumber == 1 || controllerNumber == 74;
    }

    return false;
}
} // namespace

void AudioEngine::prepare (double newSampleRate, int, int)
{
    sampleRate = newSampleRate > 0.0 ? newSampleRate : 44100.0;

    for (auto& voice : voices)
        voice.prepare (sampleRate);

    tailSustain.reset (sampleRate, 0.035);
    tailSustain.setCurrentAndTargetValue (1.0f);
    pickStiffness.reset (sampleRate, 0.035);
    pickStiffness.setCurrentAndTargetValue (0.5f);
    pickTexture.reset (sampleRate, 0.035);
    pickTexture.setCurrentAndTargetValue (0.25f);
    pickBite.reset (sampleRate, 0.035);
    pickBite.setCurrentAndTargetValue (0.5f);
    strumSpeed.reset (sampleRate, 0.050);
    strumSpeed.setCurrentAndTargetValue (0.10f);
    strumBalance.reset (sampleRate, 0.050);
    strumBalance.setCurrentAndTargetValue (-0.13f);
    playerFeel.reset (sampleRate, 0.050);
    playerFeel.setCurrentAndTargetValue (0.5f);
    playerFeelRecoverySeconds.reset (sampleRate, 0.050);
    playerFeelRecoverySeconds.setCurrentAndTargetValue (2.0f);
    palmMute.reset (sampleRate, 0.020);
    palmMute.setCurrentAndTargetValue (0.0f);
    harmonicTouch.reset (sampleRate, 0.020);
    harmonicTouch.setCurrentAndTargetValue (0.0f);
    stringAge.reset (sampleRate, 0.050);
    stringAge.setCurrentAndTargetValue (0.0f);
    bridgeIntonation.reset (sampleRate, 0.050);
    bridgeIntonation.setCurrentAndTargetValue (0.0f);
    fretPressure.reset (sampleRate, 0.050);
    fretPressure.setCurrentAndTargetValue (0.0f);
    fingerNoise.reset (sampleRate, 0.030);
    fingerNoise.setCurrentAndTargetValue (0.0f);
    neckSlideSemitones.reset (sampleRate, 0.030);
    neckSlideSemitones.setCurrentAndTargetValue (0.0f);
    slideFretSteps.reset (sampleRate, 0.030);
    slideFretSteps.setCurrentAndTargetValue (0.65f);
    slideLift.reset (sampleRate, 0.030);
    slideLift.setCurrentAndTargetValue (0.0f);
    slideSqueak.reset (sampleRate, 0.030);
    slideSqueak.setCurrentAndTargetValue (0.20f);
    slideSqueakDown.reset (sampleRate, 0.030);
    slideSqueakDown.setCurrentAndTargetValue (0.20f);
    legatoArticulation.reset (sampleRate, 0.035);
    legatoArticulation.setCurrentAndTargetValue (0.0f);
    ampFeedback.reset (sampleRate, 0.080);
    ampFeedback.setCurrentAndTargetValue (0.0f);
    vibratoSpeed.reset (sampleRate, 0.030);
    vibratoSpeed.setCurrentAndTargetValue (5.5f);
    vibratoDepth.reset (sampleRate, 0.030);
    vibratoDepth.setCurrentAndTargetValue (0.0f);
    vibratoDelay.reset (sampleRate, 0.030);
    vibratoDelay.setCurrentAndTargetValue (0.0f);
    modWheel.reset (sampleRate, 0.020);
    modWheel.setCurrentAndTargetValue (0.0f);
    pitchWheel.reset (sampleRate, 0.010);
    pitchWheel.setCurrentAndTargetValue (0.0f);
    mpePitchBendRange.reset (sampleRate, 0.030);
    mpePitchBendRange.setCurrentAndTargetValue (48.0f);
    mpePressureAmount.reset (sampleRate, 0.030);
    mpePressureAmount.setCurrentAndTargetValue (0.65f);
    mpeTimbreAmount.reset (sampleRate, 0.030);
    mpeTimbreAmount.setCurrentAndTargetValue (0.65f);
    whammyUpSemitones.reset (sampleRate, 0.030);
    whammyUpSemitones.setCurrentAndTargetValue (6.0f);
    whammyDownSemitones.reset (sampleRate, 0.030);
    whammyDownSemitones.setCurrentAndTargetValue (12.0f);
    whammySpread.reset (sampleRate, 0.030);
    whammySpread.setCurrentAndTargetValue (0.35f);
    aftertouchBendSemitones.reset (sampleRate, 0.030);
    aftertouchBendSemitones.setCurrentAndTargetValue (2.0f);
    pickupPosition.reset (sampleRate, 0.050);
    pickupPosition.setCurrentAndTargetValue (0.39f);
    pickupModel = 0;
    configureAmpFeedbackLoop();
    tone.prepare (sampleRate);
    reset();
}

void AudioEngine::reset()
{
    for (auto& voice : voices)
        voice.reset();

    fretboard.reset();
    fingerNoiseFretboard.reset();
    playerFeelFretboard.reset();
    tone.reset();
    clearScheduledMidiEvents();

    for (auto& assignment : fingerAssignments)
        assignment = {};

    for (auto& note : articulationNotes)
        note = {};

    for (auto& assignment : pendingStrumAssignments)
        assignment = {};

    for (auto& voice : fingerNoiseVoices)
        voice = {};

    resetAmpFeedbackLoop();

    tailSustain.setCurrentAndTargetValue (tailSustain.getTargetValue());
    pickStiffness.setCurrentAndTargetValue (pickStiffness.getTargetValue());
    pickTexture.setCurrentAndTargetValue (pickTexture.getTargetValue());
    pickBite.setCurrentAndTargetValue (pickBite.getTargetValue());
    strumSpeed.setCurrentAndTargetValue (strumSpeed.getTargetValue());
    strumBalance.setCurrentAndTargetValue (strumBalance.getTargetValue());
    playerFeel.setCurrentAndTargetValue (playerFeel.getTargetValue());
    playerFeelRecoverySeconds.setCurrentAndTargetValue (playerFeelRecoverySeconds.getTargetValue());
    palmMute.setCurrentAndTargetValue (palmMute.getTargetValue());
    harmonicTouch.setCurrentAndTargetValue (harmonicTouch.getTargetValue());
    stringAge.setCurrentAndTargetValue (stringAge.getTargetValue());
    bridgeIntonation.setCurrentAndTargetValue (bridgeIntonation.getTargetValue());
    fretPressure.setCurrentAndTargetValue (fretPressure.getTargetValue());
    fingerNoise.setCurrentAndTargetValue (fingerNoise.getTargetValue());
    neckSlideSemitones.setCurrentAndTargetValue (neckSlideSemitones.getTargetValue());
    slideFretSteps.setCurrentAndTargetValue (slideFretSteps.getTargetValue());
    slideLift.setCurrentAndTargetValue (slideLift.getTargetValue());
    slideSqueak.setCurrentAndTargetValue (slideSqueak.getTargetValue());
    slideSqueakDown.setCurrentAndTargetValue (slideSqueakDown.getTargetValue());
    legatoArticulation.setCurrentAndTargetValue (legatoArticulation.getTargetValue());
    ampFeedback.setCurrentAndTargetValue (ampFeedback.getTargetValue());
    vibratoSpeed.setCurrentAndTargetValue (vibratoSpeed.getTargetValue());
    vibratoDepth.setCurrentAndTargetValue (vibratoDepth.getTargetValue());
    vibratoDelay.setCurrentAndTargetValue (vibratoDelay.getTargetValue());
    modWheel.setCurrentAndTargetValue (modWheel.getTargetValue());
    pitchWheel.setCurrentAndTargetValue (pitchWheel.getTargetValue());
    mpePitchBendRange.setCurrentAndTargetValue (mpePitchBendRange.getTargetValue());
    mpePressureAmount.setCurrentAndTargetValue (mpePressureAmount.getTargetValue());
    mpeTimbreAmount.setCurrentAndTargetValue (mpeTimbreAmount.getTargetValue());
    whammyUpSemitones.setCurrentAndTargetValue (whammyUpSemitones.getTargetValue());
    whammyDownSemitones.setCurrentAndTargetValue (whammyDownSemitones.getTargetValue());
    whammySpread.setCurrentAndTargetValue (whammySpread.getTargetValue());
    aftertouchBendSemitones.setCurrentAndTargetValue (aftertouchBendSemitones.getTargetValue());
    pickupPosition.setCurrentAndTargetValue (pickupPosition.getTargetValue());
    mpePitchBendByChannel.fill (0.0f);
    mpePressureByChannel.fill (0.0f);
    mpeTimbreByChannel.fill (0.0f);
    timelineSample = 0;
    nextVoice = 0;
    nextFingerNoiseVoice = 0;
    lastPickedStringIndex = -1;
    resetPlayerFeel();
    pickAttackCounter = 0;
    nextAlternatePickDown = true;
    lastPickStrokeDirection = PickStrokeDirection::Up;
}

void AudioEngine::setTailSustain (float newTailSustain) noexcept
{
    tailSustain.setTargetValue (juce::jlimit (0.0f, 1.0f, newTailSustain));
}

void AudioEngine::setInputTransposeSemitones (int newInputTransposeSemitones) noexcept
{
    const auto clampedTranspose = juce::jlimit (-24, 24, newInputTransposeSemitones);

    if (clampedTranspose == inputTransposeSemitones)
        return;

    inputTransposeSemitones = clampedTranspose;
    reset();
}

void AudioEngine::setPickStiffness (float newPickStiffness) noexcept
{
    pickStiffness.setTargetValue (juce::jlimit (0.0f, 1.0f, newPickStiffness));
}

void AudioEngine::setPickTexture (float newPickTexture) noexcept
{
    pickTexture.setTargetValue (juce::jlimit (0.0f, 1.0f, newPickTexture));
}

void AudioEngine::setPickBite (float newPickBite) noexcept
{
    pickBite.setTargetValue (juce::jlimit (0.0f, 1.0f, newPickBite));
}

void AudioEngine::setPickStrokeMode (int newPickStrokeMode) noexcept
{
    const auto clampedMode = juce::jlimit (0, 2, newPickStrokeMode);
    const auto oldMode = pickStrokeMode;
    pickStrokeMode = static_cast<PickStrokeMode> (clampedMode);

    if (oldMode != pickStrokeMode && pickStrokeMode == PickStrokeMode::Alternate)
    {
        lastPickedStringIndex = -1;
        nextAlternatePickDown = true;
        lastPickStrokeDirection = PickStrokeDirection::Up;
    }
}

void AudioEngine::setStrumSpeed (float newStrumSpeed) noexcept
{
    strumSpeed.setTargetValue (juce::jlimit (0.0f, 1.0f, newStrumSpeed));
}

void AudioEngine::setStrumBalance (float newStrumBalance) noexcept
{
    strumBalance.setTargetValue (juce::jlimit (-1.0f, 1.0f, newStrumBalance));
}

void AudioEngine::setPlayerFeel (float newPlayerFeel) noexcept
{
    playerFeel.setTargetValue (juce::jlimit (0.0f, 1.0f, newPlayerFeel));
}

void AudioEngine::setPlayerFeelRecoverySeconds (float newPlayerFeelRecoverySeconds) noexcept
{
    playerFeelRecoverySeconds.setTargetValue (juce::jlimit (0.10f, 8.0f, newPlayerFeelRecoverySeconds));
}

void AudioEngine::resetPlayerFeel() noexcept
{
    playerFeelFretboard.reset();
    playerFeelLastStringIndex = -1;
    playerFeelLastFret = -1;
    playerFeelLastNoteNumber = -1;
    playerFeelLastTravelSign = 0;
    playerFeelLastEventSample = -1;
    playerFeelLastLoadDecaySample = -1;
    playerFeelEventCounter = 0;
    playerFeelCognitiveLoad = 0.0f;
    playerFeelDexterityLoad = 0.0f;
    playerFeelEndurance = 0.0f;
}

void AudioEngine::setPalmMute (float newPalmMute) noexcept
{
    palmMute.setTargetValue (juce::jlimit (0.0f, 1.0f, newPalmMute));
}

void AudioEngine::setHarmonicTouch (float newHarmonicTouch) noexcept
{
    harmonicTouch.setTargetValue (juce::jlimit (0.0f, 1.0f, newHarmonicTouch));
}

void AudioEngine::setStringAge (float newStringAge) noexcept
{
    stringAge.setTargetValue (juce::jlimit (0.0f, 1.0f, newStringAge));
}

void AudioEngine::setBridgeIntonation (float newBridgeIntonation) noexcept
{
    bridgeIntonation.setTargetValue (juce::jlimit (0.0f, 1.0f, newBridgeIntonation));
}

void AudioEngine::setFretPressure (float newFretPressure) noexcept
{
    fretPressure.setTargetValue (juce::jlimit (0.0f, 1.0f, newFretPressure));
}

void AudioEngine::setLookaheadSamples (int newLookaheadSamples) noexcept
{
    const auto clampedLookahead = juce::jlimit (0, 48000, newLookaheadSamples);

    if (lookaheadSamples != clampedLookahead)
    {
        lookaheadSamples = clampedLookahead;
        clearScheduledMidiEvents();
    }
}

void AudioEngine::setFingerNoise (float newFingerNoise) noexcept
{
    fingerNoise.setTargetValue (juce::jlimit (0.0f, 1.0f, newFingerNoise));
}

void AudioEngine::setNeckSlideSemitones (float newNeckSlideSemitones) noexcept
{
    const auto clampedSlide = juce::jlimit (-12.0f, 12.0f, newNeckSlideSemitones);
    neckSlideSemitones.setTargetValue (clampedSlide);

    if (std::abs (clampedSlide) <= 0.0001f)
        neckSlideSemitones.setCurrentAndTargetValue (0.0f);
}

void AudioEngine::setSlideFretSteps (float newSlideFretSteps) noexcept
{
    slideFretSteps.setTargetValue (juce::jlimit (0.0f, 1.0f, newSlideFretSteps));
}

void AudioEngine::setSlideLift (float newSlideLift) noexcept
{
    slideLift.setTargetValue (juce::jlimit (0.0f, 1.0f, newSlideLift));
}

void AudioEngine::setSlideSqueak (float newSlideSqueak) noexcept
{
    slideSqueak.setTargetValue (juce::jlimit (0.0f, 1.0f, newSlideSqueak));
}

void AudioEngine::setSlideSqueakDown (float newSlideSqueakDown) noexcept
{
    slideSqueakDown.setTargetValue (juce::jlimit (0.0f, 1.0f, newSlideSqueakDown));
}

void AudioEngine::setLegatoArticulation (float newLegatoArticulation) noexcept
{
    legatoArticulation.setTargetValue (juce::jlimit (0.0f, 1.0f, newLegatoArticulation));
}

void AudioEngine::setAmpFeedback (float newAmpFeedback) noexcept
{
    ampFeedback.setTargetValue (juce::jlimit (0.0f, 1.0f, newAmpFeedback));
}

void AudioEngine::setFeedbackReturnDistorted (bool shouldDistort) noexcept
{
    feedbackReturnDistorted = shouldDistort;
}

void AudioEngine::setVibratoSpeed (float newVibratoSpeed) noexcept
{
    vibratoSpeed.setTargetValue (juce::jlimit (0.10f, 12.0f, newVibratoSpeed));
}

void AudioEngine::setVibratoDepth (float newVibratoDepth) noexcept
{
    vibratoDepth.setTargetValue (juce::jlimit (0.0f, 60.0f, newVibratoDepth));
}

void AudioEngine::setVibratoDelay (float newVibratoDelay) noexcept
{
    vibratoDelay.setTargetValue (juce::jlimit (0.0f, 2.0f, newVibratoDelay));
}

void AudioEngine::setVibratoModWheelSpeedEnabled (bool enabled) noexcept
{
    vibratoModWheelSpeedEnabled = enabled;
}

void AudioEngine::setVibratoModWheelDepthEnabled (bool enabled) noexcept
{
    vibratoModWheelDepthEnabled = enabled;
}

void AudioEngine::setMpeEnabled (bool enabled) noexcept
{
    if (mpeEnabled == enabled)
        return;

    mpeEnabled = enabled;
    mpePitchBendByChannel.fill (0.0f);
    mpePressureByChannel.fill (0.0f);
    mpeTimbreByChannel.fill (0.0f);

    for (auto& voice : voices)
    {
        voice.setMpePitchBend (voice.getChannel(), 0.0f);
        voice.setMpePressure (voice.getChannel(), 0.0f);
        voice.setMpeTimbre (voice.getChannel(), 0.0f);
    }
}

void AudioEngine::setMpePitchBendRange (float newMpePitchBendRange) noexcept
{
    mpePitchBendRange.setTargetValue (juce::jlimit (0.0f, 96.0f, newMpePitchBendRange));
}

void AudioEngine::setMpePressureAmount (float newMpePressureAmount) noexcept
{
    mpePressureAmount.setTargetValue (juce::jlimit (0.0f, 1.0f, newMpePressureAmount));
}

void AudioEngine::setMpeTimbreAmount (float newMpeTimbreAmount) noexcept
{
    mpeTimbreAmount.setTargetValue (juce::jlimit (0.0f, 1.0f, newMpeTimbreAmount));
}

void AudioEngine::setWhammyEnabled (bool enabled) noexcept
{
    whammyEnabled = enabled;
}

void AudioEngine::setWhammyUpSemitones (float newWhammyUpSemitones) noexcept
{
    whammyUpSemitones.setTargetValue (juce::jlimit (0.0f, 24.0f, newWhammyUpSemitones));
}

void AudioEngine::setWhammyDownSemitones (float newWhammyDownSemitones) noexcept
{
    whammyDownSemitones.setTargetValue (juce::jlimit (0.0f, 36.0f, newWhammyDownSemitones));
}

void AudioEngine::setWhammySpread (float newWhammySpread) noexcept
{
    whammySpread.setTargetValue (juce::jlimit (0.0f, 1.0f, newWhammySpread));
}

void AudioEngine::setAftertouchBendSemitones (float newAftertouchBendSemitones) noexcept
{
    aftertouchBendSemitones.setTargetValue (juce::jlimit (-12.0f, 12.0f, newAftertouchBendSemitones));
}

void AudioEngine::setPickupPosition (float newPickupPosition) noexcept
{
    pickupPosition.setTargetValue (juce::jlimit (0.0f, 1.0f, newPickupPosition));
}

void AudioEngine::setPickupModel (int newPickupModel) noexcept
{
    pickupModel = juce::jlimit (0, 2, newPickupModel);
}

void AudioEngine::panicReset() noexcept
{
    releaseAllNotes();
}

void AudioEngine::setPerformanceStats (PerformanceStats* stats) noexcept
{
    performanceStats = stats;
}

int AudioEngine::getActiveVoiceCount() const noexcept
{
    auto count = 0;

    for (const auto& voice : voices)
    {
        if (voice.isActive())
            ++count;
    }

    return count;
}

AudioEngine::PlayerFeelMeters AudioEngine::getPlayerFeelMeters() const noexcept
{
    return { playerFeelCognitiveLoad, playerFeelDexterityLoad, playerFeelEndurance };
}

int AudioEngine::getActiveFingerNoiseVoiceCount() const noexcept
{
    auto count = 0;

    for (const auto& voice : fingerNoiseVoices)
    {
        if (voice.samplesRemaining > 0)
            ++count;
    }

    return count;
}

void AudioEngine::render (juce::AudioBuffer<float>& audio, const juce::MidiBuffer& midi)
{
    auto currentSample = 0;
    const auto totalSamples = audio.getNumSamples();
    IncomingMidiGroup group;
    auto groupSample = -1;

    for (const auto metadata : midi)
    {
        const auto eventSample = juce::jlimit (0, totalSamples, metadata.samplePosition);

        if (group.count > 0 && (eventSample != groupSample || group.count >= maxIncomingMidiGroup))
        {
            renderRange (audio, currentSample, groupSample);
            handleIncomingMidiGroup (group);
            currentSample = groupSample;
            group = {};
        }

        groupSample = eventSample;
        group.messages[static_cast<size_t> (group.count)] = transposeIncomingMidiMessage (metadata.getMessage());
        ++group.count;
    }

    if (group.count > 0)
    {
        renderRange (audio, currentSample, groupSample);
        handleIncomingMidiGroup (group);
        currentSample = groupSample;
    }

    renderRange (audio, currentSample, totalSamples);
    decayPlayerFeelLoads (timelineSample);
}

juce::MidiMessage AudioEngine::transposeIncomingMidiMessage (const juce::MidiMessage& message) const noexcept
{
    if (inputTransposeSemitones == 0 || (! message.isNoteOnOrOff() && ! message.isAftertouch()))
        return message;

    auto transposed = message;
    transposed.setNoteNumber (juce::jlimit (0, 127, message.getNoteNumber() + inputTransposeSemitones));
    return transposed;
}

void AudioEngine::renderRange (juce::AudioBuffer<float>& audio, int startSample, int endSample) noexcept
{
    const auto numChannels = audio.getNumChannels();

    for (auto sampleIndex = startSample; sampleIndex < endSample; ++sampleIndex)
    {
        auto mixedSample = 0.0f;
        const auto sustainAmount = tailSustain.getNextValue();
        const auto palmMuteAmount = palmMute.getNextValue();
        const auto fingerNoiseAmount = fingerNoise.getNextValue();
        const auto modWheelAmount = modWheel.getNextValue();
        const auto vibratoSpeedAmount = vibratoSpeed.getNextValue()
                                      + (vibratoModWheelSpeedEnabled ? modWheelAmount * 6.0f : 0.0f);
        const auto vibratoDepthAmount = vibratoDepth.getNextValue()
                                      + (vibratoModWheelDepthEnabled ? modWheelAmount * 55.0f : 0.0f);
        const auto vibratoDelaySeconds = vibratoDelay.getNextValue();
        const auto pitchWheelAmount = pitchWheel.getNextValue();
        const auto mpePitchBendRangeAmount = mpePitchBendRange.getNextValue();
        const auto mpePressureAmountValue = mpePressureAmount.getNextValue();
        const auto mpeTimbreAmountValue = mpeTimbreAmount.getNextValue();
        const auto whammyUpAmount = whammyUpSemitones.getNextValue();
        const auto whammyDownAmount = whammyDownSemitones.getNextValue();
        const auto whammySemitones = whammyEnabled
                                   ? pitchWheelAmount * (pitchWheelAmount >= 0.0f ? whammyUpAmount : whammyDownAmount)
                                   : 0.0f;
        const auto whammySpreadAmount = whammySpread.getNextValue();
        const auto requestedAmpFeedbackAmount = ampFeedback.getNextValue();
        const auto ampFeedbackAmount = requestedAmpFeedbackAmount * updateFeedbackBloom (requestedAmpFeedbackAmount);
        const auto feedbackLoopFrequencyForVoices = feedbackLoopFrequency;
        const auto feedbackLoopAmountForVoices = feedbackLoopAmount;
        const auto feedbackLoopSignalForVoices = feedbackLoopSignal;
        updateFeedbackStringFocus (ampFeedbackAmount, feedbackLoopFrequencyForVoices, feedbackLoopAmountForVoices);
        const auto aftertouchBendAmount = aftertouchBendSemitones.getNextValue();
        const auto neckSlideAmount = neckSlideSemitones.getNextValue();
        const auto slideFretStepsAmount = slideFretSteps.getNextValue();
        const auto slideLiftAmount = slideLift.getNextValue();
        const auto slideSqueakAmount = slideSqueak.getNextValue();
        const auto slideSqueakDownAmount = slideSqueakDown.getNextValue();
        pickStiffness.getNextValue();
        pickTexture.getNextValue();
        pickBite.getNextValue();
        strumSpeed.getNextValue();
        strumBalance.getNextValue();
        playerFeel.getNextValue();
        playerFeelRecoverySeconds.getNextValue();
        harmonicTouch.getNextValue();
        stringAge.getNextValue();
        bridgeIntonation.getNextValue();
        fretPressure.getNextValue();
        legatoArticulation.getNextValue();
        pickupPosition.getNextValue();

        dispatchScheduledMidiEvents();
        recordPerformanceSample();

        for (auto& voice : voices)
            mixedSample += voice.renderSample (sustainAmount,
                                               palmMuteAmount,
                                               vibratoDepthAmount,
                                               vibratoSpeedAmount,
                                               vibratoDelaySeconds,
                                               whammySemitones,
                                               whammySpreadAmount,
                                               ampFeedbackAmount,
                                               feedbackLoopFrequencyForVoices,
                                               feedbackLoopAmountForVoices,
                                               feedbackLoopSignalForVoices,
                                               feedbackDominantString,
                                               feedbackStringFocus,
                                               aftertouchBendAmount,
                                               mpePressureAmountValue,
                                               mpeTimbreAmountValue,
                                               mpePitchBendRangeAmount,
                                               neckSlideAmount,
                                               slideFretStepsAmount,
                                               slideLiftAmount,
                                               slideSqueakAmount,
                                               slideSqueakDownAmount);

        mixedSample += renderFingerNoiseSample() * fingerNoiseAmount;
        mixedSample = tone.processSample (mixedSample);
        updateAmpFeedbackLoop (mixedSample, ampFeedbackAmount);

        for (auto channel = 0; channel < numChannels; ++channel)
            audio.addSample (channel, sampleIndex, mixedSample);

        ++timelineSample;
    }
}

void AudioEngine::recordPerformanceSample() noexcept
{
    if (performanceStats == nullptr)
        return;

    const auto activeVoiceCount = getActiveVoiceCount();
    const auto activeFingerNoiseVoiceCount = getActiveFingerNoiseVoiceCount();

    ++performanceStats->renderedSamples;
    performanceStats->activeVoiceSamples += static_cast<uint64_t> (activeVoiceCount);
    performanceStats->activeFingerNoiseSamples += static_cast<uint64_t> (activeFingerNoiseVoiceCount);
    performanceStats->maxActiveVoices = juce::jmax (performanceStats->maxActiveVoices, activeVoiceCount);
    performanceStats->maxActiveFingerNoiseVoices = juce::jmax (performanceStats->maxActiveFingerNoiseVoices,
                                                               activeFingerNoiseVoiceCount);
}

void AudioEngine::configureAmpFeedbackLoop() noexcept
{
    constexpr auto twoPi = 6.28318530717958647692f;

    for (auto index = 0; index < feedbackResonatorCount; ++index)
    {
        const auto band = static_cast<size_t> (index);
        const auto frequency = juce::jlimit (40.0f,
                                             static_cast<float> (sampleRate * 0.42),
                                             feedbackBandFrequencies[band]);
        const auto radius = 0.9925f + 0.0030f * static_cast<float> (index) / static_cast<float> (feedbackResonatorCount - 1);
        feedbackResonatorCoefficient[band] = 2.0f * radius * std::cos (twoPi * frequency / static_cast<float> (sampleRate));
        feedbackResonatorRadiusSquared[band] = radius * radius;
    }
}

void AudioEngine::resetAmpFeedbackLoop() noexcept
{
    feedbackResonatorState1.fill (0.0f);
    feedbackResonatorState2.fill (0.0f);
    feedbackResonatorEnvelope.fill (0.0f);
    feedbackDominantBand = 0;
    feedbackLoopFrequency = 0.0f;
    feedbackLoopAmount = 0.0f;
    feedbackLoopSignal = 0.0f;
    feedbackLoopDominance = 0.0f;
    feedbackDominantString = -1;
    feedbackFocusUpdateCountdown = 0;
    feedbackStringFocus = 0.0f;
    feedbackStringFocusTarget = 0.0f;
    feedbackStringDominance = 0.0f;
    feedbackBloom = 1.0f;
    lastFeedbackBloomDuckSample = -1000000000;
}

void AudioEngine::triggerFeedbackBloomDuck (float velocity, PlayerGesture gesture) noexcept
{
    const auto feedbackTarget = juce::jlimit (0.0f, 1.0f, ampFeedback.getTargetValue());

    if (feedbackTarget <= 0.015f)
        return;

    const auto gestureScale = gesture == PlayerGesture::Picked ? 1.0f
                            : gesture == PlayerGesture::RightHandTap ? 0.78f
                            : gesture == PlayerGesture::HammerOn ? 0.70f
                            : 0.54f;
    const auto normalizedVelocity = juce::jlimit (0.0f, 1.0f, velocity);
    const auto feedbackScale = 0.35f + 0.65f * std::sqrt (feedbackTarget);
    const auto duckStrength = juce::jlimit (0.0f,
                                            1.0f,
                                            (0.55f + 0.45f * normalizedVelocity) * gestureScale * feedbackScale);
    const auto fullDuckFloor = 0.018f + 0.070f * (1.0f - normalizedVelocity);
    const auto lightDuckFloor = 0.42f;
    const auto targetBloom = juce::jlimit (0.018f,
                                           lightDuckFloor,
                                           fullDuckFloor + (lightDuckFloor - fullDuckFloor) * (1.0f - duckStrength));

    feedbackBloom = juce::jmin (feedbackBloom, targetBloom);

    const auto clusterWindowSamples = static_cast<int64_t> (sampleRate * 0.070);
    const auto sameAttackCluster = timelineSample - lastFeedbackBloomDuckSample <= clusterWindowSamples;
    lastFeedbackBloomDuckSample = timelineSample;

    if (sameAttackCluster)
        return;

    const auto loopFlush = juce::jlimit (0.0f, 0.84f, duckStrength * (0.45f + 0.45f * feedbackTarget));
    const auto loopKeep = 1.0f - loopFlush;
    const auto stateKeep = 0.30f + 0.70f * loopKeep;

    feedbackLoopAmount *= loopKeep;
    feedbackLoopSignal *= loopKeep * 0.84f;
    feedbackLoopDominance *= 0.55f + 0.45f * loopKeep;

    for (auto index = 0; index < feedbackResonatorCount; ++index)
    {
        const auto band = static_cast<size_t> (index);
        feedbackResonatorState1[band] *= stateKeep;
        feedbackResonatorState2[band] *= stateKeep;
        feedbackResonatorEnvelope[band] *= stateKeep;
    }

    feedbackDominantString = -1;
    feedbackStringFocus *= 0.40f + 0.40f * loopKeep;
    feedbackStringFocusTarget = 0.0f;
    feedbackStringDominance *= 0.50f + 0.35f * loopKeep;
}

float AudioEngine::updateFeedbackBloom (float amount) noexcept
{
    const auto feedbackAmount = juce::jlimit (0.0f, 1.0f, amount);

    if (feedbackAmount <= 0.001f)
    {
        feedbackBloom = 1.0f;
        return 1.0f;
    }

    const auto bloomSeconds = 0.68f + 0.82f * std::pow (feedbackAmount, 0.72f);
    const auto bloomStep = 1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate) * bloomSeconds);
    feedbackBloom = juce::jmin (1.0f, feedbackBloom + bloomStep);

    return feedbackBloom * feedbackBloom * (3.0f - 2.0f * feedbackBloom);
}

void AudioEngine::updateAmpFeedbackLoop (float outputSample, float amount) noexcept
{
    const auto feedbackAmount = juce::jlimit (0.0f, 1.0f, amount);
    const auto loopAmount = std::pow (juce::jlimit (0.0f, 1.0f, (feedbackAmount - 0.28f) / 0.72f), 1.18f);

    if (feedbackAmount <= 0.001f)
    {
        for (auto index = 0; index < feedbackResonatorCount; ++index)
        {
            const auto band = static_cast<size_t> (index);
            feedbackResonatorState1[band] *= 0.985f;
            feedbackResonatorState2[band] *= 0.985f;
            feedbackResonatorEnvelope[band] *= 0.985f;
        }

        feedbackLoopFrequency = 0.0f;
        feedbackLoopAmount *= 0.985f;
        feedbackLoopSignal *= 0.965f;
        feedbackLoopDominance *= 0.985f;
        return;
    }

    const auto inputScale = std::pow (feedbackAmount, 0.45f);
    const auto cleanInput = std::tanh (outputSample * (0.60f + 4.8f * feedbackAmount)) * inputScale;
    const auto clippedInput = std::tanh (outputSample * (2.20f + 15.0f * feedbackAmount)) * inputScale;
    const auto input = feedbackReturnDistorted ? 0.28f * cleanInput + 0.72f * clippedInput
                                               : cleanInput;
    auto bestScore = -1.0f;
    auto secondScore = -1.0f;
    auto bestIndex = feedbackDominantBand;
    auto bestSignal = 0.0f;

    for (auto index = 0; index < feedbackResonatorCount; ++index)
    {
        const auto band = static_cast<size_t> (index);
        const auto next = juce::jlimit (-4.0f,
                                        4.0f,
                                        input
                                            + feedbackResonatorCoefficient[band] * feedbackResonatorState1[band]
                                            - feedbackResonatorRadiusSquared[band] * feedbackResonatorState2[band]);
        const auto bandSignal = next - feedbackResonatorState2[band];
        feedbackResonatorState2[band] = feedbackResonatorState1[band];
        feedbackResonatorState1[band] = next;

        const auto rectified = std::abs (bandSignal);
        const auto attack = 0.00140f + 0.00320f * loopAmount;
        const auto release = 0.000050f + 0.000220f * loopAmount;
        const auto envelopeStep = rectified > feedbackResonatorEnvelope[band] ? attack : release;
        feedbackResonatorEnvelope[band] += envelopeStep * (rectified - feedbackResonatorEnvelope[band]);

        const auto currentWinnerBonus = index == feedbackDominantBand ? 1.08f : 1.0f;
        const auto score = feedbackResonatorEnvelope[band] * feedbackBandBias[band] * currentWinnerBonus;

        if (score > bestScore)
        {
            secondScore = bestScore;
            bestScore = score;
            bestIndex = index;
            bestSignal = bandSignal;
        }
        else if (score > secondScore)
        {
            secondScore = score;
        }
    }

    const auto dominance = bestScore > 0.000001f
                         ? juce::jlimit (0.0f, 1.0f, (bestScore - juce::jmax (0.0f, secondScore)) / (bestScore + 0.000001f))
                         : 0.0f;
    const auto takeover = bestIndex == feedbackDominantBand ? 0.00055f : 0.000075f + 0.00050f * dominance;
    feedbackDominantBand = bestIndex;
    feedbackLoopDominance += 0.0014f * (dominance - feedbackLoopDominance);

    const auto targetFrequency = feedbackBandFrequencies[static_cast<size_t> (bestIndex)];
    feedbackLoopFrequency += takeover * (targetFrequency - feedbackLoopFrequency);

    if (feedbackLoopFrequency <= 1.0f)
        feedbackLoopFrequency = targetFrequency;

    const auto targetAmount = juce::jlimit (0.0f,
                                           1.0f,
                                           loopAmount
                                               * std::sqrt (juce::jmax (0.0f, bestScore))
                                               * (2.20f + 4.00f * feedbackLoopDominance));
    feedbackLoopAmount += 0.0048f * (targetAmount - feedbackLoopAmount);
    feedbackLoopSignal += 0.24f * ((std::tanh (bestSignal * 1.35f) * feedbackLoopAmount) - feedbackLoopSignal);

}

void AudioEngine::updateFeedbackStringFocus (float amount, float loopFrequency, float loopAmount) noexcept
{
    constexpr auto focusUpdateInterval = 64;
    const auto feedbackAmount = juce::jlimit (0.0f, 1.0f, amount);
    const auto clampedLoopAmount = juce::jlimit (0.0f, 1.0f, loopAmount);
    const auto smoothFocus = [this] (float rate) noexcept
    {
        feedbackStringFocus += rate * (feedbackStringFocusTarget - feedbackStringFocus);
    };

    if (feedbackAmount < 0.30f || clampedLoopAmount <= 0.0001f || loopFrequency <= 20.0f)
    {
        feedbackStringFocusTarget = 0.0f;
        smoothFocus (0.0065f);
        feedbackStringDominance *= 0.990f;
        feedbackFocusUpdateCountdown = 0;

        if (feedbackStringFocus < 0.001f)
            feedbackDominantString = -1;

        return;
    }

    if (feedbackFocusUpdateCountdown > 0)
    {
        --feedbackFocusUpdateCountdown;
        smoothFocus (0.0040f);
        return;
    }

    feedbackFocusUpdateCountdown = focusUpdateInterval;
    auto bestScore = -1.0f;
    auto secondScore = -1.0f;
    auto bestString = feedbackDominantString;

    for (const auto& voice : voices)
    {
        if (! voice.isActive())
            continue;

        auto score = voice.getFeedbackCouplingScore (loopFrequency);

        if (voice.getStringIndex() == feedbackDominantString)
            score *= 1.12f;

        if (score > bestScore)
        {
            secondScore = bestScore;
            bestScore = score;
            bestString = voice.getStringIndex();
        }
        else if (score > secondScore)
        {
            secondScore = score;
        }
    }

    if (bestScore <= 0.0000001f || bestString < 0)
    {
        feedbackStringFocusTarget = 0.0f;
        smoothFocus (0.0065f);
        feedbackStringDominance *= 0.990f;

        if (feedbackStringFocus < 0.001f)
            feedbackDominantString = -1;

        return;
    }

    const auto dominance = juce::jlimit (0.0f,
                                        1.0f,
                                        (bestScore - juce::jmax (0.0f, secondScore)) / (bestScore + 0.000001f));

    if (feedbackDominantString < 0
        || bestString == feedbackDominantString
        || dominance > 0.10f
        || bestScore > secondScore * 1.18f)
        feedbackDominantString = bestString;

    feedbackStringFocusTarget = juce::jlimit (0.0f,
                                             1.0f,
                                             clampedLoopAmount
                                                 * std::pow (feedbackAmount, 0.62f)
                                                 * (0.42f + 1.28f * dominance));
    feedbackStringDominance += 0.0022f * (dominance - feedbackStringDominance);
    smoothFocus (0.0040f);
}

void AudioEngine::handleIncomingMidiGroup (const IncomingMidiGroup& group)
{
    if (group.count <= 0)
        return;

    for (auto index = 0; index < group.count; ++index)
    {
        if (group.messages[static_cast<size_t> (index)].isNoteOn())
        {
            reconcileFretboardOccupancy();
            break;
        }
    }

    if (group.count == 1)
    {
        handleIncomingMidiMessage (group.messages[0]);
        return;
    }

    if (handleAutoStrumGroup (group))
        return;

    for (auto index = 0; index < group.count; ++index)
        handleIncomingMidiMessage (group.messages[static_cast<size_t> (index)]);
}

bool AudioEngine::handleAutoStrumGroup (const IncomingMidiGroup& group)
{
    const auto strumAmount = juce::jlimit (0.0f, 1.0f, strumSpeed.getTargetValue());

    // Chord-aware string assignment runs for any simultaneous block chord (2+ note-ons),
    // regardless of strum setting, so partial chords get natural register-ordered strings.
    // When strumAmount == 0 perStringSeconds collapses to 0 and notes fire with zero delay.

    std::array<int, maxIncomingMidiGroup> noteIndices {};
    auto noteCount = 0;

    for (auto index = 0; index < group.count; ++index)
    {
        if (group.messages[static_cast<size_t> (index)].isNoteOn())
        {
            noteIndices[static_cast<size_t> (noteCount)] = index;
            ++noteCount;
        }
    }

    if (noteCount < 2)
        return false;

    // Groups containing non-note events (e.g. CC) return false so handleIncomingMidiGroup
    // dispatches them individually, keeping controller timing unaffected by chord logic.
    for (auto index = 0; index < group.count; ++index)
    {
        if (! group.messages[static_cast<size_t> (index)].isNoteOnOrOff())
            return false;
    }

    for (auto index = 0; index < group.count; ++index)
    {
        const auto& message = group.messages[static_cast<size_t> (index)];

        if (message.isNoteOff())
            handleIncomingMidiMessage (message);
    }

    for (auto index = 0; index < group.count; ++index)
    {
        const auto& message = group.messages[static_cast<size_t> (index)];

        if (! message.isNoteOnOrOff())
            handleIncomingMidiMessage (message);
    }

    std::sort (noteIndices.begin(), noteIndices.begin() + noteCount, [&group] (int left, int right)
    {
        const auto& leftMessage = group.messages[static_cast<size_t> (left)];
        const auto& rightMessage = group.messages[static_cast<size_t> (right)];

        if (leftMessage.getNoteNumber() != rightMessage.getNoteNumber())
            return leftMessage.getNoteNumber() < rightMessage.getNoteNumber();

        if (leftMessage.getChannel() != rightMessage.getChannel())
            return leftMessage.getChannel() < rightMessage.getChannel();

        return left < right;
    });

    auto previewFretboard = fretboard;
    std::array<AutoStrumNote, maxIncomingMidiGroup> notes {};
    auto minString = maxVoices;
    auto maxString = -1;

    if (noteCount < maxVoices)
    {
        std::array<int, maxVoices> chordNotes {};
        std::array<int, maxVoices> chordChannels {};
        std::array<FretboardAssignment, maxVoices> chordAssignments {};

        for (auto noteIndex = 0; noteIndex < noteCount; ++noteIndex)
        {
            const auto sourceIndex = noteIndices[static_cast<size_t> (noteIndex)];
            const auto& message = group.messages[static_cast<size_t> (sourceIndex)];
            chordNotes[static_cast<size_t> (noteIndex)] = message.getNoteNumber();
            chordChannels[static_cast<size_t> (noteIndex)] = message.getChannel();
        }

        previewFretboard.assignNoteGroup (chordNotes.data(), chordChannels.data(), chordAssignments.data(), noteCount);

        for (auto noteIndex = 0; noteIndex < noteCount; ++noteIndex)
        {
            const auto sourceIndex = noteIndices[static_cast<size_t> (noteIndex)];
            const auto& message = group.messages[static_cast<size_t> (sourceIndex)];
            auto& note = notes[static_cast<size_t> (noteIndex)];
            note.message = message;
            note.assignment = chordAssignments[static_cast<size_t> (noteIndex)];
            note.originalIndex = sourceIndex;
            note.active = true;

            minString = juce::jmin (minString, note.assignment.stringIndex);
            maxString = juce::jmax (maxString, note.assignment.stringIndex);
        }
    }
    else
    {
        for (auto noteIndex = 0; noteIndex < noteCount; ++noteIndex)
        {
            const auto sourceIndex = noteIndices[static_cast<size_t> (noteIndex)];
            const auto& message = group.messages[static_cast<size_t> (sourceIndex)];
            auto& note = notes[static_cast<size_t> (noteIndex)];
            note.message = message;
            note.assignment = previewFretboard.assignNote (message.getNoteNumber(), message.getChannel());
            note.originalIndex = sourceIndex;
            note.active = true;

            minString = juce::jmin (minString, note.assignment.stringIndex);
            maxString = juce::jmax (maxString, note.assignment.stringIndex);
        }
    }

    auto strokeDirection = PickStrokeDirection::Down;

    if (pickStrokeMode == PickStrokeMode::Up)
        strokeDirection = PickStrokeDirection::Up;
    else if (pickStrokeMode == PickStrokeMode::Alternate)
    {
        if (lastPickedStringIndex >= 0 && minString > lastPickedStringIndex)
            strokeDirection = PickStrokeDirection::Down;
        else if (lastPickedStringIndex >= 0 && maxString < lastPickedStringIndex)
            strokeDirection = PickStrokeDirection::Up;
        else
            strokeDirection = nextAlternatePickDown ? PickStrokeDirection::Down : PickStrokeDirection::Up;
    }

    std::sort (notes.begin(), notes.begin() + noteCount, [strokeDirection] (const auto& left, const auto& right)
    {
        if (left.assignment.stringIndex != right.assignment.stringIndex)
        {
            return strokeDirection == PickStrokeDirection::Down
                ? left.assignment.stringIndex < right.assignment.stringIndex
                : left.assignment.stringIndex > right.assignment.stringIndex;
        }

        return left.originalIndex < right.originalIndex;
    });

    const auto perStringSeconds = 0.100f * std::pow (strumAmount, 1.35f);
    const auto balanceAmount = juce::jlimit (-1.0f, 1.0f, strumBalance.getTargetValue());
    auto strokeVelocityScale = 1.0f;

    // Velocity balance only applies when there is an audible strum delay.
    if (strumAmount > 0.0001f)
    {
        if (strokeDirection == PickStrokeDirection::Up && balanceAmount > 0.0f)
            strokeVelocityScale = 1.0f - 0.94f * balanceAmount;
        else if (strokeDirection == PickStrokeDirection::Down && balanceAmount < 0.0f)
            strokeVelocityScale = 1.0f - 0.94f * std::abs (balanceAmount);
    }

    for (auto noteIndex = 0; noteIndex < noteCount; ++noteIndex)
    {
        const auto& note = notes[static_cast<size_t> (noteIndex)];
        const auto stringDistance = strokeDirection == PickStrokeDirection::Down
                                  ? note.assignment.stringIndex - minString
                                  : maxString - note.assignment.stringIndex;
        const auto delaySamples = juce::jmax (0,
                                              static_cast<int> (std::round (static_cast<double> (stringDistance)
                                                                            * static_cast<double> (perStringSeconds)
                                                                            * sampleRate)));
        auto strumMessage = note.message;

        if (strokeVelocityScale < 0.999f)
        {
            const auto scaledVelocity = juce::jlimit (1,
                                                      127,
                                                      juce::roundToInt (note.message.getFloatVelocity()
                                                                        * strokeVelocityScale
                                                                        * 127.0f));
            strumMessage = juce::MidiMessage::noteOn (note.message.getChannel(),
                                                      note.message.getNoteNumber(),
                                                      static_cast<juce::uint8> (scaledVelocity));
        }

        handleIncomingNoteOn (strumMessage, delaySamples, note.assignment.stringIndex);
    }

    return true;
}

void AudioEngine::handleIncomingNoteOn (const juce::MidiMessage& message,
                                        int additionalDelaySamples,
                                        int preferredStringIndex)
{
    const auto clampedAdditionalDelay = juce::jlimit (0,
                                                     static_cast<int> (std::round (sampleRate * 0.750)),
                                                     additionalDelaySamples);
    const auto preferredString = preferredStringIndex >= 0 && preferredStringIndex < maxVoices
                               ? preferredStringIndex
                               : -1;
    const auto feelResult = processPlayerFeelNoteOn (message, clampedAdditionalDelay, preferredString);
    const auto totalDelaySamples = lookaheadSamples + clampedAdditionalDelay + feelResult.delaySamples;
    const auto scheduledSample = timelineSample + static_cast<int64_t> (totalDelaySamples);

    if (preferredString >= 0)
        rememberPendingStrumAssignment (message.getNoteNumber(), message.getChannel(), scheduledSample, preferredString);

    if (lookaheadSamples > 0)
        triggerFingerApproach (message.getNoteNumber(), message.getChannel(), message.getFloatVelocity());

    if (totalDelaySamples > 0)
        scheduleMidiMessage (feelResult.message, scheduledSample);
    else
        handleMidiMessage (feelResult.message);
}

void AudioEngine::handleIncomingMidiMessage (const juce::MidiMessage& message)
{
    if (message.isNoteOn())
    {
        handleIncomingNoteOn (message);
        return;
    }

    if (message.isNoteOff())
    {
        releasePlayerFeelNote (message.getNoteNumber(), message.getChannel());

        if (lookaheadSamples <= 0)
        {
            handleMidiMessage (message);
            return;
        }

        triggerFingerRelease (message.getNoteNumber(), message.getChannel());
    }
    else if (lookaheadSamples <= 0)
    {
        handleMidiMessage (message);
        return;
    }

    if (shouldDelayForLookahead (message))
        scheduleMidiMessage (message, timelineSample + static_cast<int64_t> (lookaheadSamples));
    else
        handleMidiMessage (message);
}

AudioEngine::PlayerFeelResult AudioEngine::processPlayerFeelNoteOn (const juce::MidiMessage& message,
                                                                    int additionalDelaySamples,
                                                                    int preferredStringIndex) noexcept
{
    PlayerFeelResult result { message, 0 };
    const auto noteNumber = message.getNoteNumber();
    const auto channel = message.getChannel();
    const auto eventSample = timelineSample + static_cast<int64_t> (juce::jmax (0, additionalDelaySamples));
    const auto amount = juce::jlimit (0.0f, 1.0f, playerFeel.getTargetValue());
    const auto hadPrevious = playerFeelLastEventSample >= 0 && playerFeelLastStringIndex >= 0;
    const auto intervalSeconds = hadPrevious
                               ? juce::jmax (0.0f,
                                             static_cast<float> (eventSample - playerFeelLastEventSample)
                                                 / static_cast<float> (sampleRate))
                               : 0.50f;

    decayPlayerFeelLoads (eventSample);

    const auto assignment = preferredStringIndex >= 0 && preferredStringIndex < maxVoices
                          ? playerFeelFretboard.assignNote (noteNumber, channel, preferredStringIndex, 100.0f)
                          : playerFeelFretboard.assignNote (noteNumber, channel);
    const auto stringDistance = hadPrevious ? std::abs (assignment.stringIndex - playerFeelLastStringIndex) : 0;
    const auto fretDistance = hadPrevious ? std::abs (assignment.fret - playerFeelLastFret) : 0;
    const auto travelSign = hadPrevious ? getDirectionSign (assignment.stringIndex - playerFeelLastStringIndex) : 0;
    const auto sameString = hadPrevious && assignment.stringIndex == playerFeelLastStringIndex;
    const auto directionChange = travelSign != 0
                              && playerFeelLastTravelSign != 0
                              && travelSign != playerFeelLastTravelSign;
    const auto economyFlow = travelSign != 0
                          && travelSign == playerFeelLastTravelSign
                          && stringDistance == 1;
    const auto strumContinuation = hadPrevious
                                && ! sameString
                                && travelSign != 0
                                && stringDistance <= 2
                                && intervalSeconds <= 0.070f;
    const auto fast = hadPrevious
                    ? juce::jlimit (0.0f, 1.0f, (0.190f - intervalSeconds) / 0.165f)
                    : 0.0f;
    const auto veryFast = hadPrevious
                        ? juce::jlimit (0.0f, 1.0f, (0.095f - intervalSeconds) / 0.075f)
                        : 0.0f;
    const auto stringSkip = juce::jlimit (0.0f, 1.0f, static_cast<float> (juce::jmax (0, stringDistance - 1)) / 3.0f);
    const auto fretJump = juce::jlimit (0.0f, 1.0f, static_cast<float> (fretDistance) / 9.0f);

    auto cognitiveImpulse = 0.010f
                          + 0.055f * fast
                          + 0.085f * stringSkip
                          + 0.070f * fretJump
                          + (directionChange ? 0.075f : 0.0f);
    auto dexterityImpulse = 0.012f
                          + 0.110f * fast
                          + 0.110f * veryFast
                          + (sameString ? 0.080f * fast : 0.0f)
                          + 0.075f * static_cast<float> (juce::jmin (stringDistance, 3)) / 3.0f
                          + 0.085f * stringSkip
                          + 0.035f * fretJump
                          + (directionChange ? 0.055f : 0.0f);

    if (economyFlow)
    {
        cognitiveImpulse *= 0.72f;
        dexterityImpulse *= 0.82f;
    }

    if (strumContinuation)
    {
        cognitiveImpulse *= 0.30f;
        dexterityImpulse *= 0.42f;
    }

    playerFeelCognitiveLoad = juce::jlimit (0.0f, 1.0f, playerFeelCognitiveLoad + cognitiveImpulse);
    playerFeelDexterityLoad = juce::jlimit (0.0f, 1.0f, playerFeelDexterityLoad + dexterityImpulse);
    playerFeelEndurance = juce::jlimit (0.0f,
                                        1.0f,
                                        playerFeelEndurance
                                            + 0.040f * cognitiveImpulse
                                            + 0.075f * dexterityImpulse
                                            + 0.010f * fast);

    ++playerFeelEventCounter;
    const auto noiseA = getPlayerFeelNoise (0x4f1bbcddu, eventSample);
    const auto noiseB = getPlayerFeelNoise (0x72a3d113u, eventSample);
    const auto noiseC = getPlayerFeelNoise (0x1b873593u, eventSample);
    const auto load = juce::jlimit (0.0f,
                                    1.0f,
                                    0.34f * playerFeelCognitiveLoad
                                        + 0.46f * playerFeelDexterityLoad
                                        + 0.20f * playerFeelEndurance);

    if (amount > 0.0001f)
    {
        const auto overshoot = juce::jlimit (0.0f, 1.0f, (amount - 0.5f) * 2.0f);
        const auto overshootCurve = overshoot * overshoot;
        const auto timingScale = amount * 2.0f + 2.7f * overshootCurve;
        const auto energyScale = amount * 2.0f + 1.5f * overshootCurve;
        const auto positiveTimingScatter = juce::jmax (0.0f, noiseA)
                                         + overshootCurve * std::abs (noiseC);
        const auto delayMs = timingScale
                           * juce::jlimit (0.0f,
                                           16.0f,
                                           0.35f
                                               + 8.50f * load
                                               + 4.00f * playerFeelEndurance
                                               + 2.40f * positiveTimingScatter);
        result.delaySamples = juce::jlimit (0,
                                            static_cast<int> (std::round (sampleRate * 0.075)),
                                            static_cast<int> (std::round (static_cast<double> (delayMs)
                                                                          * sampleRate
                                                                          / 1000.0)));

        const auto velocityScale = juce::jlimit (0.42f,
                                                1.18f,
                                                1.0f
                                                    + energyScale
                                                        * (0.030f * noiseB
                                                           + 0.025f * noiseC
                                                           - 0.060f * playerFeelCognitiveLoad
                                                           - 0.085f * playerFeelDexterityLoad
                                                           - 0.065f * playerFeelEndurance
                                                           - 0.045f * overshootCurve * load));
        const auto adjustedVelocity = juce::jlimit (1,
                                                    127,
                                                    juce::roundToInt (message.getFloatVelocity()
                                                                      * velocityScale
                                                                      * 127.0f));
        result.message = juce::MidiMessage::noteOn (channel,
                                                    noteNumber,
                                                    static_cast<juce::uint8> (adjustedVelocity));
    }

    playerFeelLastStringIndex = assignment.stringIndex;
    playerFeelLastFret = assignment.fret;
    playerFeelLastNoteNumber = noteNumber;
    playerFeelLastTravelSign = travelSign != 0 ? travelSign : playerFeelLastTravelSign;
    playerFeelLastEventSample = eventSample + result.delaySamples;

    return result;
}

void AudioEngine::decayPlayerFeelLoads (int64_t sampleTime) noexcept
{
    if (playerFeelLastLoadDecaySample < 0)
    {
        playerFeelLastLoadDecaySample = sampleTime;
        return;
    }

    const auto elapsedSeconds = juce::jmax (0.0f,
                                           static_cast<float> (sampleTime - playerFeelLastLoadDecaySample)
                                               / static_cast<float> (sampleRate));
    const auto recoverySeconds = juce::jlimit (0.10f, 8.0f, playerFeelRecoverySeconds.getTargetValue());
    const auto cognitiveDecay = std::exp (-elapsedSeconds / juce::jmax (0.05f, recoverySeconds * 0.85f));
    const auto dexterityDecay = std::exp (-elapsedSeconds / juce::jmax (0.05f, recoverySeconds * 0.62f));
    const auto enduranceDecay = std::exp (-elapsedSeconds / juce::jmax (0.05f, recoverySeconds * 2.40f));

    playerFeelCognitiveLoad *= cognitiveDecay;
    playerFeelDexterityLoad *= dexterityDecay;
    playerFeelEndurance *= enduranceDecay;
    playerFeelLastLoadDecaySample = sampleTime;
}

void AudioEngine::releasePlayerFeelNote (int noteNumber, int channel) noexcept
{
    playerFeelFretboard.releaseNote (noteNumber, channel);
}

float AudioEngine::getPlayerFeelNoise (uint32_t salt, int64_t sampleTime) const noexcept
{
    auto seed = salt;
    seed ^= static_cast<uint32_t> (sampleTime);
    seed ^= static_cast<uint32_t> (sampleTime >> 32u) * 0x9e3779b9u;
    seed ^= playerFeelEventCounter * 0x85ebca6bu;
    seed ^= static_cast<uint32_t> (playerFeelLastNoteNumber + 128) * 0x27d4eb2du;
    seed ^= static_cast<uint32_t> (playerFeelLastStringIndex + 16) * 0x3449a1u;
    seed = mixPickAttackSeed (seed);
    return (static_cast<float> (seed & 0x00ffffffu) / 8388607.5f) - 1.0f;
}

int AudioEngine::getDirectionSign (int value) noexcept
{
    if (value > 0)
        return 1;

    if (value < 0)
        return -1;

    return 0;
}

void AudioEngine::handleMidiMessage (const juce::MidiMessage& message)
{
    if (message.isNoteOn())
    {
        noteOn (message.getNoteNumber(), message.getChannel(), message.getFloatVelocity());
        return;
    }

    if (message.isNoteOff())
    {
        noteOff (message.getNoteNumber(), message.getChannel());
        return;
    }

    if (message.isAftertouch())
    {
        applyAftertouch (message.getNoteNumber(),
                         message.getChannel(),
                         static_cast<float> (message.getAfterTouchValue()) / 127.0f);
        return;
    }

    if (message.isPitchWheel())
    {
        constexpr auto pitchWheelCenter = 8192.0f;
        const auto bend = (static_cast<float> (message.getPitchWheelValue()) - pitchWheelCenter)
                        / pitchWheelCenter;
        const auto clampedBend = juce::jlimit (-1.0f, 1.0f, bend);

        if (mpeEnabled && message.getChannel() != lowerMpeMasterChannel)
            applyMpePitchBend (message.getChannel(), clampedBend);
        else
            pitchWheel.setTargetValue (clampedBend);

        return;
    }

    if (message.isController() && message.getControllerNumber() == 1)
    {
        modWheel.setTargetValue (static_cast<float> (message.getControllerValue()) / 127.0f);
        return;
    }

    if (message.isController())
    {
        const auto controllerNumber = message.getControllerNumber();

        if (controllerNumber == 120 || controllerNumber >= 123)
        {
            releaseAllNotes();
            return;
        }

        if (controllerNumber == 121)
        {
            resetMidiControllers();
            return;
        }
    }

    if (message.isChannelPressure())
    {
        applyMpePressure (message.getChannel(), static_cast<float> (message.getChannelPressureValue()) / 127.0f);
        return;
    }

    if (message.isController() && message.getControllerNumber() == 74)
        applyMpeTimbre (message.getChannel(), static_cast<float> (message.getControllerValue()) / 127.0f);
}

void AudioEngine::releaseAllNotes() noexcept
{
    for (auto& voice : voices)
        voice.reset();

    fretboard.reset();
    fingerNoiseFretboard.reset();
    playerFeelFretboard.reset();
    clearScheduledMidiEvents();

    for (auto& assignment : fingerAssignments)
        assignment = {};

    for (auto& note : articulationNotes)
        note = {};

    for (auto& assignment : pendingStrumAssignments)
        assignment = {};

    for (auto& voice : fingerNoiseVoices)
        voice = {};

    resetMidiControllers();
    resetAmpFeedbackLoop();
    resetPlayerFeel();
    nextVoice = 0;
    nextFingerNoiseVoice = 0;
    lastPickedStringIndex = -1;
    nextAlternatePickDown = true;
    lastPickStrokeDirection = PickStrokeDirection::Up;

    if (std::abs (neckSlideSemitones.getTargetValue()) <= 0.0001f)
        neckSlideSemitones.setCurrentAndTargetValue (0.0f);
}

void AudioEngine::resetMidiControllers() noexcept
{
    modWheel.setCurrentAndTargetValue (0.0f);
    pitchWheel.setCurrentAndTargetValue (0.0f);
    mpePitchBendByChannel.fill (0.0f);
    mpePressureByChannel.fill (0.0f);
    mpeTimbreByChannel.fill (0.0f);
}

void AudioEngine::reconcileFretboardOccupancy() noexcept
{
    for (auto stringIndex = 0; stringIndex < maxVoices; ++stringIndex)
    {
        auto hasActiveVoiceOnString = false;

        for (const auto& voice : voices)
        {
            if (voice.isActive() && voice.getStringIndex() == stringIndex)
            {
                hasActiveVoiceOnString = true;
                break;
            }
        }

        if (! hasActiveVoiceOnString)
            fretboard.releaseString (stringIndex);
    }
}

void AudioEngine::scheduleMidiMessage (const juce::MidiMessage& message, int64_t sampleTime) noexcept
{
    for (auto& event : scheduledMidiEvents)
    {
        if (! event.active)
        {
            event.sampleTime = sampleTime;
            event.message = message;
            event.active = true;
            return;
        }
    }

    handleMidiMessage (message);
}

void AudioEngine::dispatchScheduledMidiEvents() noexcept
{
    for (auto& event : scheduledMidiEvents)
    {
        if (event.active && event.sampleTime <= timelineSample)
        {
            const auto message = event.message;
            event.active = false;
            handleMidiMessage (message);
        }
    }
}

void AudioEngine::clearScheduledMidiEvents() noexcept
{
    for (auto& event : scheduledMidiEvents)
        event.active = false;

    for (auto& assignment : pendingStrumAssignments)
        assignment.active = false;
}

void AudioEngine::rememberPendingStrumAssignment (int noteNumber,
                                                  int channel,
                                                  int64_t sampleTime,
                                                  int stringIndex) noexcept
{
    for (auto& assignment : pendingStrumAssignments)
    {
        if (! assignment.active)
        {
            assignment.sampleTime = sampleTime;
            assignment.noteNumber = noteNumber;
            assignment.channel = channel;
            assignment.stringIndex = stringIndex;
            assignment.active = true;
            return;
        }
    }
}

int AudioEngine::consumePendingStrumString (int noteNumber, int channel) noexcept
{
    auto bestIndex = -1;
    auto bestDistance = std::numeric_limits<int64_t>::max();

    for (auto index = 0; index < static_cast<int> (pendingStrumAssignments.size()); ++index)
    {
        const auto& assignment = pendingStrumAssignments[static_cast<size_t> (index)];

        if (! assignment.active || assignment.noteNumber != noteNumber || assignment.channel != channel)
            continue;

        const auto distance = assignment.sampleTime > timelineSample
                            ? assignment.sampleTime - timelineSample
                            : timelineSample - assignment.sampleTime;

        if (distance < bestDistance)
        {
            bestDistance = distance;
            bestIndex = index;
        }
    }

    if (bestIndex < 0)
        return -1;

    auto& assignment = pendingStrumAssignments[static_cast<size_t> (bestIndex)];
    const auto stringIndex = assignment.stringIndex;
    assignment.active = false;
    return stringIndex;
}

void AudioEngine::noteOn (int noteNumber, int channel, float velocity)
{
    const auto strumPreferredString = consumePendingStrumString (noteNumber, channel);
    const auto articulationAmount = legatoArticulation.getTargetValue();
    auto legatoSource = strumPreferredString >= 0
                      ? LegatoSource {}
                      : findLegatoSource (noteNumber, channel, articulationAmount);

    if (legatoSource.valid)
    {
        releaseLegatoSource (legatoSource);
        fretboard.releaseNote (legatoSource.noteNumber, legatoSource.channel);
        releaseArticulationNote (legatoSource.noteNumber, legatoSource.channel);
    }

    const auto preferredString = legatoSource.valid ? legatoSource.assignment.stringIndex : strumPreferredString;
    const auto preferredStringBonus = preferredString >= 0 ? 100.0f : 0.0f;
    const auto assignment = fretboard.assignNote (noteNumber,
                                                  channel,
                                                  preferredString,
                                                  preferredStringBonus,
                                                  legatoSource.valid);
    const auto gesture = legatoSource.valid && assignment.stringIndex == preferredString
                       ? legatoSource.gesture
                       : PlayerGesture::Picked;
    triggerFeedbackBloomDuck (velocity, gesture);

    const auto notePickStiffness = pickStiffness.getTargetValue();
    const auto notePickTexture = pickTexture.getTargetValue();
    const auto notePickBite = pickBite.getTargetValue();
    const auto noteHarmonicTouch = harmonicTouch.getTargetValue();
    const auto noteStringAge = stringAge.getTargetValue();
    const auto noteBridgeIntonation = bridgeIntonation.getTargetValue();
    const auto noteFretPressure = fretPressure.getTargetValue();
    const auto notePickupPosition = pickupPosition.getTargetValue();
    const auto notePickupModel = pickupModel;
    const auto pickStrokeDirection = resolvePickStrokeDirection (gesture, assignment);
    const auto pickAttackSeed = makePickAttackSeed (noteNumber, channel, assignment, gesture, pickStrokeDirection);

    auto& voice = selectVoiceForAssignment (assignment);

    if (voice.isActive())
    {
        fretboard.releaseNote (voice.getNoteNumber(), voice.getChannel());
        releaseArticulationNote (voice.getNoteNumber(), voice.getChannel());
    }

    voice.start (noteNumber,
                 channel,
                 velocity,
                 assignment,
                 notePickStiffness,
                 notePickTexture,
                 notePickBite,
                 noteHarmonicTouch,
                 noteStringAge,
                 noteBridgeIntonation,
                 noteFretPressure,
                 notePickupPosition,
                 notePickupModel,
                 gesture,
                 pickStrokeDirection,
                 pickAttackSeed);
    const auto channelIndex = static_cast<size_t> (juce::jlimit (1, 16, channel) - 1);
    voice.setMpePitchBend (channel, mpePitchBendByChannel[channelIndex]);
    voice.setMpePressure (channel, mpePressureByChannel[channelIndex]);
    voice.setMpeTimbre (channel, mpeTimbreByChannel[channelIndex]);
    rememberArticulationNote (noteNumber, channel, assignment, gesture);
}

PickStrokeDirection AudioEngine::resolvePickStrokeDirection (PlayerGesture gesture,
                                                             const FretboardAssignment& assignment) noexcept
{
    if (gesture != PlayerGesture::Picked)
        return PickStrokeDirection::Down;

    if (pickStrokeMode == PickStrokeMode::Down)
    {
        lastPickedStringIndex = assignment.stringIndex;
        lastPickStrokeDirection = PickStrokeDirection::Down;
        return PickStrokeDirection::Down;
    }

    if (pickStrokeMode == PickStrokeMode::Up)
    {
        lastPickedStringIndex = assignment.stringIndex;
        lastPickStrokeDirection = PickStrokeDirection::Up;
        return PickStrokeDirection::Up;
    }

    auto strokeDirection = nextAlternatePickDown ? PickStrokeDirection::Down : PickStrokeDirection::Up;

    if (lastPickedStringIndex >= 0)
    {
        if (assignment.stringIndex > lastPickedStringIndex)
            strokeDirection = PickStrokeDirection::Down;
        else if (assignment.stringIndex < lastPickedStringIndex)
            strokeDirection = PickStrokeDirection::Up;
        else
            strokeDirection = lastPickStrokeDirection == PickStrokeDirection::Down
                            ? PickStrokeDirection::Up
                            : PickStrokeDirection::Down;
    }

    lastPickedStringIndex = assignment.stringIndex;
    lastPickStrokeDirection = strokeDirection;
    nextAlternatePickDown = strokeDirection == PickStrokeDirection::Up;
    return strokeDirection;
}

uint32_t AudioEngine::makePickAttackSeed (int noteNumber,
                                          int channel,
                                          const FretboardAssignment& assignment,
                                          PlayerGesture gesture,
                                          PickStrokeDirection strokeDirection) noexcept
{
    auto seed = 0x6d2b79f5u;
    const auto combine = [&seed] (uint32_t component) noexcept
    {
        seed ^= mixPickAttackSeed (component + 0x9e3779b9u + (seed << 6u) + (seed >> 2u));
    };

    const auto sampleTime = static_cast<uint64_t> (timelineSample);
    combine (static_cast<uint32_t> (noteNumber + 128) * 0x45d9f3bu);
    combine (static_cast<uint32_t> (channel + 32) * 0x119de1f3u);
    combine (static_cast<uint32_t> (assignment.stringIndex + 16) * 0x3449a1u);
    combine (static_cast<uint32_t> (assignment.fret + 64) * 0x27d4eb2du);
    combine (static_cast<uint32_t> (sampleTime));
    combine (static_cast<uint32_t> (sampleTime >> 32u));
    combine (++pickAttackCounter * 0x85ebca6bu);
    combine (static_cast<uint32_t> (gesture) * 0xc2b2ae35u);
    combine (static_cast<uint32_t> (strokeDirection) * 0x27d4eb2fu);

    seed = mixPickAttackSeed (seed);
    return seed == 0u ? 0x12345678u : seed;
}

uint32_t AudioEngine::mixPickAttackSeed (uint32_t value) noexcept
{
    value ^= value >> 16u;
    value *= 0x7feb352du;
    value ^= value >> 15u;
    value *= 0x846ca68bu;
    value ^= value >> 16u;
    return value;
}

StringVoice& AudioEngine::selectVoiceForAssignment (const FretboardAssignment& assignment) noexcept
{
    const auto stringIndex = juce::jlimit (0, 5, assignment.stringIndex);

    for (auto& voice : voices)
    {
        if (voice.isActive() && voice.getStringIndex() == stringIndex)
            return voice;
    }

    for (auto& voice : voices)
    {
        if (! voice.isActive())
            return voice;
    }

    auto& stolenVoice = voices[static_cast<size_t> (nextVoice)];
    nextVoice = (nextVoice + 1) % maxVoices;
    return stolenVoice;
}

AudioEngine::LegatoSource AudioEngine::findLegatoSource (int noteNumber, int channel, float amount) const noexcept
{
    LegatoSource best {};

    if (amount <= 0.20f)
        return best;

    constexpr auto minimumSourceAgeSeconds = 0.018f;
    auto bestScore = 1000000.0f;
    const auto style = juce::jlimit (0.0f, 1.0f, (amount - 0.20f) / 0.80f);

    for (const auto& note : articulationNotes)
    {
        if (! note.valid || note.noteNumber == noteNumber)
            continue;

        // Same-instant block-chord tones should remain independent voices, not
        // become hammer-ons from notes that only just started.
        const auto sourceAgeSamples = timelineSample - note.startSample;

        if (sourceAgeSamples < 0)
            continue;

        const auto sourceAgeSeconds = static_cast<float> (sourceAgeSamples) / static_cast<float> (sampleRate);

        if (sourceAgeSeconds < minimumSourceAgeSeconds)
            continue;

        const auto destinationFret = fretboard.getFretForString (noteNumber, note.assignment.stringIndex);

        if (destinationFret < 0 || destinationFret == note.assignment.fret)
            continue;

        const auto referenceSample = note.active ? note.startSample : note.releaseSample;
        const auto ageSamples = timelineSample - referenceSample;

        if (ageSamples < 0)
            continue;

        const auto ageSeconds = static_cast<float> (ageSamples) / static_cast<float> (sampleRate);
        const auto maxAgeSeconds = note.active ? 0.70f : 0.18f + 0.12f * style;

        if (ageSeconds > maxAgeSeconds)
            continue;

        const auto fretDelta = destinationFret - note.assignment.fret;
        const auto fretDistance = std::abs (fretDelta);
        auto gesture = PlayerGesture::Picked;
        auto probability = 0.0f;

        if (fretDelta < 0)
        {
            const auto maxDistance = amount < 0.50f ? 5 : amount < 0.70f ? 8 : 12;

            if (fretDistance > maxDistance && destinationFret != 0)
                continue;

            gesture = PlayerGesture::PullOff;
            const auto rangeProbability = amount < 0.30f
                                        ? juce::jmap ((amount - 0.20f) / 0.10f, 0.12f, 0.36f)
                                        : juce::jmap (style, 0.38f, 0.92f);
            const auto distanceConfidence = juce::jlimit (0.25f, 1.0f, 1.16f - 0.075f * static_cast<float> (fretDistance));
            const auto openBonus = destinationFret == 0 ? 1.16f : 1.0f;
            probability = rangeProbability * distanceConfidence * openBonus;
        }
        else if (amount >= 0.70f
                 && fretDistance >= 5
                 && destinationFret >= 7
                 && note.assignment.fret <= 14)
        {
            gesture = PlayerGesture::RightHandTap;
            const auto tapAmount = juce::jlimit (0.0f, 1.0f, (amount - 0.70f) / 0.30f);
            const auto distanceConfidence = juce::jlimit (0.35f, 1.0f, 0.50f + 0.055f * static_cast<float> (fretDistance));
            const auto fretConfidence = juce::jlimit (0.45f, 1.0f, static_cast<float> (destinationFret - 5) / 11.0f);
            probability = juce::jmap (tapAmount, 0.24f, 0.94f) * distanceConfidence * fretConfidence;
        }
        else if (amount >= 0.30f)
        {
            const auto maxDistance = amount < 0.50f ? 4 : amount < 0.70f ? 7 : 10;

            if (fretDistance > maxDistance)
                continue;

            gesture = PlayerGesture::HammerOn;
            const auto hammerAmount = juce::jlimit (0.0f, 1.0f, (amount - 0.30f) / 0.55f);
            const auto distanceConfidence = juce::jlimit (0.25f, 1.0f, 1.10f - 0.080f * static_cast<float> (fretDistance));
            probability = juce::jmap (hammerAmount, 0.18f, 0.86f) * distanceConfidence;
        }

        if (gesture == PlayerGesture::Picked)
            continue;

        const auto activeBonus = note.active ? 0.18f : 0.0f;
        const auto channelBonus = note.channel == channel ? 0.05f : 0.0f;
        const auto phraseRate = juce::jlimit (0.55f, 1.18f, 1.12f - ageSeconds * 2.2f);
        probability = juce::jlimit (0.0f, 0.98f, probability * phraseRate + activeBonus + channelBonus);

        if (getDeterministicGestureChance (noteNumber, channel, note.noteNumber, timelineSample) > probability)
            continue;

        const auto gesturePriority = gesture == PlayerGesture::RightHandTap ? 0.16f
                                 : gesture == PlayerGesture::PullOff ? 0.08f
                                                                      : 0.0f;
        const auto score = ageSeconds * 3.0f
                         + static_cast<float> (fretDistance) * 0.10f
                         - (note.active ? 0.22f : 0.0f)
                         - channelBonus
                         - gesturePriority;

        if (score < bestScore)
        {
            bestScore = score;
            best.noteNumber = note.noteNumber;
            best.channel = note.channel;
            best.assignment = note.assignment;
            best.gesture = gesture;
            best.destinationFret = destinationFret;
            best.startSample = note.startSample;
            best.releaseSample = note.releaseSample;
            best.active = note.active;
            best.valid = true;
        }
    }

    return best;
}

void AudioEngine::rememberArticulationNote (int noteNumber,
                                            int channel,
                                            const FretboardAssignment& assignment,
                                            PlayerGesture gesture) noexcept
{
    auto* slot = &articulationNotes.front();
    auto oldestSample = timelineSample + 1;

    for (auto& note : articulationNotes)
    {
        if (! note.valid || (note.noteNumber == noteNumber && note.channel == channel))
        {
            slot = &note;
            break;
        }

        const auto ageReference = note.active ? note.startSample : note.releaseSample;

        if (ageReference < oldestSample)
        {
            oldestSample = ageReference;
            slot = &note;
        }
    }

    *slot = { noteNumber, channel, assignment, gesture, timelineSample, timelineSample, true, true };
}

void AudioEngine::releaseArticulationNote (int noteNumber, int channel) noexcept
{
    for (auto& note : articulationNotes)
    {
        if (note.valid && note.active && note.noteNumber == noteNumber && note.channel == channel)
        {
            note.active = false;
            note.releaseSample = timelineSample;
        }
    }
}

void AudioEngine::releaseLegatoSource (const LegatoSource& source) noexcept
{
    if (! source.valid)
        return;

    for (auto& voice : voices)
        voice.release (source.noteNumber, source.channel);
}

float AudioEngine::getDeterministicGestureChance (int noteNumber,
                                                  int channel,
                                                  int sourceNoteNumber,
                                                  int64_t sampleTime) noexcept
{
    auto hash = static_cast<uint32_t> ((noteNumber + 101) * 1103515245u)
              ^ static_cast<uint32_t> ((sourceNoteNumber + 31) * 2654435761u)
              ^ static_cast<uint32_t> ((channel + 17) * 2246822519u)
              ^ static_cast<uint32_t> ((sampleTime / 128) * 3266489917ull);
    hash ^= hash >> 16;
    hash *= 0x7feb352du;
    hash ^= hash >> 15;
    hash *= 0x846ca68bu;
    hash ^= hash >> 16;

    return static_cast<float> ((hash >> 8) & 0x00ffffffu) / static_cast<float> (0x00ffffffu);
}

void AudioEngine::triggerFingerApproach (int noteNumber, int channel, float velocity) noexcept
{
    const auto assignment = fingerNoiseFretboard.assignNote (noteNumber, channel);
    rememberFingerAssignment (noteNumber, channel, assignment);

    const auto velocityScale = 0.35f + 0.65f * juce::jlimit (0.0f, 1.0f, velocity);
    const auto fretScale = 0.55f + 0.45f * juce::jlimit (0.0f, 1.0f, static_cast<float> (assignment.fret) / 12.0f);
    const auto stringScale = 1.0f + 0.28f * static_cast<float> (5 - juce::jlimit (0, 5, assignment.stringIndex)) / 5.0f;
    const auto openScale = assignment.fret <= 0 ? 0.42f : 1.0f;

    startFingerNoise (assignment, velocityScale * fretScale * stringScale * openScale, false);
}

void AudioEngine::triggerFingerRelease (int noteNumber, int channel) noexcept
{
    const auto assignment = findFingerAssignment (noteNumber, channel);
    const auto openScale = assignment.fret <= 0 ? 0.30f : 1.0f;
    startFingerNoise (assignment, openScale * (0.55f + 0.45f * assignment.woundAmount), true);
    releaseFingerAssignment (noteNumber, channel);
    fingerNoiseFretboard.releaseNote (noteNumber, channel);
}

void AudioEngine::rememberFingerAssignment (int noteNumber, int channel, const FretboardAssignment& assignment) noexcept
{
    for (auto& stored : fingerAssignments)
    {
        if (! stored.active || (stored.noteNumber == noteNumber && stored.channel == channel))
        {
            stored = { noteNumber, channel, assignment, true };
            return;
        }
    }

    fingerAssignments[0] = { noteNumber, channel, assignment, true };
}

FretboardAssignment AudioEngine::findFingerAssignment (int noteNumber, int channel) const noexcept
{
    for (const auto& stored : fingerAssignments)
    {
        if (stored.active && stored.noteNumber == noteNumber && stored.channel == channel)
            return stored.assignment;
    }

    return {};
}

void AudioEngine::releaseFingerAssignment (int noteNumber, int channel) noexcept
{
    for (auto& stored : fingerAssignments)
    {
        if (stored.active && stored.noteNumber == noteNumber && stored.channel == channel)
            stored = {};
    }
}

void AudioEngine::startFingerNoise (const FretboardAssignment& assignment, float intensity, bool releaseNoise) noexcept
{
    const auto amount = fingerNoise.getTargetValue();

    if (amount <= 0.0001f)
        return;

    auto& voice = fingerNoiseVoices[static_cast<size_t> (nextFingerNoiseVoice)];
    nextFingerNoiseVoice = (nextFingerNoiseVoice + 1) % maxFingerNoiseVoices;

    const auto clampedIntensity = juce::jlimit (0.0f, 2.0f, intensity);
    const auto woundAmount = juce::jlimit (0.0f, 1.0f, assignment.woundAmount);
    const auto stringPosition = static_cast<float> (juce::jlimit (0, 5, assignment.stringIndex)) / 5.0f;
    const auto durationSeconds = releaseNoise ? 0.034f + 0.052f * woundAmount
                                              : 0.046f + 0.080f * woundAmount;
    const auto decaySeconds = releaseNoise ? durationSeconds * 0.55f : durationSeconds * 0.72f;
    const auto stickDecaySeconds = 0.0035f + 0.0045f * woundAmount;
    constexpr auto twoPi = 6.28318530717958647692f;
    const auto baseFrequency = 720.0f
                             + 95.0f * static_cast<float> (assignment.fret)
                             + 260.0f * stringPosition
                             + 520.0f * woundAmount
                             - 180.0f * (1.0f - stringPosition) * woundAmount
                             + (releaseNoise ? 180.0f : 0.0f);

    voice.samplesRemaining = juce::jmax (1, static_cast<int> (sampleRate * durationSeconds));
    voice.amplitude = clampedIntensity * (releaseNoise ? 0.021f : 0.029f) * (0.76f + 0.36f * woundAmount);
    voice.decay = std::pow (0.001f, 1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate * decaySeconds)));
    voice.previousNoise = 0.0f;
    voice.bodyState = 0.0f;
    voice.scrapeState = 0.0f;
    voice.stickImpulse = (releaseNoise ? 0.10f : 0.18f) * (0.35f + 0.65f * woundAmount);
    voice.stickImpulseDecay = std::pow (0.001f,
                                        1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate * stickDecaySeconds)));
    voice.squeakAmount = (0.28f + 0.72f * woundAmount) * (releaseNoise ? 0.86f : 1.0f);
    voice.phase = 0.31f * static_cast<float> ((assignment.fret + 1) * (assignment.stringIndex + 3));
    voice.phaseStep = twoPi * juce::jlimit (180.0f,
                                            static_cast<float> (sampleRate * 0.38),
                                            baseFrequency)
                    / static_cast<float> (sampleRate);
    voice.woundAmount = woundAmount;
    voice.stickCountdown = 0;
    voice.randomState = static_cast<uint32_t> ((assignment.fret + 11) * 1103515245u
                                               + (assignment.stringIndex + 3) * 12345u
                                               + (releaseNoise ? 0x9e3779b9u : 0x85ebca6bu));
}

float AudioEngine::renderFingerNoiseSample() noexcept
{
    auto output = 0.0f;
    constexpr auto twoPi = 6.28318530717958647692f;

    for (auto& voice : fingerNoiseVoices)
    {
        if (voice.samplesRemaining <= 0)
            continue;

        const auto rawNoise = nextFingerNoiseRandom (voice.randomState);
        const auto frictionDelta = rawNoise - voice.previousNoise;
        voice.previousNoise += (0.12f + 0.08f * voice.woundAmount) * (rawNoise - voice.previousNoise);
        voice.scrapeState += (0.10f + 0.08f * voice.woundAmount) * (frictionDelta - voice.scrapeState);
        voice.bodyState += (0.020f + 0.014f * voice.woundAmount) * (voice.scrapeState - voice.bodyState);

        if (voice.stickCountdown <= 0)
        {
            const auto intervalNoise = 0.5f + 0.5f * nextFingerNoiseRandom (voice.randomState);
            const auto intervalSeconds = (0.00055f + 0.00230f * (1.0f - voice.woundAmount))
                                       * (0.70f + 0.80f * intervalNoise);
            voice.stickCountdown = juce::jmax (1, static_cast<int> (sampleRate * intervalSeconds));
            voice.stickImpulse = juce::jlimit (0.0f,
                                               1.85f,
                                               voice.stickImpulse
                                                   + voice.squeakAmount
                                                        * (0.20f + 0.80f * std::abs (nextFingerNoiseRandom (voice.randomState))));
        }

        voice.phase += voice.phaseStep * (1.0f + 0.018f * voice.scrapeState + 0.010f * rawNoise);

        if (voice.phase > twoPi)
            voice.phase -= twoPi;

        const auto ridge = std::sin (voice.phase)
                         + (0.18f + 0.28f * voice.woundAmount) * std::sin (voice.phase * (2.11f + 0.62f * voice.woundAmount))
                         + 0.12f * voice.woundAmount * std::sin (voice.phase * 3.73f);
        const auto squeak = voice.stickImpulse * ridge;
        const auto filteredScrape = voice.scrapeState - 0.34f * voice.bodyState;
        const auto scrape = squeak * (0.78f + 0.66f * voice.woundAmount)
                          + filteredScrape * (0.045f + 0.070f * voice.woundAmount)
                          + voice.bodyState * (0.090f + 0.120f * voice.woundAmount);

        output += scrape * voice.amplitude;
        voice.amplitude *= voice.decay;
        voice.stickImpulse *= voice.stickImpulseDecay;
        --voice.stickCountdown;
        --voice.samplesRemaining;
    }

    return output;
}

float AudioEngine::nextFingerNoiseRandom (uint32_t& state) noexcept
{
    state = state * 1664525u + 1013904223u;
    const auto value = static_cast<float> ((state >> 8) & 0x00ffffffu) / static_cast<float> (0x00ffffffu);
    return 2.0f * value - 1.0f;
}

void AudioEngine::noteOff (int noteNumber, int channel)
{
    fretboard.releaseNote (noteNumber, channel);
    releaseArticulationNote (noteNumber, channel);

    for (auto& voice : voices)
        voice.release (noteNumber, channel);
}

void AudioEngine::applyAftertouch (int noteNumber, int channel, float pressure) noexcept
{
    for (auto& voice : voices)
        voice.setAftertouchPressure (noteNumber, channel, pressure);
}

void AudioEngine::applyMpePitchBend (int channel, float bend) noexcept
{
    const auto clampedChannel = juce::jlimit (1, 16, channel);
    const auto clampedBend = juce::jlimit (-1.0f, 1.0f, bend);
    mpePitchBendByChannel[static_cast<size_t> (clampedChannel - 1)] = clampedBend;

    for (auto& voice : voices)
        voice.setMpePitchBend (clampedChannel, clampedBend);
}

void AudioEngine::applyMpePressure (int channel, float pressure) noexcept
{
    const auto clampedChannel = juce::jlimit (1, 16, channel);
    const auto clampedPressure = juce::jlimit (0.0f, 1.0f, pressure);

    if (! mpeEnabled)
    {
        mpePressureByChannel.fill (clampedPressure);

        for (auto& voice : voices)
            voice.setMpePressure (voice.getChannel(), clampedPressure);

        return;
    }

    mpePressureByChannel[static_cast<size_t> (clampedChannel - 1)] = clampedPressure;

    for (auto& voice : voices)
        voice.setMpePressure (clampedChannel, clampedPressure);
}

void AudioEngine::applyMpeTimbre (int channel, float timbre) noexcept
{
    const auto clampedChannel = juce::jlimit (1, 16, channel);
    const auto clampedTimbre = juce::jlimit (0.0f, 1.0f, timbre);

    if (! mpeEnabled)
    {
        mpeTimbreByChannel.fill (clampedTimbre);

        for (auto& voice : voices)
            voice.setMpeTimbre (voice.getChannel(), clampedTimbre);

        return;
    }

    mpeTimbreByChannel[static_cast<size_t> (clampedChannel - 1)] = clampedTimbre;

    for (auto& voice : voices)
        voice.setMpeTimbre (clampedChannel, clampedTimbre);
}

} // namespace guitar_ag
