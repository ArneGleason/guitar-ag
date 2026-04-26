#include "ElectricGuitarTone.h"

namespace guitar_ag
{

void ElectricGuitarTone::prepare (double newSampleRate)
{
    sampleRate = newSampleRate > 0.0 ? newSampleRate : 44100.0;
    reset();
}

void ElectricGuitarTone::reset()
{
    previousHighPassInput = 0.0f;
    highPassState = 0.0f;
    presenceState = 0.0f;
    toneState = 0.0f;
}

float ElectricGuitarTone::processSample (float input) noexcept
{
    highPassState = 0.995f * (highPassState + input - previousHighPassInput);
    previousHighPassInput = input;
    presenceState += 0.16f * (highPassState - presenceState);
    toneState += 0.035f * (input - toneState);

    const auto presence = highPassState - presenceState;
    const auto body = toneState;
    const auto shaped = 0.96f * input + 0.34f * presence + 0.045f * body;

    return shaped * 0.90f;
}

} // namespace guitar_ag
