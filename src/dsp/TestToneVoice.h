#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace guitar_ag
{

class TestToneVoice
{
public:
    void prepare (double newSampleRate);
    void reset();

    void start (int midiNoteNumber, int midiChannel, float velocity);
    void release (int midiNoteNumber, int midiChannel);

    [[nodiscard]] bool isActive() const noexcept { return active; }

    float renderSample() noexcept;

private:
    double sampleRate = 44100.0;
    double phase = 0.0;
    double phaseIncrement = 0.0;

    int noteNumber = -1;
    int channel = 0;

    float targetLevel = 0.0f;
    float currentLevel = 0.0f;
    float attackStep = 0.001f;
    float releaseStep = 0.001f;

    bool active = false;
    bool releasing = false;
};

} // namespace guitar_ag
