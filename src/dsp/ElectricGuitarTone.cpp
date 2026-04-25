#include "ElectricGuitarTone.h"

#include <juce_core/juce_core.h>

namespace guitar_ag
{

void ElectricGuitarTone::prepare (double newSampleRate)
{
    sampleRate = newSampleRate > 0.0 ? newSampleRate : 44100.0;

    const auto pickupPosition = 0.18;
    const auto lowEPeriodSamples = sampleRate / 82.4069;
    pickupDelaySamples = juce::jlimit (1, maxPickupDelaySamples - 1,
                                       static_cast<int> (lowEPeriodSamples * pickupPosition * 0.12));

    reset();
}

void ElectricGuitarTone::reset()
{
    pickupDelay.fill (0.0f);
    pickupWriteIndex = 0;
    previousInput = 0.0f;
    previousHighPassInput = 0.0f;
    highPassState = 0.0f;
    presenceState = 0.0f;
    toneState = 0.0f;
}

float ElectricGuitarTone::processSample (float input) noexcept
{
    const auto delayedIndex = (pickupWriteIndex + maxPickupDelaySamples - pickupDelaySamples) % maxPickupDelaySamples;
    const auto delayed = pickupDelay[static_cast<size_t> (delayedIndex)];
    pickupDelay[static_cast<size_t> (pickupWriteIndex)] = input;
    pickupWriteIndex = (pickupWriteIndex + 1) % maxPickupDelaySamples;

    const auto pickupReadout = input - 0.72f * delayed;
    const auto stringVelocity = pickupReadout - previousInput;
    previousInput = pickupReadout;

    highPassState = 0.995f * (highPassState + pickupReadout - previousHighPassInput);
    previousHighPassInput = pickupReadout;
    presenceState += 0.18f * (stringVelocity - presenceState);
    toneState += 0.08f * (pickupReadout - toneState);

    const auto presence = stringVelocity - presenceState;
    const auto body = toneState;
    const auto shaped = 0.72f * pickupReadout + 0.42f * presence + 0.14f * highPassState + 0.18f * body;

    return shaped * 0.72f;
}

} // namespace guitar_ag
