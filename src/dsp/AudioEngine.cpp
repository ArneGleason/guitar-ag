#include "AudioEngine.h"

namespace guitar_ag
{

void AudioEngine::prepare (double sampleRate, int, int)
{
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
    tone.prepare (sampleRate);
    reset();
}

void AudioEngine::reset()
{
    for (auto& voice : voices)
        voice.reset();

    fretboard.reset();
    tone.reset();
    tailSustain.setCurrentAndTargetValue (tailSustain.getTargetValue());
    pickStiffness.setCurrentAndTargetValue (pickStiffness.getTargetValue());
    pickTexture.setCurrentAndTargetValue (pickTexture.getTargetValue());
    palmMute.setCurrentAndTargetValue (palmMute.getTargetValue());
    harmonicTouch.setCurrentAndTargetValue (harmonicTouch.getTargetValue());
    stringAge.setCurrentAndTargetValue (stringAge.getTargetValue());
    nextVoice = 0;
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
        const auto sustainAmount = tailSustain.getNextValue();
        const auto palmMuteAmount = palmMute.getNextValue();
        pickStiffness.getNextValue();
        pickTexture.getNextValue();
        harmonicTouch.getNextValue();
        stringAge.getNextValue();

        for (auto& voice : voices)
            mixedSample += voice.renderSample (sustainAmount, palmMuteAmount);

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
    const auto notePickStiffness = pickStiffness.getTargetValue();
    const auto notePickTexture = pickTexture.getTargetValue();
    const auto noteHarmonicTouch = harmonicTouch.getTargetValue();
    const auto noteStringAge = stringAge.getTargetValue();

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
                         noteStringAge);
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
                       noteStringAge);
    nextVoice = (nextVoice + 1) % maxVoices;
}

void AudioEngine::noteOff (int noteNumber, int channel)
{
    fretboard.releaseNote (noteNumber, channel);

    for (auto& voice : voices)
        voice.release (noteNumber, channel);
}

} // namespace guitar_ag
