#pragma once

namespace guitar_ag
{

class ElectricGuitarTone
{
public:
    void prepare (double newSampleRate);
    void reset();

    float processSample (float input) noexcept;

private:
    double sampleRate = 44100.0;

    float previousHighPassInput = 0.0f;
    float highPassState = 0.0f;
    float presenceState = 0.0f;
    float toneState = 0.0f;
};

} // namespace guitar_ag
