#include "StatefulStringVoice.h"

#include <cmath>

namespace guitar_ag
{

namespace
{
constexpr auto twoPi = 6.28318530717958647692f;

float smoothStep (float value) noexcept
{
    const auto clamped = juce::jlimit (0.0f, 1.0f, value);
    return clamped * clamped * (3.0f - 2.0f * clamped);
}
} // namespace

void StatefulStringVoice::prepare (double newSampleRate)
{
    sampleRate = newSampleRate > 0.0 ? newSampleRate : 44100.0;
    reset();
}

void StatefulStringVoice::reset() noexcept
{
    primaryDelay.fill (0.0f);
    secondaryDelay.fill (0.0f);
    writeIndex = 0;
    samplesSinceStart = 0;
    noteNumber = -1;
    channel = 0;
    stringIndex = 0;
    fret = 0;
    baseFrequency = 110.0f;
    currentPrimaryDelay = static_cast<float> (sampleRate) / baseFrequency;
    currentSecondaryDelay = currentPrimaryDelay * 1.0008f;
    primaryLoopLowState = 0.0f;
    secondaryLoopLowState = 0.0f;
    previousPrimaryPickup = 0.0f;
    previousSecondaryPickup = 0.0f;
    pickupFilterState1 = 0.0f;
    pickupFilterState2 = 0.0f;
    pickupDcInput = 0.0f;
    pickupDcOutput = 0.0f;
    pickupPosition = 0.18f;
    pickupAperture = 0.018f;
    woundAmount = 0.0f;
    stringAge = 0.0f;
    energy = 0.0f;
    vibratoPhase = 0.0f;
    aftertouchPressure = 0.0f;
    aftertouchPressureTarget = 0.0f;
    mpePitchBend = 0.0f;
    mpePitchBendTarget = 0.0f;
    mpePressure = 0.0f;
    mpePressureTarget = 0.0f;
    mpeTimbre = 0.0f;
    mpeTimbreTarget = 0.0f;
    contactTipPosition = 0.0f;
    contactTravelPerSample = 0.0f;
    contactReleaseDeflection = 0.02f;
    contactStiffness = 0.5f;
    contactDamping = 0.2f;
    contactDrive = 0.1f;
    contactPolarization = 0.3f;
    previousContactRelative = 0.0f;
    contactForceState = 0.0f;
    contactForceCoefficient = 0.2f;
    contactSamples = 0;
    contactMaximumSamples = 1;
    strokeSign = 1.0f;
    randomState = 0x12345678u;
    pickupModel = 0;
    active = false;
    releasing = false;
    contactActive = false;
}

void StatefulStringVoice::start (int midiNoteNumber,
                                 int midiChannel,
                                 float velocity,
                                 const FretboardAssignment& assignment,
                                 float pickStiffness,
                                 float pickTexture,
                                 float pickBite,
                                 float newStringAge,
                                 float bridgeIntonation,
                                 float fretPressure,
                                 float pickupPositionControl,
                                 int newPickupModel,
                                 PlayerGesture gesture,
                                 PickStrokeDirection strokeDirection,
                                 uint32_t attackSeed) noexcept
{
    const auto preserveState = preserveStateOnStart && active && stringIndex == assignment.stringIndex;

    if (! preserveState)
    {
        primaryDelay.fill (0.0f);
        secondaryDelay.fill (0.0f);
        writeIndex = 0;
        primaryLoopLowState = 0.0f;
        secondaryLoopLowState = 0.0f;
        previousPrimaryPickup = 0.0f;
        previousSecondaryPickup = 0.0f;
        pickupFilterState1 = 0.0f;
        pickupFilterState2 = 0.0f;
        pickupDcInput = 0.0f;
        pickupDcOutput = 0.0f;
        contactForceState = 0.0f;
        contactActive = false;
        energy = 0.0f;
        vibratoPhase = 0.0f;
    }

    noteNumber = midiNoteNumber;
    channel = midiChannel;
    stringIndex = juce::jlimit (0, 5, assignment.stringIndex);
    fret = assignment.fret;
    woundAmount = juce::jlimit (0.0f, 1.0f, assignment.woundAmount);
    stringAge = juce::jlimit (0.0f, 1.0f, newStringAge);
    pickupModel = juce::jlimit (0, 2, newPickupModel);

    const auto intonationCents = juce::jlimit (-1.0f, 1.0f, bridgeIntonation)
                               * static_cast<float> (juce::jmax (0, assignment.fret)) * 0.22f;
    const auto pressureCents = juce::jlimit (0.0f, 1.0f, fretPressure)
                             * juce::jlimit (0.0f, 1.0f, static_cast<float> (assignment.fret) / 18.0f)
                             * 5.0f;
    baseFrequency = static_cast<float> (juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber))
                  * std::pow (2.0f, (intonationCents + pressureCents) / 1200.0f);

