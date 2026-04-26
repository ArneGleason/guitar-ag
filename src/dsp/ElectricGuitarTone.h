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
    void updateCoefficients() noexcept;

    double sampleRate = 44100.0;

    float previousHighPassInput = 0.0f;
    float highPassState = 0.0f;
    float presenceState = 0.0f;
    float toneState = 0.0f;
    float resonatorCoefficient = 0.0f;
    float resonatorRadiusSquared = 0.0f;
    float resonatorState1 = 0.0f;
    float resonatorState2 = 0.0f;
};

} // namespace guitar_ag
