#include "ElectricGuitarTone.h"

#include <cmath>

namespace guitar_ag
{

void ElectricGuitarTone::prepare (double newSampleRate)
{
    sampleRate = newSampleRate > 0.0 ? newSampleRate : 44100.0;
    updateCoefficients();
    reset();
}

void ElectricGuitarTone::reset()
{
    previousHighPassInput = 0.0f;
    highPassState = 0.0f;
    presenceState = 0.0f;
    toneState = 0.0f;
    resonatorState1 = 0.0f;
    resonatorState2 = 0.0f;
}

float ElectricGuitarTone::processSample (float input) noexcept
{
    highPassState = 0.995f * (highPassState + input - previousHighPassInput);
    previousHighPassInput = input;
    presenceState += 0.16f * (highPassState - presenceState);
    toneState += 0.035f * (input - toneState);

    const auto resonatorNext = highPassState
                             + resonatorCoefficient * resonatorState1
                             - resonatorRadiusSquared * resonatorState2;
    const auto pickupResonance = resonatorNext - resonatorState2;
    resonatorState2 = resonatorState1;
    resonatorState1 = resonatorNext;

    const auto presence = highPassState - presenceState;
    const auto body = toneState;
    const auto shaped = 0.72f * highPassState
                      + 0.16f * body
                      + 0.62f * presence
                      + 0.0028f * pickupResonance;

    return shaped * 0.78f;
}

void ElectricGuitarTone::updateCoefficients() noexcept
{
    const auto clampedSampleRate = sampleRate > 1000.0 ? sampleRate : 44100.0;
    const auto resonanceFrequency = 3100.0f;
    const auto radius = 0.9865f;
    const auto angle = 2.0f * 3.14159265358979323846f * resonanceFrequency
                     / static_cast<float> (clampedSampleRate);

    resonatorCoefficient = 2.0f * radius * std::cos (angle);
    resonatorRadiusSquared = radius * radius;
}

} // namespace guitar_ag