    constexpr auto pickupPositionMin = 0.055f;
    constexpr auto pickupPositionMax = 0.335f;
    pickupPosition = juce::jmap (juce::jlimit (0.0f, 1.0f, pickupPositionControl),
                                 pickupPositionMin,
                                 pickupPositionMax);
    pickupAperture = pickupModel == 1 ? 0.028f : 0.012f;

    const auto targetDelay = juce::jlimit (6.0f,
                                           static_cast<float> (maxDelaySamples - 4),
                                           static_cast<float> (sampleRate) / juce::jmax (20.0f, baseFrequency));

    if (! preserveState)
    {
        currentPrimaryDelay = targetDelay;
        const auto polarizationDetuneCents = 0.65f + 0.55f * woundAmount;
        currentSecondaryDelay = targetDelay * std::pow (2.0f, polarizationDetuneCents / 1200.0f);
    }

    // Preserve the physical displacement and loop filters, but restart the
    // per-gesture clock so delayed vibrato and release timing belong to the
    // new articulation rather than the previous note on this string.
    samplesSinceStart = 0;
    releasing = false;
    active = true;
    energy = juce::jmax (energy, 0.08f + 0.72f * juce::jlimit (0.0f, 1.0f, velocity));
    beginPlectrumContact (velocity,
                          pickStiffness,
                          pickTexture,
                          pickBite,
                          gesture,
                          strokeDirection,
                          attackSeed);
}

void StatefulStringVoice::release (int midiNoteNumber, int midiChannel) noexcept
{
    if (active && noteNumber == midiNoteNumber && channel == midiChannel)
        releasing = true;
}

void StatefulStringVoice::setAftertouchPressure (int midiNoteNumber, int midiChannel, float pressure) noexcept
{
    if (active && noteNumber == midiNoteNumber && channel == midiChannel)
        aftertouchPressureTarget = juce::jlimit (0.0f, 1.0f, pressure);
}

void StatefulStringVoice::setMpePitchBend (int midiChannel, float bend) noexcept
{
    if (active && channel == midiChannel)
        mpePitchBendTarget = juce::jlimit (-1.0f, 1.0f, bend);
}

void StatefulStringVoice::setMpePressure (int midiChannel, float pressure) noexcept
{
    if (active && channel == midiChannel)
        mpePressureTarget = juce::jlimit (0.0f, 1.0f, pressure);
}

void StatefulStringVoice::setMpeTimbre (int midiChannel, float timbre) noexcept
{
    if (active && channel == midiChannel)
        mpeTimbreTarget = juce::jlimit (0.0f, 1.0f, timbre);
}

