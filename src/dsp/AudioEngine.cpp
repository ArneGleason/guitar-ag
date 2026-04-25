#include "AudioEngine.h"

namespace guitar_ag
{

void AudioEngine::prepare (double sampleRate, int, int)
{
    for (auto& voice : voices)
        voice.prepare (sampleRate);

    reset();
}

void AudioEngine::reset()
{
    for (auto& voice : voices)
        voice.reset();

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
    for (auto& voice : voices)
    {
        if (! voice.isActive())
        {
            voice.start (noteNumber, channel, velocity);
            return;
        }
    }

    voices[static_cast<size_t> (nextVoice)].start (noteNumber, channel, velocity);
    nextVoice = (nextVoice + 1) % maxVoices;
}

void AudioEngine::noteOff (int noteNumber, int channel)
{
    for (auto& voice : voices)
        voice.release (noteNumber, channel);
}

} // namespace guitar_ag
