#include "StringVoice.h"

#include <cmath>

namespace guitar_ag
{

void StringVoice::prepare (double newSampleRate)
{
    sampleRate = newSampleRate > 0.0 ? newSampleRate : 44100.0;
    reset();
}

void StringVoice::reset()
{
    delayLine.fill (0.0f);
    secondaryDelayLine.fill (0.0f);
    modalSine.fill (0.0f);
    modalCosine.fill (1.0f);
    modalSinStep.fill (0.0f);
    modalCosStep.fill (1.0f);
    modalAmplitude.fill (0.0f);
    modalDecay.fill (1.0f);
    modalTailDampingScale.fill (1.0f);

    delayLength = 1;
    writeIndex = 0;
    pickupOffsetSamples = 1;
    secondaryPickupOffsetSamples = 1;
    pickupApertureSamples = 1;
    samplesSinceStart = 0;
    noteNumber = -1;
    channel = 0;
    stringIndex = 0;
    fret = 0;
    woundAmount = 0.0f;
    damping = baseDamping;
    lastOutput = 0.0f;
    lastSecondaryOutput = 0.0f;
    previousPickupSample = 0.0f;
    previousSecondaryPickupSample = 0.0f;
    energy = 0.0f;
    pickTransient = 0.0f;
    pickTransientDecay = 0.0f;
    pickContact = 0.0f;
    pickContactDecay = 0.0f;
    previousContactNoise = 0.0f;
    pickContactSamplesRemaining = 0;
    attackRampSeconds = 0.0025f;
    modalReleaseDecay = 1.0f;
    resonanceCoefficient.fill (0.0f);
    resonanceRadiusSquared.fill (0.0f);
    resonanceState1.fill (0.0f);
    resonanceState2.fill (0.0f);
    resonanceEnvelope = 0.0f;
    resonanceDecay = 0.0f;
    resonanceMoveSamples = 1;
    dampingTiltState = 0.0f;
    secondaryDampingTiltState = 0.0f;
    highFeedbackGain = 1.0f;
    highFeedbackGainTarget = 1.0f;
    highFeedbackGainStep = 0.0f;
    highFeedbackGainSamplesRemaining = 0;
    secondaryHighFeedbackGain = 1.0f;
    secondaryHighFeedbackGainTarget = 1.0f;
    secondaryHighFeedbackGainStep = 0.0f;
    leftHandDamping = 1.0f;
    leftHandDampingTarget = 1.0f;
    leftHandDampingStep = 0.0f;
    active = false;
    releasing = false;
    woundString = false;
}

void StringVoice::start (int midiNoteNumber,
                         int midiChannel,
                         float velocity,
                         const FretboardAssignment& assignment,
                         float pickStiffness,
                         float pickTexture)
{
    const auto stiffnessAmount = juce::jlimit (0.0f, 1.0f, pickStiffness);
    const auto textureAmount = juce::jlimit (0.0f, 1.0f, pickTexture);
    const auto stiffnessBipolar = 2.0f * stiffnessAmount - 1.0f;
    const auto textureScale = 0.15f + 1.70f * textureAmount;
    const auto pickEdgeScale = 0.65f + 0.70f * stiffnessAmount;
    const auto partialStiffnessScale = 0.75f + 0.50f * stiffnessAmount;

    noteNumber = midiNoteNumber;
    channel = midiChannel;
    stringIndex = assignment.stringIndex;
    fret = assignment.fret;
    woundAmount = juce::jlimit (0.0f, 1.0f, assignment.woundAmount);
    writeIndex = 0;
    samplesSinceStart = 0;
    lastOutput = 0.0f;
    lastSecondaryOutput = 0.0f;
    previousPickupSample = 0.0f;
    previousSecondaryPickupSample = 0.0f;
    pickTransient = 0.0f;
    pickTransientDecay = 0.0f;
    pickContact = 0.0f;
    pickContactDecay = 0.0f;
    previousContactNoise = 0.0f;
    pickContactSamplesRemaining = 0;
    attackRampSeconds = juce::jmap (stiffnessAmount, 0.0046f, 0.0014f);
    modalSine.fill (0.0f);
    modalCosine.fill (1.0f);
    modalSinStep.fill (0.0f);
    modalCosStep.fill (1.0f);
    modalAmplitude.fill (0.0f);
    modalDecay.fill (1.0f);
    modalTailDampingScale.fill (1.0f);
    modalReleaseDecay = 1.0f;
    resonanceState1.fill (0.0f);
    resonanceState2.fill (0.0f);
    dampingTiltState = 0.0f;
    secondaryDampingTiltState = 0.0f;
    leftHandDamping = 1.0f;
    leftHandDampingTarget = 1.0f;
    leftHandDampingStep = 0.0f;
    releasing = false;
    active = true;
    woundString = woundAmount > 0.0f;

    const auto frequency = juce::jlimit (20.0, 8000.0, juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));
    delayLength = juce::jlimit (2, maxDelaySamples, static_cast<int> (std::round (sampleRate / frequency)));
    pickupOffsetSamples = juce::jlimit (1, delayLength - 1, static_cast<int> (std::round (delayLength * 0.18f)));
    secondaryPickupOffsetSamples = juce::jlimit (1, delayLength - 1, static_cast<int> (std::round (delayLength * 0.205f)));
    pickupApertureSamples = juce::jlimit (1, delayLength / 9, static_cast<int> (std::round (delayLength * 0.012f)));

