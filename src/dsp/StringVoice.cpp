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
    pickContact = 0.0f;
    pickContactDecay = 0.0f;
    previousContactNoise = 0.0f;
    pickContactSamplesRemaining = 0;
    dispersionCoefficient = 0.0f;
    dispersionTarget = 0.0f;
    dispersionStep = 0.0f;
    dispersionInputState = 0.0f;
    dispersionOutputState = 0.0f;
    dispersionSamplesRemaining = 0;
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
    pickContact = 0.0f;
    pickContactDecay = 0.0f;
    previousContactNoise = 0.0f;
    pickContactSamplesRemaining = 0;
    dispersionInputState = 0.0f;
    dispersionOutputState = 0.0f;
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
    constexpr auto twoPi = 6.28318530717958647692f;
    const auto steelPartialAmount = (0.010f + 0.040f * brightness) * velocityGain;
    auto mean = 0.0f;

    randomState = static_cast<uint32_t> ((midiNoteNumber + 1) * 1103515245u + (midiChannel + 17) * 12345u);

    for (auto i = 0; i < delayLength; ++i)
    {
        const auto x = static_cast<float> (i) / static_cast<float> (delayLength);
        const auto shape = pluckShapeAt (x, pluckPosition);
        const auto pickDistance = std::abs (x - pluckPosition);
        const auto localPickContact = std::exp (-pickDistance * static_cast<float> (delayLength) * 0.18f);
        const auto scrapeNoise = nextNoiseSample() * localPickContact * noiseAmount;
        const auto steelPartials = 0.46f * std::sin (twoPi * 5.0f * x)
                                 + 0.34f * std::sin (twoPi * 7.0f * x)
                                 + 0.24f * std::sin (twoPi * 11.0f * x)
                                 + 0.16f * std::sin (twoPi * 13.0f * x);
        const auto pickKink = juce::jlimit (-1.0f, 1.0f, (pluckPosition - x) * 8.0f);
        const auto sample = shape * displacementAmount
                          + scrapeNoise
                          + steelPartials * steelPartialAmount
                          + pickKink * localPickContact * steelPartialAmount;

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
    pickContact = (0.006f + 0.030f * brightness) * velocityGain;
    pickContactDecay = 0.9991f - 0.00035f * brightness;
    pickContactSamplesRemaining = static_cast<int> (sampleRate * (0.014f + 0.012f * brightness));
    dispersionCoefficient = 0.030f + 0.015f * brightness;
    dispersionTarget = 0.120f + 0.030f * brightness;
    dispersionSamplesRemaining = static_cast<int> (sampleRate * (0.26f + 0.12f * brightness));
    dispersionStep = (dispersionTarget - dispersionCoefficient) / juce::jmax (1.0f, static_cast<float> (dispersionSamplesRemaining));
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

    auto contactOutput = 0.0f;

    if (pickContactSamplesRemaining > 0)
    {
        const auto contactNoise = nextNoiseSample();
        const auto highPassedContact = contactNoise - previousContactNoise;
        previousContactNoise = contactNoise;
        contactOutput = highPassedContact * pickContact;
        current += contactOutput * 0.55f;
        pickContact *= pickContactDecay;
        --pickContactSamplesRemaining;
    }

    if (releasing && leftHandDamping > leftHandDampingTarget)
        leftHandDamping = juce::jmax (leftHandDampingTarget, leftHandDamping - leftHandDampingStep);

    const auto slope = current - lastOutput;
    const auto contactDrive = softClip (slope * 2.8f) * 0.018f;
    const auto filtered = (0.58f * current + 0.42f * lastOutput + contactDrive) * damping * leftHandDamping;

    if (dispersionSamplesRemaining > 0)
    {
        dispersionCoefficient += dispersionStep;
        --dispersionSamplesRemaining;
    }
    else
    {
        dispersionCoefficient = dispersionTarget;
    }

    delayLine[index] = processDispersion (filtered);
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

    const auto pickupReadout = 0.82f * pickupSample + 0.62f * pickupVelocity + 0.28f * contactOutput;
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

float StringVoice::processDispersion (float input) noexcept
{
    const auto coefficient = juce::jlimit (-0.35f, 0.35f, dispersionCoefficient);
    const auto output = -coefficient * input + dispersionInputState + coefficient * dispersionOutputState;
    dispersionInputState = input;
    dispersionOutputState = output;
    return output;
}

float StringVoice::softClip (float value) const noexcept
{
    return std::tanh (value);
}

} // namespace guitar_ag
