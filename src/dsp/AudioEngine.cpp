#include "AudioEngine.h"

#include <cmath>

namespace guitar_ag
{

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
    pickTexture.setCurrentAndTargetValue (0.5f);
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
    tone.prepare (sampleRate);
    reset();
}

void AudioEngine::reset()
{
    for (auto& voice : voices)
        voice.reset();

    fretboard.reset();
    fingerNoiseFretboard.reset();
    tone.reset();
    clearScheduledMidiEvents();

    for (auto& assignment : fingerAssignments)
        assignment = {};

    for (auto& voice : fingerNoiseVoices)
        voice = {};

    tailSustain.setCurrentAndTargetValue (tailSustain.getTargetValue());
    pickStiffness.setCurrentAndTargetValue (pickStiffness.getTargetValue());
    pickTexture.setCurrentAndTargetValue (pickTexture.getTargetValue());
    palmMute.setCurrentAndTargetValue (palmMute.getTargetValue());
    harmonicTouch.setCurrentAndTargetValue (harmonicTouch.getTargetValue());
    stringAge.setCurrentAndTargetValue (stringAge.getTargetValue());
    bridgeIntonation.setCurrentAndTargetValue (bridgeIntonation.getTargetValue());
    fretPressure.setCurrentAndTargetValue (fretPressure.getTargetValue());
    fingerNoise.setCurrentAndTargetValue (fingerNoise.getTargetValue());
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
}

void AudioEngine::setTailSustain (float newTailSustain) noexcept
{
    tailSustain.setTargetValue (juce::jlimit (0.0f, 1.0f, newTailSustain));
}

void AudioEngine::setPickStiffness (float newPickStiffness) noexcept
{
    pickStiffness.setTargetValue (juce::jlimit (0.0f, 1.0f, newPickStiffness));
}