    const auto velocityGain = applyVelocityCurve (velocity);
    const auto strikeVelocity = 0.05f + (velocityGain - 0.05f) * (0.63f / 0.95f);
    const auto velocityNormal = juce::jlimit (0.0f, 1.0f, (strikeVelocity - 0.05f) / 0.95f);
    const auto strikeInput = juce::jlimit (0.0f, 1.0f, velocityNormal * 1.55f);
    const auto strikeAmount = std::pow (strikeInput, 0.58f);
    const auto hardStrike = std::pow (juce::jlimit (0.0f, 1.0f, (velocityNormal - 0.46f) / 0.54f), 0.55f);
    const auto brightness = strikeAmount;
    const auto pluckPosition = juce::jmap (strikeAmount, 0.225f, 0.080f);
    const auto pickupPosition = 0.165f;
    const auto pickupWidth = 0.026f;
    const auto displacementAmount = 0.70f * velocityGain;
    const auto horizontalAmount = (0.28f + 0.06f * woundAmount) * velocityGain;
    const auto noiseAmount = (0.006f + 0.014f * brightness) * (1.0f + 0.2f * woundAmount) * textureScale;
    constexpr auto twoPi = 6.28318530717958647692f;
    const auto steelPartialAmount = (0.012f + 0.032f * brightness) * velocityGain * partialStiffnessScale;
    auto mean = 0.0f;
    auto secondaryMean = 0.0f;

    randomState = static_cast<uint32_t> ((midiNoteNumber + 1) * 1103515245u + (midiChannel + 17) * 12345u);

