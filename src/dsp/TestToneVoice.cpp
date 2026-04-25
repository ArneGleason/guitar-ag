#include "TestToneVoice.h"

#include <cmath>

namespace guitar_ag
{

void TestToneVoice::prepare (double newSampleRate)
{
    sampleRate = newSampleRate > 0.0 ? newSampleRate : 44100.0;
    attackStep = static_cast<float> (1.0 / (0.01 * sampleRate));
    releaseStep = static_cast<float> (1.0 / (0.04 * sampleRate));
}

void TestToneVoice::reset()
{
    phase = 0.0;
    phaseIncrement = 0.0;
    noteNumber = -1;
    channel = 0;
    targetLevel = 0.0f;
    currentLevel = 0.0f;
    active = false;
    releasing = false;
}

void TestToneVoice::start (int midiNoteNumber, int midiChannel, float velocity)
{
    noteNumber = midiNoteNumber;
    channel = midiChannel;
    phase = 0.0;

    const auto frequency = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    phaseIncrement = juce::MathConstants<double>::twoPi * frequency / sampleRate;

    targetLevel = juce::jlimit (0.01f, 0.08f, velocity * 0.08f);
    currentLevel = 0.0f;
    active = true;
    releasing = false;
}

void TestToneVoice::release (int midiNoteNumber, int midiChannel)
{
    if (active && noteNumber == midiNoteNumber && channel == midiChannel)
        releasing = true;
}

float TestToneVoice::renderSample() noexcept
{
    if (! active)
        return 0.0f;

    if (releasing)
    {
        currentLevel -= releaseStep;

        if (currentLevel <= 0.0f)
        {
            reset();
            return 0.0f;
        }
    }
    else if (currentLevel < targetLevel)
    {
        currentLevel = juce::jmin (targetLevel, currentLevel + attackStep);
    }

    const auto sample = static_cast<float> (std::sin (phase)) * currentLevel;
    phase += phaseIncrement;

    if (phase >= juce::MathConstants<double>::twoPi)
        phase -= juce::MathConstants<double>::twoPi;

    return sample;
}

} // namespace guitar_ag
