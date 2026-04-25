#pragma once

#include "StringVoice.h"

#include <array>

namespace guitar_ag
{

class AudioEngine
{
public:
    void prepare (double sampleRate, int maximumBlockSize, int outputChannels);
    void reset();

    void render (juce::AudioBuffer<float>& audio, const juce::MidiBuffer& midi);

private:
    static constexpr auto maxVoices = 8;

    void renderRange (juce::AudioBuffer<float>& audio, int startSample, int endSample) noexcept;
    void handleMidiMessage (const juce::MidiMessage& message);
    void noteOn (int noteNumber, int channel, float velocity);
    void noteOff (int noteNumber, int channel);

    std::array<StringVoice, maxVoices> voices;
    int nextVoice = 0;
};

} // namespace guitar_ag