    for (auto i = 0; i < delayLength; ++i)
    {
        const auto x = static_cast<float> (i) / static_cast<float> (delayLength);
        const auto shape = pluckShapeAt (x, pluckPosition);
        const auto horizontalShape = pluckShapeAt (std::fmod (x + 0.017f, 1.0f), pluckPosition);
        const auto pickDistance = std::abs (x - pluckPosition);
        const auto localPickContact = std::exp (-pickDistance * static_cast<float> (delayLength) * (0.12f + 0.12f * stiffnessAmount));
        const auto scrapeNoise = nextNoiseSample() * localPickContact * noiseAmount;
        const auto steelPartials = 0.46f * std::sin (twoPi * 5.0f * x)
                                 + 0.34f * std::sin (twoPi * 7.0f * x)
                                 + 0.24f * std::sin (twoPi * 11.0f * x)
                                 + 0.16f * std::sin (twoPi * 13.0f * x);
        const auto horizontalPartials = 0.38f * std::sin (twoPi * (5.0f * x + 0.19f))
                                      + 0.28f * std::sin (twoPi * (7.0f * x + 0.31f))
                                      + 0.18f * std::sin (twoPi * (11.0f * x + 0.43f));
        const auto pickKink = juce::jlimit (-1.0f, 1.0f, (pluckPosition - x) * 8.0f);
        const auto sample = shape * displacementAmount
                          + scrapeNoise
                          + steelPartials * steelPartialAmount
                          + pickKink * localPickContact * steelPartialAmount * pickEdgeScale;
        const auto secondarySample = horizontalShape * horizontalAmount
                                   - scrapeNoise * 0.18f
                                   + horizontalPartials * steelPartialAmount * (0.42f + 0.46f * textureAmount)
                                   - pickKink * localPickContact * steelPartialAmount * 0.38f * pickEdgeScale;

        delayLine[static_cast<size_t> (i)] = sample;
        secondaryDelayLine[static_cast<size_t> (i)] = secondarySample;
        mean += sample;
        secondaryMean += secondarySample;
    }

    mean /= static_cast<float> (delayLength);
    secondaryMean /= static_cast<float> (delayLength);

    for (auto i = 0; i < delayLength; ++i)
    {
        delayLine[static_cast<size_t> (i)] -= mean;
        secondaryDelayLine[static_cast<size_t> (i)] -= secondaryMean;
    }

    for (auto i = delayLength; i < maxDelaySamples; ++i)
    {
        delayLine[static_cast<size_t> (i)] = 0.0f;
        secondaryDelayLine[static_cast<size_t> (i)] = 0.0f;
    }

    energy = velocityGain;
    pickTransient = (0.004f + 0.014f * brightness) * pickEdgeScale * (nextNoiseSample() >= 0.0f ? 1.0f : -1.0f);
    pickTransientDecay = 0.9990f - 0.0015f * brightness - 0.00025f * stiffnessBipolar;
    pickContact = (0.003f + 0.012f * brightness) * velocityGain * textureScale;
    pickContactDecay = 0.9993f - 0.00025f * brightness + 0.00018f * textureAmount;
    pickContactSamplesRemaining = static_cast<int> (sampleRate * (0.005f + 0.011f * textureAmount + 0.006f * brightness));
    configureResonator (0, frequency * 5.0f, 0.9895f);
    configureResonator (1, frequency * 7.0f, 0.9880f);
    configureResonator (2, frequency * 11.0f, 0.9860f);
    resonanceEnvelope = (0.012f + 0.045f * brightness) * velocityGain;
    resonanceDecay = 0.99976f - 0.00008f * brightness;
    resonanceMoveSamples = juce::jmax (1, static_cast<int> (sampleRate * (0.36f + 0.16f * brightness)));
    highFeedbackGain = 0.9995f;
    highFeedbackGainTarget = 0.9935f - 0.0015f * brightness;
    highFeedbackGainSamplesRemaining = juce::jmax (1, static_cast<int> (sampleRate * (0.55f + 0.25f * brightness)));
    highFeedbackGainStep = (highFeedbackGainTarget - highFeedbackGain)
                         / juce::jmax (1.0f, static_cast<float> (highFeedbackGainSamplesRemaining));
    secondaryHighFeedbackGain = 0.9992f;
    secondaryHighFeedbackGainTarget = 0.9955f - 0.0010f * brightness + 0.0010f * woundAmount;
    secondaryHighFeedbackGainStep = (secondaryHighFeedbackGainTarget - secondaryHighFeedbackGain)
                                  / juce::jmax (1.0f, static_cast<float> (highFeedbackGainSamplesRemaining));

    auto modeIndex = 0;
    const auto stiffness = 0.000045f + (0.000080f - 0.000045f) * woundAmount;
    const auto modalGain = (0.020f + 0.070f * std::pow (velocityNormal, 0.62f)) * (0.70f + 0.85f * velocityGain);
    const auto contactWidth = juce::jmap (strikeAmount, 0.070f, 0.006f) * (1.0f - 0.45f * stiffnessBipolar);
    const auto attackModeGain = (juce::jmap (strikeAmount, 0.008f, 0.075f) + 0.045f * hardStrike)
                              * pickEdgeScale
                              * (0.82f + 0.36f * textureAmount);

