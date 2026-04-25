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
    noteNumber = -1;
    channel = 0;
    damping = baseDamping;
    lastOutput = 0.0f;
    energy = 0.0f;
    pickTransient = 0.0f;
    pickTransientDecay = 0.0f;
    active = false;
    releasing = false;
}

void StringVoice::start (int midiNoteNumber, int midiChannel, float velocity)
{
    noteNumber = midiNoteNumber;
    channel = midiChannel;
    writeIndex = 0;
    lastOutput = 0.0f;
    pickTransient = 0.0f;
    pickTransientDecay = 0.0f;
    releasing = false;
    active = true;

    const auto frequency = juce::jlimit (20.0, 8000.0, juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));
    delayLength = juce::jlimit (2, maxDelaySamples, static_cast<int> (std::round (sampleRate / frequency)));

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

    const auto filtered = 0.5f * (current + lastOutput) * damping;

    delayLine[index] = filtered;
    lastOutput = current;

    ++writeIndex;

    if (writeIndex >= delayLength)
        writeIndex = 0;

    energy = 0.9995f * energy + 0.0005f * std::abs (current);

    if (energy < 0.00008f)
    {
        reset();
        return 0.0f;
    }

    return current * outputGain;
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
    releaseDamping = 0.985f;
    damping = releasing ? releaseDamping : baseDamping;
}

float StringVoice::pluckShapeAt (float position, float pluckPosition) const noexcept
{
    const auto left = position < pluckPosition ? position / pluckPosition
                                               : (1.0f - position) / (1.0f - pluckPosition);

    const auto centered = juce::jlimit (0.0f, 1.0f, left);
    return 2.0f * centered - 1.0f;
}

} // namespace guitar_ag
