#include "StringVoice.h"

#include <cmath>

namespace guitar_ag
{

void StringVoice::prepare (double newSampleRate)
{
    sampleRate = newSampleRate > 0.0 ? newSampleRate : 44100.0;
    reset();
}

void StringVoice::reset()
{
    delayLine.fill (0.0f);

    delayLength = 1;
    writeIndex = 0;
    pickupOffsetSamples = 1;
    samplesSinceStart = 0;
    noteNumber = -1;
    channel = 0;
    damping = baseDamping;
    lastOutput = 0.0f;
    previousPickupSample = 0.0f;
    energy = 0.0f;
    pickTransient = 0.0f;
    pickTransientDecay = 0.0f;
    leftHandDamping = 1.0f;
    leftHandDampingTarget = 1.0f;
    leftHandDampingStep = 0.0f;
    active = false;
    releasing = false;
}

void StringVoice::start (int midiNoteNumber, int midiChannel, float velocity)
{
    noteNumber = midiNoteNumber;
    channel = midiChannel;
    writeIndex = 0;
    samplesSinceStart = 0;
    lastOutput = 0.0f;
    previousPickupSample = 0.0f;
    pickTransient = 0.0f;
    pickTransientDecay = 0.0f;
    leftHandDamping = 1.0f;
    leftHandDampingTarget = 1.0f;
    leftHandDampingStep = 0.0f;
    releasing = false;
    active = true;

    const auto frequency = juce::jlimit (20.0, 8000.0, juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));
    delayLength = juce::jlimit (2, maxDelaySamples, static_cast<int> (std::round (sampleRate / frequency)));
    pickupOffsetSamples = juce::jlimit (1, delayLength - 1, static_cast<int> (std::round (delayLength * 0.18f)));

    const auto velocityGain = juce::jlimit (0.05f, 1.0f, velocity);
    const auto brightness = juce::jlimit (0.0f, 1.0f, velocityGain);
    const auto pluckPosition = 0.20f;
    const auto displacementAmount = 0.75f * velocityGain;
    const auto noiseAmount = 0.025f + 0.09f * brightness;
    auto mean = 0.0f;

    randomState = static_cast<uint32_t> ((midiNoteNumber + 1) * 1103515245u + (midiChannel + 17) * 12345u);

    for (auto i = 0; i < delayLength; ++i)
    {
        const auto x = static_cast<float> (i) / static_cast<float> (delayLength);
        const auto shape = pluckShapeAt (x, pluckPosition);
        const auto pickDistance = std::abs (x - pluckPosition);
        const auto localPickContact = std::exp (-pickDistance * static_cast<float> (delayLength) * 0.18f);
        const auto scrapeNoise = nextNoiseSample() * localPickContact * noiseAmount;
        const auto sample = shape * displacementAmount + scrapeNoise;

        delayLine[static_cast<size_t> (i)] = sample;
        mean += sample;
    }

    mean /= static_cast<float> (delayLength);

    for (auto i = 0; i < delayLength; ++i)
        delayLine[static_cast<size_t> (i)] -= mean;

    for (auto i = delayLength; i < maxDelaySamples; ++i)
        delayLine[static_cast<size_t> (i)] = 0.0f;

    energy = velocityGain;
    pickTransient = (0.02f + 0.08f * brightness) * (nextNoiseSample() >= 0.0f ? 1.0f : -1.0f);
    pickTransientDecay = 0.998f - 0.003f * brightness;
    updateDamping();
}

void StringVoice::release (int midiNoteNumber, int midiChannel)
{
    if (active && noteNumber == midiNoteNumber && channel == midiChannel)
    {
        releasing = true;
        startLeftHandRelease();
        updateDamping();
    }
}

float StringVoice::renderSample() noexcept
{
    if (! active)
        return 0.0f;

    const auto index = static_cast<size_t> (writeIndex);
    auto current = delayLine[index];

    if (std::abs (pickTransient) > 0.000001f)
    {
        current += pickTransient;
        pickTransient *= pickTransientDecay;
    }

    if (releasing && leftHandDamping > leftHandDampingTarget)
        leftHandDamping = juce::jmax (leftHandDampingTarget, leftHandDamping - leftHandDampingStep);

    const auto filtered = 0.5f * (current + lastOutput) * damping * leftHandDamping;

    delayLine[index] = filtered;
    lastOutput = current;

    ++writeIndex;

    if (writeIndex >= delayLength)
        writeIndex = 0;

    ++samplesSinceStart;

    energy = 0.9995f * energy + 0.0005f * std::abs (current);

    if (energy < 0.00008f)
    {
        reset();
        return 0.0f;
    }

    const auto pickupSample = readDelayLineAtOffset (pickupOffsetSamples);
    const auto pickupVelocity = pickupSample - previousPickupSample;
    previousPickupSample = pickupSample;

    const auto pickupReadout = 0.82f * pickupSample + 0.55f * pickupVelocity;
    return pickupReadout * outputGain;
}

float StringVoice::nextNoiseSample() noexcept
{
    randomState = randomState * 1664525u + 1013904223u;
    const auto value = static_cast<float> ((randomState >> 8) & 0x00ffffffu) / static_cast<float> (0x00ffffffu);
    return 2.0f * value - 1.0f;
}

void StringVoice::updateDamping() noexcept
{
    baseDamping = 0.9965f;
    releaseDamping = 0.992f;
    damping = releasing ? releaseDamping : baseDamping;
}

void StringVoice::startLeftHandRelease() noexcept
{
    const auto heldSeconds = static_cast<float> (samplesSinceStart) / static_cast<float> (sampleRate);

    if (heldSeconds < 0.12f)
    {
        leftHandDampingTarget = 0.55f;
        const auto transitionSamples = juce::jmax (1.0f, static_cast<float> (sampleRate) * 0.008f);
        leftHandDampingStep = (leftHandDamping - leftHandDampingTarget) / transitionSamples;
        energy *= 0.35f;
        return;
    }

    if (heldSeconds < 0.45f)
    {
        const auto blend = (heldSeconds - 0.12f) / 0.33f;
        leftHandDampingTarget = juce::jmap (blend, 0.55f, 0.82f);
        const auto transitionSeconds = juce::jmap (blend, 0.012f, 0.035f);
        const auto transitionSamples = juce::jmax (1.0f, static_cast<float> (sampleRate) * transitionSeconds);
        leftHandDampingStep = (leftHandDamping - leftHandDampingTarget) / transitionSamples;
        energy *= juce::jmap (blend, 0.45f, 0.75f);
        return;
    }

    leftHandDampingTarget = 0.90f;
    const auto transitionSamples = juce::jmax (1.0f, static_cast<float> (sampleRate) * 0.06f);
    leftHandDampingStep = (leftHandDamping - leftHandDampingTarget) / transitionSamples;
}

float StringVoice::pluckShapeAt (float position, float pluckPosition) const noexcept
{
    const auto left = position < pluckPosition ? position / pluckPosition
                                               : (1.0f - position) / (1.0f - pluckPosition);

    const auto centered = juce::jlimit (0.0f, 1.0f, left);
    return 2.0f * centered - 1.0f;
}

float StringVoice::readDelayLineAtOffset (int offset) const noexcept
{
    const auto index = (writeIndex + delayLength - juce::jlimit (0, delayLength - 1, offset)) % delayLength;
    return delayLine[static_cast<size_t> (index)];
}

} // namespace guitar_ag