float StatefulStringVoice::renderSample (float tailSustain,
                                         float palmMute,
                                         float vibratoDepthCents,
                                         float vibratoSpeedHz,
                                         float vibratoDelaySeconds,
                                         float whammySemitones,
                                         float whammySpread,
                                         float aftertouchBendSemitones,
                                         float mpePressureAmount,
                                         float mpeTimbreAmount,
                                         float mpePitchBendRange,
                                         float neckSlideSemitones,
                                         float slideLift,
                                         float ampFeedback,
                                         float feedbackLoopSignal) noexcept
{
    if (! active)
        return 0.0f;

    aftertouchPressure += (aftertouchPressureTarget - aftertouchPressure) * 0.0030f;
    mpePitchBend += (mpePitchBendTarget - mpePitchBend) * 0.0030f;
    mpePressure += (mpePressureTarget - mpePressure) * 0.0030f;
    mpeTimbre += (mpeTimbreTarget - mpeTimbre) * 0.0035f;

    const auto pitchRatio = calculatePitchRatio (vibratoDepthCents,
                                                  vibratoSpeedHz,
                                                  vibratoDelaySeconds,
                                                  whammySemitones,
                                                  whammySpread,
                                                  aftertouchBendSemitones,
                                                  mpePitchBendRange,
                                                  neckSlideSemitones);
    const auto frequency = juce::jlimit (20.0f, 8000.0f, baseFrequency * pitchRatio);
    const auto targetPrimaryDelay = juce::jlimit (6.0f,
                                                  static_cast<float> (maxDelaySamples - 4),
                                                  static_cast<float> (sampleRate) / frequency);
    const auto polarizationDetuneCents = 0.65f + 0.55f * woundAmount;
    const auto targetSecondaryDelay = targetPrimaryDelay * std::pow (2.0f, polarizationDetuneCents / 1200.0f);
    const auto delaySmoothing = 0.0025f;
    currentPrimaryDelay += delaySmoothing * (targetPrimaryDelay - currentPrimaryDelay);
    currentSecondaryDelay += delaySmoothing * (targetSecondaryDelay - currentSecondaryDelay);

    const auto primaryRead = readFractional (primaryDelay, currentPrimaryDelay);
    const auto secondaryRead = readFractional (secondaryDelay, currentSecondaryDelay);
    const auto coupling = 0.0014f * (secondaryRead - primaryRead);
    const auto coupledPrimary = primaryRead + coupling;
    const auto coupledSecondary = secondaryRead - coupling;

    primaryLoopLowState += 0.16f * (coupledPrimary - primaryLoopLowState);
    secondaryLoopLowState += 0.13f * (coupledSecondary - secondaryLoopLowState);

    const auto palmAmount = juce::jlimit (0.0f, 1.0f, palmMute);
    const auto sustainAmount = juce::jlimit (0.0f, 1.0f, tailSustain);
    const auto pressureAmount = juce::jlimit (0.0f, 1.0f, mpePressure * mpePressureAmount);
    const auto timbreAmount = juce::jlimit (0.0f, 1.0f, mpeTimbre * mpeTimbreAmount);
    auto decaySeconds = 3.2f + 7.5f * sustainAmount;
    decaySeconds *= 1.0f - 0.30f * stringAge;
    decaySeconds *= 1.0f + 0.20f * pressureAmount;

    if (palmAmount > 0.0001f)
        decaySeconds = juce::jmap (std::pow (palmAmount, 1.25f), decaySeconds, 0.085f);

    if (releasing)
        decaySeconds = juce::jmin (decaySeconds, 0.20f + 0.38f * sustainAmount);

    const auto cyclesToMinus60Db = juce::jmax (1.0f, decaySeconds * frequency);
    const auto loopGain = std::pow (0.001f, 1.0f / cyclesToMinus60Db);
    const auto ageLoss = 0.002f + 0.016f * stringAge;
    const auto palmLoss = 0.12f * palmAmount;
    const auto primaryHighKeep = juce::jlimit (0.72f, 0.997f,
                                               0.997f - ageLoss - palmLoss + 0.002f * timbreAmount);
    const auto secondaryHighKeep = juce::jlimit (0.74f, 0.998f,
                                                 primaryHighKeep + 0.0015f + 0.0015f * woundAmount);
    const auto primaryFiltered = primaryLoopLowState
                               + (coupledPrimary - primaryLoopLowState) * primaryHighKeep;
    const auto secondaryFiltered = secondaryLoopLowState
                                 + (coupledSecondary - secondaryLoopLowState) * secondaryHighKeep;

    const auto localPrimary = readFractional (primaryDelay, 1.0f);
    const auto localSecondary = readFractional (secondaryDelay, 1.0f);
    const auto contactForce = renderPlectrumContact (localPrimary, localSecondary);
    const auto feedbackInjection = juce::jlimit (-0.025f,
                                                 0.025f,
                                                 feedbackLoopSignal * juce::jlimit (0.0f, 1.0f, ampFeedback) * 0.018f);
    const auto liftDamping = 1.0f - 0.12f * juce::jlimit (0.0f, 1.0f, slideLift);
    const auto primaryWrite = (primaryFiltered * loopGain + contactForce + feedbackInjection) * liftDamping;
    const auto secondaryWrite = (secondaryFiltered * loopGain
                                  + contactForce * contactPolarization
                                  - feedbackInjection * 0.28f)
                                * liftDamping;

    primaryDelay[static_cast<size_t> (writeIndex)] = std::tanh (primaryWrite);
    secondaryDelay[static_cast<size_t> (writeIndex)] = std::tanh (secondaryWrite);
    writeIndex = (writeIndex + 1) % maxDelaySamples;

    auto primaryPickup = readPickup (primaryDelay, currentPrimaryDelay, pickupPosition, pickupAperture);
    auto secondaryPickup = readPickup (secondaryDelay,
                                       currentSecondaryDelay,
                                       juce::jlimit (0.04f, 0.38f, pickupPosition + 0.012f),
                                       pickupAperture * 1.12f);

    if (pickupModel == 1)
    {
        primaryPickup = 0.5f * (primaryPickup
                                + readPickup (primaryDelay,
                                              currentPrimaryDelay,
                                              juce::jlimit (0.04f, 0.38f, pickupPosition + 0.046f),
                                              pickupAperture));
    }
    else if (pickupModel == 2)
    {
        const auto secondCoil = readPickup (primaryDelay,
                                            currentPrimaryDelay,
                                            juce::jlimit (0.04f, 0.38f, pickupPosition + 0.086f),
                                            pickupAperture);
        primaryPickup = 0.72f * (primaryPickup - secondCoil);
    }

    previousPrimaryPickup = primaryPickup;
    previousSecondaryPickup = secondaryPickup;
    const auto rawPickup = primaryPickup + secondaryPickup * (0.28f + 0.16f * woundAmount);
    const auto pickupCutoff = pickupModel == 1 ? 4800.0f : 6800.0f;
    const auto pickupCoefficient = 1.0f - std::exp (-twoPi * pickupCutoff / static_cast<float> (sampleRate));
    pickupFilterState1 += pickupCoefficient * (rawPickup - pickupFilterState1);
    pickupFilterState2 += pickupCoefficient * (pickupFilterState1 - pickupFilterState2);
    const auto dcBlocked = pickupFilterState2 - pickupDcInput + 0.995f * pickupDcOutput;
    pickupDcInput = pickupFilterState2;
    pickupDcOutput = dcBlocked;
    const auto pickupOutput = dcBlocked;

    ++samplesSinceStart;
    energy = 0.99990f * energy + 0.00010f * std::abs (pickupOutput) * 4.0f;

    if (releasing && ! contactActive)
        energy *= 0.99980f;

    if (releasing
        && samplesSinceStart > static_cast<int> (sampleRate * 0.10)
        && energy < 0.000020f)
    {
        reset();
        return 0.0f;
    }

    return pickupOutput * 220.0f * (0.92f + 0.14f * pressureAmount);
}