    for (auto harmonic = 1; harmonic <= 32 && modeIndex < modalCount; ++harmonic)
    {
        const auto harmonicFloat = static_cast<float> (harmonic);
        const auto stiffFrequency = static_cast<float> (frequency) * harmonicFloat
                                  * std::sqrt (1.0f + stiffness * harmonicFloat * harmonicFloat);

        if (stiffFrequency >= sampleRate * 0.43)
            break;

        const auto pluckShape = std::sin (twoPi * 0.5f * harmonicFloat * pluckPosition);
        const auto pickupShape = std::sin (twoPi * 0.5f * harmonicFloat * pickupPosition);
        const auto aperture = std::abs (harmonicFloat * pickupWidth) < 0.0001f
                            ? 1.0f
                            : std::sin (twoPi * 0.5f * harmonicFloat * pickupWidth)
                                / (twoPi * 0.5f * harmonicFloat * pickupWidth);
        const auto decayBaseSeconds = 6.4f + (8.2f - 6.4f) * woundAmount;
        const auto decayCurvature = 0.0090f + (0.0065f - 0.0090f) * woundAmount;
        const auto decaySeconds = decayBaseSeconds / (1.0f + decayCurvature * harmonicFloat * harmonicFloat);
        const auto decay = std::pow (0.001f, 1.0f / static_cast<float> (sampleRate * decaySeconds));
        const auto tiltStart = 1.00f + (0.92f - 1.00f) * woundAmount;
        const auto tiltEnd = 0.48f + (0.34f - 0.48f) * woundAmount;
        const auto tiltExponent = juce::jmap (strikeAmount, tiltStart, tiltEnd);
        const auto contactFilter = std::exp (-contactWidth * harmonicFloat);
        const auto partialTilt = contactFilter / std::pow (harmonicFloat, tiltExponent);
        const auto velocityScale = juce::jlimit (0.12f, 4.0f, stiffFrequency / 470.0f);
        const auto attackEmphasis = 1.0f
                                  + strikeAmount * juce::jlimit (0.0f, 3.0f, (harmonicFloat - 1.0f) / 8.0f)
                                  + hardStrike * juce::jlimit (0.0f, 4.0f, (harmonicFloat - 4.0f) / 8.0f);
        const auto amplitude = modalGain * pluckShape * pickupShape * aperture * partialTilt * velocityScale * attackEmphasis;
        const auto phase = (harmonic % 2 == 0 ? 0.18f : -0.11f) * harmonicFloat;
        const auto tailDampingScale = juce::jlimit (0.14f, 0.62f, 0.11f + 0.012f * harmonicFloat);

        configureMode (modeIndex++, stiffFrequency, amplitude, decay, phase, tailDampingScale);

        if (modeIndex < modalCount && harmonic >= 4)
        {
            const auto sideFrequency = stiffFrequency * (1.0035f + (0.00025f + 0.00045f * woundAmount) * harmonicFloat);
            const auto sideDecay = std::pow (0.001f, 1.0f / static_cast<float> (sampleRate * decaySeconds * 0.95f));
            const auto plainSideRegime = juce::jmap (strikeAmount, 0.35f, 1.55f) + hardStrike * 0.55f;
            const auto woundSideRegime = juce::jmap (strikeAmount, 0.45f, 2.20f) + hardStrike * 0.95f;
            const auto sideRegime = plainSideRegime + (woundSideRegime - plainSideRegime) * woundAmount;
            const auto sideAmount = 0.045f + ((0.12f + 0.014f * harmonicFloat) - 0.045f) * woundAmount;
            configureMode (modeIndex++,
                           sideFrequency,
                           amplitude * sideRegime * sideAmount,
                           sideDecay,
                           phase + 1.7f,
                           juce::jlimit (0.28f, 0.76f, tailDampingScale + 0.14f));
        }

        if (woundAmount > 0.0f && harmonic >= 2 && harmonic <= 18 && modeIndex < modalCount)
        {
            const auto windingFrequency = static_cast<float> (frequency)
                                        * (harmonicFloat + 0.34f + 0.011f * harmonicFloat * harmonicFloat);
            const auto windingDecaySeconds = 0.72f + 0.055f * harmonicFloat;
            const auto windingDecay = std::pow (0.001f, 1.0f / static_cast<float> (sampleRate * windingDecaySeconds));
            const auto windingAperture = 1.0f / std::sqrt (harmonicFloat);

            configureMode (modeIndex++,
                           windingFrequency,
                           std::abs (amplitude)
                               * (juce::jmap (strikeAmount, 0.35f, 2.50f) + hardStrike * 1.25f)
                               * (0.12f + 0.008f * harmonicFloat)
                               * windingAperture
                               * woundAmount,
                           windingDecay,
                           phase + 2.35f + 0.29f * harmonicFloat,
                           0.65f);
        }

        if (strikeAmount > 0.18f && harmonic >= 4 && harmonic <= 26 && modeIndex < modalCount)
        {
            const auto chirpFrequency = stiffFrequency * (1.030f + 0.0025f * harmonicFloat + 0.010f * hardStrike);
            const auto chirpSeconds = juce::jmap (strikeAmount, 0.075f, 0.032f)
                                    + 0.0015f * harmonicFloat;
            const auto chirpDecay = std::pow (0.001f, 1.0f / static_cast<float> (sampleRate * chirpSeconds));

            configureMode (modeIndex++,
                           chirpFrequency,
                           std::abs (amplitude) * attackModeGain * 0.48f * std::pow (harmonicFloat, 0.72f),
                           chirpDecay,
                           phase + 0.63f * harmonicFloat);
        }
    }

