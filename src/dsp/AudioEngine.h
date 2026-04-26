#pragma once

#include "ElectricGuitarTone.h"
#include "FretboardMapper.h"
#include "StringVoice.h"

#include <array>

namespace guitar_ag
{

class AudioEngine
{
public:
    void prepare (double sampleRate, int maximumBlockSize, int outputChannels);
    void reset();
    void setTailSustain (float newTailSustain) noexcept;
    void setPickStiffness (float newPickStiffness) noexcept;
    void setPickTexture (float newPickTexture) noexcept;
    void setPalmMute (float newPalmMute) noexcept;
    void setHarmonicTouch (float newHarmonicTouch) noexcept;
    void setStringAge (float newStringAge) noexcept;
    void setBridgeIntonation (float newBridgeIntonation) noexcept;
    void setPickupPosition (float newPickupPosition) noexcept;
    void setPickupModel (int newPickupModel) noexcept;

    void render (juce::AudioBuffer<float>& audio, const juce::MidiBuffer& midi);

private:
    static constexpr auto maxVoices = 8;

    void renderRange (juce::AudioBuffer<float>& audio, int startSample, int endSample) noexcept;
    void handleMidiMessage (const juce::MidiMessage& message);
    void noteOn (int noteNumber, int channel, float velocity);
    void noteOff (int noteNumber, int channel);

    std::array<StringVoice, maxVoices> voices;
    FretboardMapper fretboard;
    ElectricGuitarTone tone;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> tailSustain { 1.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pickStiffness { 0.5f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pickTexture { 0.5f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> palmMute { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> harmonicTouch { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> stringAge { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> bridgeIntonation { 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pickupPosition { 0.39f };
    int pickupModel = 0;
    int nextVoice = 0;
};

} // namespace guitar_ag