float StatefulStringVoice::readFractional (const std::array<float, maxDelaySamples>& buffer,
                                           float delaySamples) const noexcept
{
    auto readPosition = static_cast<float> (writeIndex) - juce::jlimit (1.0f,
                                                                        static_cast<float> (maxDelaySamples - 2),
                                                                        delaySamples);

    while (readPosition < 0.0f)
        readPosition += static_cast<float> (maxDelaySamples);

    while (readPosition >= static_cast<float> (maxDelaySamples))
        readPosition -= static_cast<float> (maxDelaySamples);

    const auto index0 = static_cast<int> (readPosition);
    const auto index1 = (index0 + 1) % maxDelaySamples;
    const auto fraction = readPosition - static_cast<float> (index0);
    return buffer[static_cast<size_t> (index0)]
         + fraction * (buffer[static_cast<size_t> (index1)] - buffer[static_cast<size_t> (index0)]);
}

float StatefulStringVoice::readPickup (const std::array<float, maxDelaySamples>& buffer,
                                       float loopDelaySamples,
                                       float position,
                                       float aperture) const noexcept
{
    const auto centerDelay = juce::jlimit (1.0f, loopDelaySamples - 2.0f, loopDelaySamples * position);
    const auto apertureDelay = juce::jlimit (0.5f, loopDelaySamples * 0.08f, loopDelaySamples * aperture);
    const auto center = readFractional (buffer, centerDelay);
    const auto ahead = readFractional (buffer, centerDelay + apertureDelay);
    const auto behind = readFractional (buffer, juce::jmax (1.0f, centerDelay - apertureDelay));
    return 0.58f * center + 0.21f * (ahead + behind);
}

