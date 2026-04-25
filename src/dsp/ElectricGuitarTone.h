#pragma once

#include <array>

namespace guitar_ag
{

class ElectricGuitarTone
{
public:
    void prepare (double newSampleRate);
    void reset();

    float processSample (float input) noexcept;

private:
    static constexpr auto maxPickupDelaySamples = 128;

    std::array<float, maxPickupDelaySamples> pickupDelay {};

    double sampleRate = 44100.0;
    int pickupDelaySamples = 12;
    int pickupWriteIndex = 0;

    float previousInput = 0.0f;
    float previousHighPassInput = 0.0f;
    float highPassState = 0.0f;
    float presenceState = 0.0f;
    float toneState = 0.0f;
};

} // namespace guitar_ag
