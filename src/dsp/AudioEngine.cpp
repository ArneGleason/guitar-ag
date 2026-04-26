#include "AudioEngine.h"

namespace guitar_ag
{

void AudioEngine::prepare (double sampleRate, int, int)
{
    for (auto& voice : voices)
        voice.prepare (sampleRate);

    tone.prepare (sampleRate);
    reset();
}

void AudioEngine::reset()
{
    for (auto& voice : voices)
        voice.reset();

    fretboard.reset();
    tone.reset();
    nextVoice = 0;
}

void AudioEngine::render (juce::AudioBuffer<float>& audio, const juce::MidiBuffer& midi)
{
    auto currentSample = 0;
    const auto totalSamples = audio.getNumSamples();

    for (const auto metadata : midi)
    {
        const auto eventSample = juce::jlimit (0, totalSamples, metadata.samplePosition);
        renderRange (audio, currentSample, eventSample);
        handleMidiMessage (metadata.getMessage());
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

        for (auto& voice : voices)
            mixedSample += voice.renderSample();

        mixedSample = tone.processSample (mixedSample);

        for (auto channel = 0; channel < numChannels; ++channel)
            audio.addSample (channel, sampleIndex, mixedSample);
    }
}

void AudioEngine::handleMidiMessage (const juce::MidiMessage& message)
{
    if (message.isNoteOn())
    {
        noteOn (message.getNoteNumber(), message.getChannel(), message.getFloatVelocity());
        return;
    }

    if (message.isNoteOff())
        noteOff (message.getNoteNumber(), message.getChannel());
}

void AudioEngine::noteOn (int noteNumber, int channel, float velocity)
{
    const auto assignment = fretboard.assignNote (noteNumber, channel);

    for (auto& voice : voices)
    {
        if (! voice.isActive())
        {
            voice.start (noteNumber, channel, velocity, assignment);
            return;
        }
    }

    auto& stolenVoice = voices[static_cast<size_t> (nextVoice)];
    fretboard.releaseNote (stolenVoice.getNoteNumber(), stolenVoice.getChannel());
    stolenVoice.start (noteNumber, channel, velocity, assignment);
    nextVoice = (nextVoice + 1) % maxVoices;
}

void AudioEngine::noteOff (int noteNumber, int channel)
{
    fretboard.releaseNote (noteNumber, channel);

    for (auto& voice : voices)
        voice.release (noteNumber, channel);
}

} // namespace guitar_ag