float StatefulStringVoice::renderPlectrumContact (float localPrimary, float localSecondary) noexcept
{
    if (! contactActive)
    {
        contactForceState += contactForceCoefficient * (0.0f - contactForceState);
        return contactForceState;
    }

    contactTipPosition += contactTravelPerSample;
    const auto localStringPosition = 0.78f * localPrimary + 0.22f * localSecondary;
    const auto relative = strokeSign * (contactTipPosition - localStringPosition);
    const auto relativeVelocity = relative - previousContactRelative;
    previousContactRelative = relative;
    ++contactSamples;

    if ((relative <= 0.0f && contactSamples > 3)
        || relative >= contactReleaseDeflection
        || contactSamples >= contactMaximumSamples)
    {
        contactActive = false;
        contactForceState += contactForceCoefficient * (0.0f - contactForceState);
        return contactForceState;
    }

    const auto normalizedDeflection = juce::jlimit (0.0f, 1.0f, relative / contactReleaseDeflection);
    const auto elasticForce = normalizedDeflection * (0.32f + 0.68f * contactStiffness);
    const auto dampingForce = relativeVelocity * contactDamping / juce::jmax (0.0001f, contactReleaseDeflection);
    const auto targetForce = strokeSign * std::tanh ((elasticForce + dampingForce) * 1.45f) * contactDrive;
    contactForceState += contactForceCoefficient * (targetForce - contactForceState);
    return contactForceState;
}