    updateDamping();
}

void StringVoice::release (int midiNoteNumber, int midiChannel)
{
    if (active && noteNumber == midiNoteNumber && channel == midiChannel)
    {
        releasing = true;
        startLeftHandRelease();
        updateDamping();
    }
}

float StringVoice::renderSample (float tailSustain) noexcept
{
    if (! active)
        return 0.0f;

    const auto sustainAmount = juce::jlimit (0.0f, 1.0f, tailSustain);

    if (releasing)
        modalReleaseDecay = juce::jmin (modalReleaseDecay, 0.99935f);

    auto modalOutput = 0.0f;
    const auto heldSeconds = static_cast<float> (samplesSinceStart) / static_cast<float> (sampleRate);
    const auto tailBlend = releasing ? 0.0f : sustainAmount * juce::jlimit (0.0f, 1.0f, (heldSeconds - 0.55f) / 1.60f);

    for (auto mode = 0; mode < modalCount; ++mode)
    {
        modalOutput += modalAmplitude[static_cast<size_t> (mode)] * modalCosine[static_cast<size_t> (mode)];

        const auto nextSine = modalSine[static_cast<size_t> (mode)] * modalCosStep[static_cast<size_t> (mode)]
                            + modalCosine[static_cast<size_t> (mode)] * modalSinStep[static_cast<size_t> (mode)];
        const auto nextCosine = modalCosine[static_cast<size_t> (mode)] * modalCosStep[static_cast<size_t> (mode)]
                              - modalSine[static_cast<size_t> (mode)] * modalSinStep[static_cast<size_t> (mode)];

        modalSine[static_cast<size_t> (mode)] = nextSine;
        modalCosine[static_cast<size_t> (mode)] = nextCosine;

        const auto modeIndex = static_cast<size_t> (mode);
        const auto normalDecay = modalDecay[modeIndex];
        const auto relaxedDecay = 1.0f - (1.0f - normalDecay) * modalTailDampingScale[modeIndex];
        const auto effectiveDecay = normalDecay + (relaxedDecay - normalDecay) * tailBlend;
        modalAmplitude[modeIndex] *= effectiveDecay * modalReleaseDecay;
    }

    if (std::abs (pickTransient) > 0.000001f)
    {
        modalOutput += pickTransient;
        pickTransient *= pickTransientDecay;
    }

    const auto attackRampSamples = juce::jmax (1.0f, static_cast<float> (sampleRate) * attackRampSeconds);
    modalOutput *= juce::jlimit (0.0f, 1.0f, static_cast<float> (samplesSinceStart) / attackRampSamples);

    ++samplesSinceStart;
    energy = 0.9994f * energy + 0.0006f * std::abs (modalOutput);

    const auto energyCutoff = 0.00004f + (0.000008f - 0.00004f) * sustainAmount;

    if (energy < energyCutoff)
    {
        reset();
        return 0.0f;
    }

    return modalOutput * outputGain;
}