void AudioEngine::setPickTexture (float newPickTexture) noexcept
{
    pickTexture.setTargetValue (juce::jlimit (0.0f, 1.0f, newPickTexture));
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

    if (! mpeEnabled)
    {
        mpePitchBendByChannel.fill (0.0f);

        for (auto& voice : voices)
            voice.setMpePitchBend (voice.getChannel(), 0.0f);
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

void AudioEngine::render (juce::AudioBuffer<float>& audio, const juce::MidiBuffer& midi)
{
    auto currentSample = 0;
    const auto totalSamples = audio.getNumSamples();

    for (const auto metadata : midi)
    {
        const auto eventSample = juce::jlimit (0, totalSamples, metadata.samplePosition);
        renderRange (audio, currentSample, eventSample);
        handleIncomingMidiMessage (metadata.getMessage());
        currentSample = eventSample;
    }

    renderRange (audio, currentSample, totalSamples);
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
        const auto whammySemitones = whammyEnabled && ! mpeEnabled
                                   ? pitchWheelAmount * (pitchWheelAmount >= 0.0f ? whammyUpAmount : whammyDownAmount)
                                   : 0.0f;
        const auto whammySpreadAmount = whammySpread.getNextValue();
        const auto aftertouchBendAmount = aftertouchBendSemitones.getNextValue();
        pickStiffness.getNextValue();
        pickTexture.getNextValue();
        harmonicTouch.getNextValue();
        stringAge.getNextValue();
        bridgeIntonation.getNextValue();
        fretPressure.getNextValue();
        pickupPosition.getNextValue();

        dispatchScheduledMidiEvents();

        for (auto& voice : voices)
            mixedSample += voice.renderSample (sustainAmount,
                                               palmMuteAmount,
                                               vibratoDepthAmount,
                                               vibratoSpeedAmount,
                                               vibratoDelaySeconds,
                                               whammySemitones,
                                               whammySpreadAmount,
                                               aftertouchBendAmount,
                                               mpePressureAmountValue,
                                               mpeTimbreAmountValue,
                                               mpePitchBendRangeAmount);

        mixedSample += renderFingerNoiseSample() * fingerNoiseAmount;
        mixedSample = tone.processSample (mixedSample);

        for (auto channel = 0; channel < numChannels; ++channel)
            audio.addSample (channel, sampleIndex, mixedSample);

        ++timelineSample;
    }
}

void AudioEngine::handleIncomingMidiMessage (const juce::MidiMessage& message)
{
    if (message.isController() && message.getControllerNumber() == 1)
    {
        modWheel.setTargetValue (static_cast<float> (message.getControllerValue()) / 127.0f);
        return;
    }

    if (message.isController() && message.getControllerNumber() == 74)
    {
        applyMpeTimbre (message.getChannel(), static_cast<float> (message.getControllerValue()) / 127.0f);
        return;
    }

    if (message.isChannelPressure())
    {
        applyMpePressure (message.getChannel(), static_cast<float> (message.getChannelPressureValue()) / 127.0f);
        return;
    }

    if (message.isPitchWheel())
    {
        constexpr auto pitchWheelCenter = 8192.0f;
        const auto bend = (static_cast<float> (message.getPitchWheelValue()) - pitchWheelCenter)
                        / pitchWheelCenter;
        const auto clampedBend = juce::jlimit (-1.0f, 1.0f, bend);

        if (mpeEnabled)
            applyMpePitchBend (message.getChannel(), clampedBend);
        else
            pitchWheel.setTargetValue (clampedBend);

        return;
    }

    if (lookaheadSamples <= 0)
    {
        handleMidiMessage (message);
        return;
    }

    if (message.isNoteOn())
        triggerFingerApproach (message.getNoteNumber(), message.getChannel(), message.getFloatVelocity());
    else if (message.isNoteOff())
        triggerFingerRelease (message.getNoteNumber(), message.getChannel());

    if (message.isNoteOnOrOff() || message.isAftertouch())
        scheduleMidiMessage (message, timelineSample + static_cast<int64_t> (lookaheadSamples));
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

    if (message.isChannelPressure())
    {
        applyMpePressure (message.getChannel(), static_cast<float> (message.getChannelPressureValue()) / 127.0f);
        return;
    }

    if (message.isController() && message.getControllerNumber() == 74)
        applyMpeTimbre (message.getChannel(), static_cast<float> (message.getControllerValue()) / 127.0f);
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
}

void AudioEngine::noteOn (int noteNumber, int channel, float velocity)
{
    const auto assignment = fretboard.assignNote (noteNumber, channel);
    const auto notePickStiffness = pickStiffness.getTargetValue();
    const auto notePickTexture = pickTexture.getTargetValue();
    const auto noteHarmonicTouch = harmonicTouch.getTargetValue();
    const auto noteStringAge = stringAge.getTargetValue();
    const auto noteBridgeIntonation = bridgeIntonation.getTargetValue();
    const auto noteFretPressure = fretPressure.getTargetValue();
    const auto notePickupPosition = pickupPosition.getTargetValue();
    const auto notePickupModel = pickupModel;

    for (auto& voice : voices)
    {
        if (! voice.isActive())
        {
            voice.start (noteNumber,
                         channel,
                         velocity,
                         assignment,
                         notePickStiffness,
                         notePickTexture,
                         noteHarmonicTouch,
                         noteStringAge,
                         noteBridgeIntonation,
                         noteFretPressure,
                         notePickupPosition,
                         notePickupModel);
            const auto channelIndex = static_cast<size_t> (juce::jlimit (1, 16, channel) - 1);
            voice.setMpePitchBend (channel, mpePitchBendByChannel[channelIndex]);
            voice.setMpePressure (channel, mpePressureByChannel[channelIndex]);
            voice.setMpeTimbre (channel, mpeTimbreByChannel[channelIndex]);
            return;
        }
    }

    auto& stolenVoice = voices[static_cast<size_t> (nextVoice)];
    fretboard.releaseNote (stolenVoice.getNoteNumber(), stolenVoice.getChannel());
    stolenVoice.start (noteNumber,
                       channel,
                       velocity,
                       assignment,
                       notePickStiffness,
                       notePickTexture,
                       noteHarmonicTouch,
                       noteStringAge,
                       noteBridgeIntonation,
                       noteFretPressure,
                       notePickupPosition,
                       notePickupModel);
    const auto channelIndex = static_cast<size_t> (juce::jlimit (1, 16, channel) - 1);
    stolenVoice.setMpePitchBend (channel, mpePitchBendByChannel[channelIndex]);
    stolenVoice.setMpePressure (channel, mpePressureByChannel[channelIndex]);
    stolenVoice.setMpeTimbre (channel, mpeTimbreByChannel[channelIndex]);
    nextVoice = (nextVoice + 1) % maxVoices;
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
    const auto durationSeconds = releaseNoise ? 0.030f + 0.050f * woundAmount
                                              : 0.040f + 0.070f * woundAmount;
    const auto decaySeconds = releaseNoise ? durationSeconds * 0.55f : durationSeconds * 0.72f;
    constexpr auto twoPi = 6.28318530717958647692f;
    const auto baseFrequency = 900.0f
                             + 120.0f * static_cast<float> (assignment.fret)
                             + 520.0f * woundAmount
                             + (releaseNoise ? 260.0f : 0.0f);

    voice.samplesRemaining = juce::jmax (1, static_cast<int> (sampleRate * durationSeconds));
    voice.amplitude = clampedIntensity * (releaseNoise ? 0.024f : 0.032f);
    voice.decay = std::pow (0.001f, 1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate * decaySeconds)));
    voice.previousNoise = 0.0f;
    voice.bodyState = 0.0f;
    voice.phase = 0.31f * static_cast<float> ((assignment.fret + 1) * (assignment.stringIndex + 3));
    voice.phaseStep = twoPi * juce::jlimit (180.0f,
                                            static_cast<float> (sampleRate * 0.38),
                                            baseFrequency)
                    / static_cast<float> (sampleRate);
    voice.woundAmount = woundAmount;
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
        const auto scratch = rawNoise - voice.previousNoise * 0.74f;
        voice.previousNoise = rawNoise;
        voice.bodyState += (0.050f + 0.035f * voice.woundAmount) * (scratch - voice.bodyState);
        voice.phase += voice.phaseStep * (1.0f + 0.025f * rawNoise);

        if (voice.phase > twoPi)
            voice.phase -= twoPi;

        const auto ridge = std::sin (voice.phase)
                         + 0.28f * std::sin (voice.phase * (2.0f + 1.4f * voice.woundAmount));
        const auto bright = scratch - voice.bodyState * 0.35f;
        const auto scrape = bright * (0.38f + 0.20f * voice.woundAmount)
                          + ridge * (0.22f + 0.52f * voice.woundAmount)
                          + voice.bodyState * 0.42f;

        output += scrape * voice.amplitude;
        voice.amplitude *= voice.decay;
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
    mpePressureByChannel[static_cast<size_t> (clampedChannel - 1)] = clampedPressure;

    for (auto& voice : voices)
        voice.setMpePressure (clampedChannel, clampedPressure);
}

void AudioEngine::applyMpeTimbre (int channel, float timbre) noexcept
{
    const auto clampedChannel = juce::jlimit (1, 16, channel);
    const auto clampedTimbre = juce::jlimit (0.0f, 1.0f, timbre);
    mpeTimbreByChannel[static_cast<size_t> (clampedChannel - 1)] = clampedTimbre;

    for (auto& voice : voices)
        voice.setMpeTimbre (clampedChannel, clampedTimbre);
}

} // namespace guitar_ag