void StatefulStringVoice::beginPlectrumContact (float velocity,
                                                float pickStiffness,
                                                float pickTexture,
                                                float pickBite,
                                                PlayerGesture gesture,
                                                PickStrokeDirection strokeDirection,
                                                uint32_t attackSeed) noexcept
{
    randomState = attackSeed != 0u ? attackSeed : 0x12345678u;
    const auto depthScatter = nextRandomBipolar();
    const auto angleScatter = nextRandomBipolar();
    const auto inputVelocity = juce::jlimit (0.0f, 1.0f, velocity);
    const auto velocityGain = 0.08f + 0.92f * std::sqrt (inputVelocity);
    const auto stiffness = juce::jlimit (0.0f, 1.0f, pickStiffness);
    const auto texture = juce::jlimit (0.0f, 1.0f, pickTexture);
    const auto bite = juce::jlimit (0.0f, 1.0f, pickBite);
    const auto depthInput = juce::jlimit (0.0f,
                                         1.0f,
                                         0.06f + 0.68f * inputVelocity + 0.22f * bite + 0.035f * depthScatter);
    const auto releasedDepth = smoothStep ((depthInput - 0.12f) / 0.78f);
    auto gestureScale = 1.0f;

    if (gesture == PlayerGesture::HammerOn)
        gestureScale = 0.36f;
    else if (gesture == PlayerGesture::PullOff)
        gestureScale = 0.42f;
    else if (gesture == PlayerGesture::RightHandTap)
        gestureScale = 0.58f;

    strokeSign = strokeDirection == PickStrokeDirection::Down ? 1.0f : -1.0f;
    contactReleaseDeflection = 0.0045f + 0.052f * std::pow (releasedDepth, 1.20f);
    const auto contactDurationSeconds = 0.00075f
                                      + 0.0028f * (1.0f - stiffness)
                                      + 0.00055f * (1.0f - inputVelocity);
    const auto contactDurationSamples = juce::jmax (8,
                                                    static_cast<int> (sampleRate * contactDurationSeconds));
    contactMaximumSamples = juce::jmax (contactDurationSamples + 2,
                                        static_cast<int> (static_cast<float> (contactDurationSamples) * 1.45f));
    contactTravelPerSample = strokeSign * contactReleaseDeflection
                           / static_cast<float> (contactDurationSamples);
    contactStiffness = 0.18f + 0.82f * stiffness;
    contactDamping = 0.08f + 0.22f * (1.0f - stiffness) + 0.08f * texture;
    contactDrive = (0.065f + 0.255f * velocityGain) * gestureScale;
    const auto contactCutoff = 1500.0f + 4200.0f * stiffness;
    contactForceCoefficient = 1.0f - std::exp (-twoPi * contactCutoff / static_cast<float> (sampleRate));
    contactPolarization = juce::jlimit (-0.52f,
                                        0.52f,
                                        strokeSign * (0.25f + 0.14f * woundAmount + 0.10f * angleScatter));
    const auto localStringPosition = 0.78f * readFractional (primaryDelay, 1.0f)
                                   + 0.22f * readFractional (secondaryDelay, 1.0f);
    contactTipPosition = localStringPosition;
    previousContactRelative = 0.0f;
    contactSamples = 0;
    contactActive = true;
}

float StatefulStringVoice::calculatePitchRatio (float vibratoDepthCents,
                                                 float vibratoSpeedHz,
                                                 float vibratoDelaySeconds,
                                                 float whammySemitones,
                                                 float whammySpread,
                                                 float aftertouchBendSemitones,
                                                 float mpePitchBendRange,
                                                 float neckSlideSemitones) noexcept
{
    const auto heldSeconds = static_cast<float> (samplesSinceStart) / static_cast<float> (sampleRate);
    const auto vibratoDelay = juce::jmax (0.0f, vibratoDelaySeconds);
    const auto vibratoRamp = juce::jlimit (0.0f, 1.0f, (heldSeconds - vibratoDelay) / 0.08f);
    vibratoPhase += twoPi * juce::jlimit (0.1f, 14.0f, vibratoSpeedHz) / static_cast<float> (sampleRate);

    if (vibratoPhase >= twoPi)
        vibratoPhase -= twoPi;

    const auto vibratoSemitones = std::sin (vibratoPhase)
                                * juce::jlimit (0.0f, 200.0f, vibratoDepthCents)
                                * vibratoRamp / 100.0f;
    const auto stringSpread = (static_cast<float> (stringIndex) - 2.5f) / 2.5f;
    const auto spreadWhammy = whammySemitones
                            * (1.0f + stringSpread * juce::jlimit (0.0f, 1.0f, whammySpread) * 0.035f);
    const auto semitones = mpePitchBend * juce::jlimit (0.0f, 96.0f, mpePitchBendRange)
                         + aftertouchPressure * juce::jlimit (0.0f, 12.0f, aftertouchBendSemitones)
                         + spreadWhammy
                         + juce::jlimit (-24.0f, 24.0f, neckSlideSemitones)
                         + vibratoSemitones;
    return std::pow (2.0f, juce::jlimit (-48.0f, 48.0f, semitones) / 12.0f);
}

float StatefulStringVoice::nextRandomBipolar() noexcept
{
    randomState = randomState * 1664525u + 1013904223u;
    const auto value = static_cast<float> ((randomState >> 8) & 0x00ffffffu)
                     / static_cast<float> (0x00ffffffu);
    return 2.0f * value - 1.0f;
}

} // namespace guitar_ag