float StringVoice::nextNoiseSample() noexcept
{
    randomState = randomState * 1664525u + 1013904223u;
    const auto value = static_cast<float> ((randomState >> 8) & 0x00ffffffu) / static_cast<float> (0x00ffffffu);
    return 2.0f * value - 1.0f;
}

void StringVoice::updateDamping() noexcept
{
    baseDamping = 0.9965f;
    releaseDamping = 0.992f;
    damping = releasing ? releaseDamping : baseDamping;
}

void StringVoice::startLeftHandRelease() noexcept
{
    const auto heldSeconds = static_cast<float> (samplesSinceStart) / static_cast<float> (sampleRate);

    if (heldSeconds < 0.12f)
    {
        leftHandDampingTarget = 0.55f;
        modalReleaseDecay = 0.9935f;
        const auto transitionSamples = juce::jmax (1.0f, static_cast<float> (sampleRate) * 0.008f);
        leftHandDampingStep = (leftHandDamping - leftHandDampingTarget) / transitionSamples;
        energy *= 0.35f;
        return;
    }

    if (heldSeconds < 0.45f)
    {
        const auto blend = (heldSeconds - 0.12f) / 0.33f;
        leftHandDampingTarget = juce::jmap (blend, 0.55f, 0.82f);
        modalReleaseDecay = juce::jmap (blend, 0.9955f, 0.9988f);
        const auto transitionSeconds = juce::jmap (blend, 0.012f, 0.035f);
        const auto transitionSamples = juce::jmax (1.0f, static_cast<float> (sampleRate) * transitionSeconds);
        leftHandDampingStep = (leftHandDamping - leftHandDampingTarget) / transitionSamples;
        energy *= juce::jmap (blend, 0.45f, 0.75f);
        return;
    }

    leftHandDampingTarget = 0.90f;
    modalReleaseDecay = 0.99935f;
    const auto transitionSamples = juce::jmax (1.0f, static_cast<float> (sampleRate) * 0.06f);
    leftHandDampingStep = (leftHandDamping - leftHandDampingTarget) / transitionSamples;
}

float StringVoice::pluckShapeAt (float position, float pluckPosition) const noexcept
{
    const auto left = position < pluckPosition ? position / pluckPosition
                                               : (1.0f - position) / (1.0f - pluckPosition);

    const auto centered = juce::jlimit (0.0f, 1.0f, left);
    return 2.0f * centered - 1.0f;
}

float StringVoice::readDelayLineAtOffset (int offset) const noexcept
{
    const auto index = (writeIndex + delayLength - juce::jlimit (0, delayLength - 1, offset)) % delayLength;
    return delayLine[static_cast<size_t> (index)];
}

float StringVoice::readSecondaryDelayLineAtOffset (int offset) const noexcept
{
    const auto index = (writeIndex + delayLength - juce::jlimit (0, delayLength - 1, offset)) % delayLength;
    return secondaryDelayLine[static_cast<size_t> (index)];
}

void StringVoice::configureResonator (int index, float frequency, float radius) noexcept
{
    const auto clampedIndex = static_cast<size_t> (juce::jlimit (0, resonanceCount - 1, index));
    const auto clampedFrequency = juce::jlimit (20.0f, static_cast<float> (sampleRate * 0.43), frequency);
    const auto clampedRadius = juce::jlimit (0.80f, 0.999f, radius);
    constexpr auto twoPi = 6.28318530717958647692f;

    resonanceCoefficient[clampedIndex] = 2.0f * clampedRadius * std::cos (twoPi * clampedFrequency / static_cast<float> (sampleRate));
    resonanceRadiusSquared[clampedIndex] = clampedRadius * clampedRadius;
}

void StringVoice::updateHighFrequencyFeedback() noexcept
{
    if (highFeedbackGainSamplesRemaining > 0)
    {
        highFeedbackGain += highFeedbackGainStep;
        secondaryHighFeedbackGain += secondaryHighFeedbackGainStep;
        --highFeedbackGainSamplesRemaining;
    }
    else
    {
        highFeedbackGain = highFeedbackGainTarget;
        secondaryHighFeedbackGain = secondaryHighFeedbackGainTarget;
    }
}

float StringVoice::applyVelocityCurve (float velocity) const noexcept
{
    const auto input = juce::jlimit (0.0f, 1.0f, velocity);

    if (input <= 0.10f)
        return juce::jlimit (0.05f, 1.0f, 0.05f + input * 2.50f);

    if (input <= 0.90f)
        return 0.30f + (input - 0.10f) * (0.50f / 0.80f);

    return 0.80f + (input - 0.90f) * 2.0f;
}

float StringVoice::processHarmonicDamping (float input, float& state, float highGain, float splitCoefficient) noexcept
{
    state += splitCoefficient * (input - state);

    const auto lowComponent = state;
    const auto highComponent = input - lowComponent;
    return lowComponent + highComponent * highGain;
}

float StringVoice::processMovingResonance (float input) noexcept
{
    const auto movement = juce::jlimit (0.0f, 1.0f, static_cast<float> (samplesSinceStart) / static_cast<float> (resonanceMoveSamples));
    const auto highWeight = (1.0f - movement) * (1.0f - movement);
    const auto midWeight = 0.45f + 0.35f * (1.0f - std::abs (2.0f * movement - 1.0f));
    const auto lowWeight = 0.25f + 0.75f * movement;
    const std::array<float, resonanceCount> weights { lowWeight, midWeight, highWeight };
    auto output = 0.0f;

    for (auto i = 0; i < resonanceCount; ++i)
    {
        const auto next = input + resonanceCoefficient[i] * resonanceState1[i] - resonanceRadiusSquared[i] * resonanceState2[i];
        const auto band = next - resonanceState2[i];
        resonanceState2[i] = resonanceState1[i];
        resonanceState1[i] = next;
        output += band * weights[static_cast<size_t> (i)];
    }

    output *= resonanceEnvelope;
    resonanceEnvelope *= resonanceDecay;
    return output;
}

float StringVoice::softClip (float value) const noexcept
{
    return std::tanh (value);
}

void StringVoice::configureMode (int index, float frequency, float amplitude, float decay, float phase, float tailDampingScale) noexcept
{
    const auto clampedIndex = static_cast<size_t> (juce::jlimit (0, modalCount - 1, index));
    const auto clampedFrequency = juce::jlimit (20.0f, static_cast<float> (sampleRate * 0.45), frequency);
    const auto step = 2.0f * 3.14159265358979323846f * clampedFrequency / static_cast<float> (sampleRate);

    modalSine[clampedIndex] = std::sin (phase);
    modalCosine[clampedIndex] = std::cos (phase);
    modalSinStep[clampedIndex] = std::sin (step);
    modalCosStep[clampedIndex] = std::cos (step);
    modalAmplitude[clampedIndex] = amplitude;
    modalDecay[clampedIndex] = juce::jlimit (0.90f, 0.999999f, decay);
    modalTailDampingScale[clampedIndex] = juce::jlimit (0.05f, 1.0f, tailDampingScale);
}

} // namespace guitar_ag
