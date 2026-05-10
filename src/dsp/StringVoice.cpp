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
    modalPhaseStep.fill (0.0f);
    modalAmplitude.fill (0.0f);
    modalDecay.fill (1.0f);
    modalTailDampingScale.fill (1.0f);
    modalModePosition.fill (0.0f);
    modalHighWeight.fill (0.0f);
    modalLowWeight.fill (1.0f);
    modalPalmWeight.fill (0.78f);
    modalFrequency.fill (0.0f);
    modalFeedbackWeight.fill (0.0f);
    modalLoopWeight.fill (0.0f);

    delayLength = 1;
    writeIndex = 0;
    pickupOffsetSamples = 1;
    secondaryPickupOffsetSamples = 1;
    pickupApertureSamples = 1;
    samplesSinceStart = 0;
    activeModalCount = 0;
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
    pickContactRing = 0.0f;
    pickContactRingDecay = 0.0f;
    pickContactPhase = 0.0f;
    pickContactPhaseStep = 0.0f;
    pickGrindAmount = 0.0f;
    pickGrindDecay = 0.0f;
    pickGrindPhase = 0.0f;
    pickGrindPhaseStep = 0.0f;
    pickSlipImpulse = 0.0f;
    pickSlipDecay = 0.0f;
    pickSlipCountdown = 0;
    pickCoinAmount = 0.0f;
    pickCoinDecay = 0.0f;
    pickCoinPhase = 0.0f;
    pickCoinPhaseStep = 0.0f;
    pickCoinImpulse = 0.0f;
    pickCoinImpulseDecay = 0.0f;
    pickCoinCountdown = 0;
    pickHeavyAmount = 0.0f;
    pickHeavyDecay = 0.0f;
    pickHeavyPhase = 0.0f;
    pickHeavyPhaseStep = 0.0f;
    pickHeavyState = 0.0f;
    pickHeavyRaspState = 0.0f;
    pickHeavyBodyState = 0.0f;
    pickHeavyChoke = 0.0f;
    pickContactSamplesRemaining = 0;
    fingerImpact = 0.0f;
    fingerImpactDecay = 0.0f;
    fingerImpactPhase = 0.0f;
    fingerImpactPhaseStep = 0.0f;
    pullOffSnap = 0.0f;
    pullOffSnapDecay = 0.0f;
    previousGestureNoise = 0.0f;
    slideFretContact = 0.0f;
    slideFretContactDecay = std::pow (0.001f, 1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate * 0.018f)));
    slideFretScrape = 0.0f;
    slideFretScrapeDecay = std::pow (0.001f, 1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate * 0.024f)));
    slideFretContactPhase = 0.0f;
    slideFretContactPhaseStep = 0.0f;
    previousSlideFretNoise = 0.0f;
    slideFretScrapeState = 0.0f;
    slideFretBodyState = 0.0f;
    slideFretImpulse = 0.0f;
    slideFretImpulseDecay = std::pow (0.001f, 1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate * 0.0065f)));
    slideFretMotionDrive = 0.0f;
    slideFretMotionDriveDecay = std::pow (0.001f, 1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate * 0.026f)));
    slideFretSpeed = 0.0f;
    slideFretSlipCountdown = 0;
    previousNeckSlideSemitones = 0.0f;
    slideMotionActivity = 0.0f;
    slideMotionActivityDecay = std::pow (0.001f,
                                         static_cast<float> (pitchControlUpdateInterval)
                                             / juce::jmax (1.0f, static_cast<float> (sampleRate * 0.95f)));
    slideLiftEnvelope = 0.0f;
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
    vibratoPhase = 0.0f;
    aftertouchPressure = 0.0f;
    aftertouchPressureTarget = 0.0f;
    mpePitchBend = 0.0f;
    mpePitchBendTarget = 0.0f;
    cachedPitchRatio = 1.0f;
    cachedFeedbackDrive = 0.0f;
    cachedFeedbackHowl = 0.0f;
    mpePressure = 0.0f;
    mpePressureTarget = 0.0f;
    mpeTimbre = 0.0f;
    mpeTimbreTarget = 0.0f;
    pitchControlSamplesUntilUpdate = 0;
    feedbackControlSamplesUntilUpdate = 0;
    previousSlideFret = 0;
    active = false;
    releasing = false;
    woundString = false;
    useCachedPitchSteps = false;
    slideFretStateInitialized = false;
}

void StringVoice::start (int midiNoteNumber,
                         int midiChannel,
                         float velocity,
                         const FretboardAssignment& assignment,
                         float pickStiffness,
                         float pickTexture,
                         float harmonicTouch,
                         float stringAge,
                         float bridgeIntonation,
                         float fretPressure,
                         float pickupPositionControl,
                         int pickupModel,
                         PlayerGesture gesture)
{
    const auto harmonicTouchAmount = juce::jlimit (0.0f, 1.0f, harmonicTouch);
    const auto stringAgeAmount = juce::jlimit (0.0f, 1.0f, stringAge);
    const auto intonationRatio = getBridgeIntonationRatio (assignment, bridgeIntonation);
    const auto fretPressureRatio = getFretPressureRatio (assignment, fretPressure);
    const auto pickupAmount = juce::jlimit (0.0f, 1.0f, pickupPositionControl);
    const auto pickupModelIndex = juce::jlimit (0, 2, pickupModel);
    const auto harmonicActive = harmonicTouchAmount > 0.25f;
    const auto inputVelocity = juce::jlimit (0.0f, 1.0f, velocity);
    auto gestureEnergyScale = 1.0f;
    auto gestureDisplacementScale = 1.0f;
    auto gestureHorizontalScale = 1.0f;
    auto gestureSteelScale = 1.0f;
    auto gestureModalScale = 1.0f;
    auto gesturePickLayerScale = 1.0f;
    auto gestureStiffness = juce::jlimit (0.0f, 1.0f, pickStiffness);
    auto gestureTexture = juce::jlimit (0.0f, 1.0f, pickTexture);
    auto gestureAttackRampSeconds = -1.0f;
    auto gestureImpactAmount = 0.0f;
    auto gestureImpactDecaySeconds = 0.026f;
    auto gestureImpactFrequencyMultiplier = 15.0f;
    auto gesturePullOffAmount = 0.0f;

    if (gesture == PlayerGesture::HammerOn)
    {
        gestureEnergyScale = 0.28f + 0.34f * std::pow (inputVelocity, 0.75f);
        gestureDisplacementScale = 0.62f;
        gestureHorizontalScale = 0.58f;
        gestureSteelScale = 0.52f;
        gestureModalScale = 0.86f;
        gesturePickLayerScale = 0.0f;
        gestureStiffness = 0.58f;
        gestureTexture = 0.0f;
        gestureAttackRampSeconds = 0.0038f;
        gestureImpactAmount = 0.010f + 0.026f * inputVelocity;
        gestureImpactDecaySeconds = 0.024f;
        gestureImpactFrequencyMultiplier = 11.0f + 0.20f * static_cast<float> (assignment.fret);
    }
    else if (gesture == PlayerGesture::PullOff)
    {
        gestureEnergyScale = 0.22f + 0.36f * std::pow (inputVelocity, 0.65f);
        gestureDisplacementScale = 1.06f;
        gestureHorizontalScale = 1.36f;
        gestureSteelScale = 0.38f;
        gestureModalScale = 0.92f;
        gesturePickLayerScale = 0.0f;
        gestureStiffness = 0.44f;
        gestureTexture = 0.0f;
        gestureAttackRampSeconds = 0.0048f;
        gestureImpactAmount = 0.004f + 0.012f * inputVelocity;
        gestureImpactDecaySeconds = 0.020f;
        gestureImpactFrequencyMultiplier = 7.5f + 0.12f * static_cast<float> (assignment.fret);
        gesturePullOffAmount = 0.015f + 0.050f * inputVelocity;
    }
    else if (gesture == PlayerGesture::RightHandTap)
    {
        gestureEnergyScale = 0.42f + 0.40f * std::pow (inputVelocity, 0.65f);
        gestureDisplacementScale = 0.72f;
        gestureHorizontalScale = 0.76f;
        gestureSteelScale = 0.70f;
        gestureModalScale = 0.92f;
        gesturePickLayerScale = 0.0f;
        gestureStiffness = 0.82f;
        gestureTexture = 0.0f;
        gestureAttackRampSeconds = 0.0017f;
        gestureImpactAmount = 0.020f + 0.054f * inputVelocity;
        gestureImpactDecaySeconds = 0.018f;
        gestureImpactFrequencyMultiplier = 15.0f + 0.30f * static_cast<float> (assignment.fret);
    }

    const auto stiffnessAmount = harmonicActive ? 0.0f : gestureStiffness;
    const auto textureAmount = harmonicActive ? 0.0f : gestureTexture;
    const auto stiffnessBipolar = 2.0f * stiffnessAmount - 1.0f;
    const auto mappedTexture = juce::jlimit (0.0f, 1.0f, textureAmount / 0.8f);
    const auto coinTexture = juce::jlimit (0.0f, 1.0f, (textureAmount - 0.8f) * 5.0f);
    const auto heavyCoinTexture = juce::jlimit (0.0f, 1.0f, (textureAmount - 0.95f) * 20.0f);
    const auto activeCoinTexture = coinTexture * (1.0f - 0.18f * heavyCoinTexture);
    const auto highTexture = juce::jlimit (0.0f, 1.0f, (mappedTexture - 0.5f) * 2.0f);
    const auto textureScale = mappedTexture <= 0.5f ? mappedTexture * 2.0f
                                                     : 1.0f + (mappedTexture - 0.5f) * 0.45f;
    const auto pickEdgeScale = stiffnessAmount <= 0.5f ? 0.18f + stiffnessAmount * 1.64f
                                                       : 1.0f + (stiffnessAmount - 0.5f) * 0.84f;
    const auto partialStiffnessScale = stiffnessAmount <= 0.5f ? 0.48f + stiffnessAmount * 1.04f
                                                               : 1.0f + (stiffnessAmount - 0.5f) * 0.70f;

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
    pickContactRing = 0.0f;
    pickContactRingDecay = 0.0f;
    pickContactPhase = 0.0f;
    pickContactPhaseStep = 0.0f;
    pickGrindAmount = 0.0f;
    pickGrindDecay = 0.0f;
    pickGrindPhase = 0.0f;
    pickGrindPhaseStep = 0.0f;
    pickSlipImpulse = 0.0f;
    pickSlipDecay = 0.0f;
    pickSlipCountdown = 0;
    pickCoinAmount = 0.0f;
    pickCoinDecay = 0.0f;
    pickCoinPhase = 0.0f;
    pickCoinPhaseStep = 0.0f;
    pickCoinImpulse = 0.0f;
    pickCoinImpulseDecay = 0.0f;
    pickCoinCountdown = 0;
    pickHeavyAmount = 0.0f;
    pickHeavyDecay = 0.0f;
    pickHeavyPhase = 0.0f;
    pickHeavyPhaseStep = 0.0f;
    pickHeavyState = 0.0f;
    pickHeavyRaspState = 0.0f;
    pickHeavyBodyState = 0.0f;
    pickHeavyChoke = 0.0f;
    pickContactSamplesRemaining = 0;
    fingerImpact = 0.0f;
    fingerImpactDecay = 0.0f;
    fingerImpactPhase = 0.0f;
    fingerImpactPhaseStep = 0.0f;
    pullOffSnap = 0.0f;
    pullOffSnapDecay = 0.0f;
    previousGestureNoise = 0.0f;
    slideFretContact = 0.0f;
    slideFretContactDecay = std::pow (0.001f, 1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate * 0.018f)));
    slideFretScrape = 0.0f;
    slideFretScrapeDecay = std::pow (0.001f, 1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate * 0.024f)));
    slideFretContactPhase = 0.0f;
    slideFretContactPhaseStep = 0.0f;
    previousSlideFretNoise = 0.0f;
    slideFretScrapeState = 0.0f;
    slideFretBodyState = 0.0f;
    slideFretImpulse = 0.0f;
    slideFretImpulseDecay = std::pow (0.001f, 1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate * 0.0065f)));
    slideFretMotionDrive = 0.0f;
    slideFretMotionDriveDecay = std::pow (0.001f, 1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate * 0.026f)));
    slideFretSpeed = 0.0f;
    slideFretSlipCountdown = 0;
    previousNeckSlideSemitones = 0.0f;
    slideMotionActivity = 0.0f;
    slideMotionActivityDecay = std::pow (0.001f,
                                         static_cast<float> (pitchControlUpdateInterval)
                                             / juce::jmax (1.0f, static_cast<float> (sampleRate * 0.95f)));
    slideLiftEnvelope = 0.0f;
    attackRampSeconds = gestureAttackRampSeconds > 0.0f
                        ? gestureAttackRampSeconds
                        : juce::jmap (stiffnessAmount, 0.0090f, 0.0013f);
    modalSine.fill (0.0f);
    modalCosine.fill (1.0f);
    modalSinStep.fill (0.0f);
    modalCosStep.fill (1.0f);
    modalPitchSinStep.fill (0.0f);
    modalPitchCosStep.fill (1.0f);
    modalPhaseStep.fill (0.0f);
    modalAmplitude.fill (0.0f);
    modalDecay.fill (1.0f);
    modalTailDampingScale.fill (1.0f);
    modalModePosition.fill (0.0f);
    modalHighWeight.fill (0.0f);
    modalLowWeight.fill (1.0f);
    modalPalmWeight.fill (0.78f);
    modalFrequency.fill (0.0f);
    modalFeedbackWeight.fill (0.0f);
    modalLoopWeight.fill (0.0f);
    activeModalCount = 0;
    modalReleaseDecay = 1.0f;
    resonanceState1.fill (0.0f);
    resonanceState2.fill (0.0f);
    dampingTiltState = 0.0f;
    secondaryDampingTiltState = 0.0f;
    leftHandDamping = 1.0f;
    leftHandDampingTarget = 1.0f;
    leftHandDampingStep = 0.0f;
    vibratoPhase = 0.0f;
    aftertouchPressure = 0.0f;
    aftertouchPressureTarget = 0.0f;
    mpePitchBend = 0.0f;
    mpePitchBendTarget = 0.0f;
    cachedPitchRatio = 1.0f;
    cachedFeedbackDrive = 0.0f;
    cachedFeedbackHowl = 0.0f;
    mpePressure = 0.0f;
    mpePressureTarget = 0.0f;
    mpeTimbre = 0.0f;
    mpeTimbreTarget = 0.0f;
    pitchControlSamplesUntilUpdate = 0;
    feedbackControlSamplesUntilUpdate = 0;
    previousSlideFret = 0;
    releasing = false;
    active = true;
    woundString = woundAmount > 0.0f;
    useCachedPitchSteps = false;
    slideFretStateInitialized = false;

    constexpr auto twoPi = 6.28318530717958647692f;
    const auto frequency = juce::jlimit (20.0,
                                         8000.0,
                                         juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber)
                                             * static_cast<double> (intonationRatio * fretPressureRatio));
    slideFretContactPhase = nextNoiseSample() * twoPi;
    slideFretContactPhaseStep = twoPi
                              * juce::jlimit (700.0f,
                                               static_cast<float> (sampleRate * 0.40),
                                               static_cast<float> (frequency) * (12.0f + 5.0f * woundAmount))
                              / static_cast<float> (sampleRate);
    delayLength = juce::jlimit (2, maxDelaySamples, static_cast<int> (std::round (sampleRate / frequency)));
    pickupOffsetSamples = juce::jlimit (1, delayLength - 1, static_cast<int> (std::round (delayLength * 0.18f)));
    secondaryPickupOffsetSamples = juce::jlimit (1, delayLength - 1, static_cast<int> (std::round (delayLength * 0.205f)));
    pickupApertureSamples = juce::jlimit (1, delayLength / 9, static_cast<int> (std::round (delayLength * 0.012f)));

    const auto velocityGain = applyVelocityCurve (velocity) * gestureEnergyScale;
    const auto strikeVelocity = 0.05f + (velocityGain - 0.05f) * (0.63f / 0.95f);
    const auto velocityNormal = juce::jlimit (0.0f, 1.0f, (strikeVelocity - 0.05f) / 0.95f);
    const auto strikeInput = juce::jlimit (0.0f, 1.0f, velocityNormal * 1.55f);
    const auto strikeAmount = std::pow (strikeInput, 0.58f);
    const auto hardStrike = std::pow (juce::jlimit (0.0f, 1.0f, (velocityNormal - 0.46f) / 0.54f), 0.55f);
    const auto brightness = strikeAmount;
    const auto pluckPosition = juce::jmap (strikeAmount, 0.225f, 0.080f);
    constexpr auto pickupPositionMin = 0.055f;
    constexpr auto pickupPositionMax = 0.335f;
    constexpr auto humbuckerCoilSeparation = 0.046f;
    constexpr auto outOfPhaseSingleCoilSpacing = 0.086f;
    const auto pickupPosition = pickupModelIndex == 2
                              ? juce::jmap (pickupAmount,
                                             pickupPositionMin + outOfPhaseSingleCoilSpacing * 0.5f,
                                             pickupPositionMax - outOfPhaseSingleCoilSpacing * 0.5f)
                              : juce::jmap (pickupAmount, pickupPositionMin, pickupPositionMax);
    const auto pickupWidth = pickupModelIndex == 1 ? 0.056f : 0.024f;
    const auto outOfPhaseBridgePosition = pickupPosition - outOfPhaseSingleCoilSpacing * 0.5f;
    const auto outOfPhaseNeckPosition = pickupPosition + outOfPhaseSingleCoilSpacing * 0.5f;
    const auto displacementAmount = 0.70f * velocityGain * gestureDisplacementScale;
    const auto horizontalAmount = (0.28f + 0.06f * woundAmount) * velocityGain * gestureHorizontalScale;
    const auto ageBrightnessScale = 1.06f - 0.46f * stringAgeAmount;
    const auto ageContactScale = 1.04f - 0.38f * stringAgeAmount;
    const auto noiseAmount = (0.004f + 0.014f * brightness) * (1.0f + 0.2f * woundAmount)
                           * textureScale * ageContactScale * gesturePickLayerScale;
    const auto steelPartialAmount = (0.012f + 0.032f * brightness) * velocityGain
                                  * partialStiffnessScale * ageBrightnessScale * gestureSteelScale;
    auto mean = 0.0f;
    auto secondaryMean = 0.0f;
    auto harmonicDivision = 1;
    auto harmonicBandPosition = 0.0f;

    if (harmonicTouchAmount > 0.75f)
    {
        harmonicDivision = 2;
        harmonicBandPosition = (harmonicTouchAmount - 0.75f) * 4.0f;
    }
    else if (harmonicTouchAmount > 0.50f)
    {
        harmonicDivision = 3;
        harmonicBandPosition = (harmonicTouchAmount - 0.50f) * 4.0f;
    }
    else if (harmonicTouchAmount > 0.25f)
    {
        harmonicDivision = 4;
        harmonicBandPosition = (harmonicTouchAmount - 0.25f) * 4.0f;
    }

    const auto harmonicAccuracy = harmonicDivision > 1
                                ? std::pow (juce::jlimit (0.0f, 1.0f, harmonicBandPosition), 0.70f)
                                : 0.0f;
    const auto harmonicEnergyScale = harmonicDivision == 2 ? 0.94f
                                  : harmonicDivision == 3 ? 0.74f
                                  : harmonicDivision == 4 ? 0.58f
                                                          : 1.0f;

    randomState = static_cast<uint32_t> ((midiNoteNumber + 1) * 1103515245u + (midiChannel + 17) * 12345u);

    for (auto i = 0; i < delayLength; ++i)
    {
        const auto x = static_cast<float> (i) / static_cast<float> (delayLength);
        const auto shape = pluckShapeAt (x, pluckPosition);
        const auto horizontalShape = pluckShapeAt (std::fmod (x + 0.017f, 1.0f), pluckPosition);
        const auto pickDistance = std::abs (x - pluckPosition);
        const auto localPickContact = std::exp (-pickDistance * static_cast<float> (delayLength) * (0.055f + 0.190f * stiffnessAmount));
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
                                   + horizontalPartials * steelPartialAmount * (0.42f + 0.46f * mappedTexture)
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

    energy = velocityGain * harmonicEnergyScale;
    pickTransient = (0.004f + 0.014f * brightness) * pickEdgeScale * ageContactScale
                  * gesturePickLayerScale * (nextNoiseSample() >= 0.0f ? 1.0f : -1.0f);
    pickTransientDecay = 0.9990f - 0.0015f * brightness - 0.00025f * stiffnessBipolar - 0.00020f * stringAgeAmount;
    const auto smoothTexture = mappedTexture <= 0.5f ? std::pow (mappedTexture * 2.0f, 1.35f)
                                                     : 1.0f - 0.42f * highTexture;
    const auto grindTexture = std::pow (highTexture, 1.35f);
    pickContact = (0.006f + 0.050f * brightness) * velocityGain * smoothTexture
                * ageContactScale * gesturePickLayerScale;
    pickContactDecay = 0.9987f - 0.00020f * brightness + 0.00038f * mappedTexture - 0.00025f * stringAgeAmount;
    pickContactRing = (0.002f + 0.025f * brightness) * velocityGain * smoothTexture
                    * ageBrightnessScale * gesturePickLayerScale;
    pickContactRingDecay = 0.9986f + 0.00040f * mappedTexture - 0.00035f * stringAgeAmount;
    pickContactPhase = nextNoiseSample() * twoPi;
    pickContactPhaseStep = twoPi * juce::jlimit (1200.0f,
                                                  static_cast<float> (sampleRate * 0.42),
                                                  static_cast<float> (frequency) * (11.0f + 10.0f * mappedTexture + 3.0f * woundAmount))
                         / static_cast<float> (sampleRate);
    pickGrindAmount = (0.018f + 0.115f * brightness) * velocityGain * grindTexture * gesturePickLayerScale;
    pickGrindDecay = 0.99925f + 0.00045f * highTexture;
    pickGrindPhase = nextNoiseSample() * twoPi;
    pickGrindPhaseStep = twoPi * juce::jlimit (900.0f,
                                                static_cast<float> (sampleRate * 0.38),
                                                static_cast<float> (frequency) * (8.0f + 15.0f * highTexture + 4.0f * woundAmount))
                       / static_cast<float> (sampleRate);
    pickSlipImpulse = 0.0f;
    pickSlipDecay = 0.92f + 0.065f * highTexture;
    pickSlipCountdown = 0;
    pickCoinAmount = (0.020f + 0.125f * brightness) * velocityGain
                   * std::pow (activeCoinTexture, 1.25f) * gesturePickLayerScale;
    pickCoinDecay = 0.99945f + 0.00030f * activeCoinTexture;
    pickCoinPhase = nextNoiseSample() * twoPi;
    pickCoinPhaseStep = twoPi * juce::jlimit (1400.0f,
                                               static_cast<float> (sampleRate * 0.40),
                                               static_cast<float> (frequency) * (18.0f + 20.0f * activeCoinTexture + 5.0f * woundAmount))
                      / static_cast<float> (sampleRate);
    pickCoinImpulse = 0.0f;
    pickCoinImpulseDecay = 0.965f + 0.030f * activeCoinTexture;
    pickCoinCountdown = 0;
    pickHeavyAmount = (0.035f + 0.165f * brightness) * velocityGain
                    * std::pow (heavyCoinTexture, 1.08f) * gesturePickLayerScale;
    pickHeavyDecay = 0.99972f + 0.00022f * heavyCoinTexture;
    pickHeavyPhase = nextNoiseSample() * twoPi;
    pickHeavyPhaseStep = twoPi * juce::jlimit (180.0f,
                                                static_cast<float> (sampleRate * 0.16),
                                                static_cast<float> (frequency) * (3.8f + 6.2f * woundAmount + 2.4f * brightness))
                       / static_cast<float> (sampleRate);
    pickHeavyState = 0.0f;
    pickHeavyRaspState = 0.0f;
    pickHeavyBodyState = 0.0f;
    pickHeavyChoke = 0.08f * heavyCoinTexture;
    pickContactSamplesRemaining = textureAmount <= 0.0f || gesturePickLayerScale <= 0.0f
                                ? 0
                                : static_cast<int> (sampleRate * (0.010f + 0.040f * mappedTexture + 0.055f * highTexture + 0.085f * coinTexture + 0.075f * heavyCoinTexture + 0.010f * brightness));
    fingerImpact = gestureImpactAmount * gestureEnergyScale * ageContactScale;

    if (fingerImpact > 0.0f)
    {
        fingerImpactDecay = std::pow (0.001f,
                                      1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate * gestureImpactDecaySeconds)));
        fingerImpactPhase = nextNoiseSample() * twoPi;
        fingerImpactPhaseStep = twoPi * juce::jlimit (700.0f,
                                                       static_cast<float> (sampleRate * 0.42),
                                                       static_cast<float> (frequency)
                                                           * gestureImpactFrequencyMultiplier)
                              / static_cast<float> (sampleRate);
    }

    pullOffSnap = gesturePullOffAmount * gestureEnergyScale * (0.72f + 0.30f * woundAmount) * ageContactScale;
    pullOffSnapDecay = std::pow (0.001f, 1.0f / juce::jmax (1.0f, static_cast<float> (sampleRate * 0.032f)));
    configureResonator (0, frequency * 5.0f, 0.9895f);
    configureResonator (1, frequency * 7.0f, 0.9880f);
    configureResonator (2, frequency * 11.0f, 0.9860f);
    resonanceEnvelope = (0.012f + 0.045f * brightness) * velocityGain * (1.04f - 0.34f * stringAgeAmount);
    resonanceDecay = 0.99976f - 0.00008f * brightness - 0.00020f * stringAgeAmount;
    resonanceMoveSamples = juce::jmax (1, static_cast<int> (sampleRate * (0.36f + 0.16f * brightness)));
    highFeedbackGain = 0.9995f;
    highFeedbackGainTarget = 0.9935f - 0.0015f * brightness - 0.0018f * stringAgeAmount;
    highFeedbackGainSamplesRemaining = juce::jmax (1, static_cast<int> (sampleRate * (0.55f + 0.25f * brightness)));
    highFeedbackGainStep = (highFeedbackGainTarget - highFeedbackGain)
                         / juce::jmax (1.0f, static_cast<float> (highFeedbackGainSamplesRemaining));
    secondaryHighFeedbackGain = 0.9992f;
    secondaryHighFeedbackGainTarget = 0.9955f - 0.0010f * brightness + 0.0010f * woundAmount - 0.0014f * stringAgeAmount;
    secondaryHighFeedbackGainStep = (secondaryHighFeedbackGainTarget - secondaryHighFeedbackGain)
                                  / juce::jmax (1.0f, static_cast<float> (highFeedbackGainSamplesRemaining));

    auto modeIndex = 0;
    const auto stiffness = 0.000045f + (0.000080f - 0.000045f) * woundAmount;
    const auto modalGain = (0.020f + 0.070f * std::pow (velocityNormal, 0.62f))
                         * (0.70f + 0.85f * velocityGain) * gestureModalScale;
    const auto contactWidth = juce::jmap (strikeAmount, 0.070f, 0.006f) * (1.0f - 0.60f * stiffnessBipolar);
    const auto attackModeGain = (juce::jmap (strikeAmount, 0.008f, 0.075f) + 0.045f * hardStrike)
                              * pickEdgeScale
                              * (0.82f + 0.36f * mappedTexture);

    for (auto harmonic = 1; harmonic <= 32 && modeIndex < modalCount; ++harmonic)
    {
        const auto harmonicFloat = static_cast<float> (harmonic);
        const auto stiffFrequency = static_cast<float> (frequency) * harmonicFloat
                                  * std::sqrt (1.0f + stiffness * harmonicFloat * harmonicFloat);

        if (stiffFrequency >= sampleRate * 0.43)
            break;

        const auto pluckShape = std::sin (twoPi * 0.5f * harmonicFloat * pluckPosition);
        const auto pickupShape = std::sin (twoPi * 0.5f * harmonicFloat * pickupPosition);
        const auto humbuckerCoilA = std::sin (twoPi * 0.5f * harmonicFloat
                                           * juce::jlimit (0.020f,
                                                           0.380f,
                                                           pickupPosition - humbuckerCoilSeparation * 0.5f));
        const auto humbuckerCoilB = std::sin (twoPi * 0.5f * harmonicFloat
                                           * juce::jlimit (0.020f,
                                                           0.380f,
                                                           pickupPosition + humbuckerCoilSeparation * 0.5f));
        const auto outOfPhaseBridgeCoil = std::sin (twoPi * 0.5f * harmonicFloat * outOfPhaseBridgePosition);
        const auto outOfPhaseNeckCoil = std::sin (twoPi * 0.5f * harmonicFloat * outOfPhaseNeckPosition);
        const auto pickupModelShape = pickupModelIndex == 0 ? pickupShape
                                  : pickupModelIndex == 1 ? 0.5f * (humbuckerCoilA + humbuckerCoilB) * 2.05f
                                                          : (outOfPhaseNeckCoil - outOfPhaseBridgeCoil) * 1.35f;
        const auto pickupElectricalTilt = pickupModelIndex == 1 ? std::exp (-0.010f * harmonicFloat)
                                                                : 1.0f;
        const auto aperture = std::abs (harmonicFloat * pickupWidth) < 0.0001f
                            ? 1.0f
                            : std::sin (twoPi * 0.5f * harmonicFloat * pickupWidth)
                                / (twoPi * 0.5f * harmonicFloat * pickupWidth);
        const auto decayBaseSeconds = (6.4f + (8.2f - 6.4f) * woundAmount) * (1.0f - 0.16f * stringAgeAmount);
        const auto decayCurvature = (0.0090f + (0.0065f - 0.0090f) * woundAmount) * (1.0f + 1.55f * stringAgeAmount);
        const auto decaySeconds = decayBaseSeconds / (1.0f + decayCurvature * harmonicFloat * harmonicFloat);
        const auto decay = std::pow (0.001f, 1.0f / static_cast<float> (sampleRate * decaySeconds));
        const auto tiltStart = 1.00f + (0.92f - 1.00f) * woundAmount;
        const auto tiltEnd = 0.48f + (0.34f - 0.48f) * woundAmount;
        const auto tiltExponent = juce::jmap (strikeAmount, tiltStart, tiltEnd);
        const auto contactFilter = std::exp (-contactWidth * harmonicFloat);
        const auto agePartialDamping = std::exp (-0.026f * stringAgeAmount * juce::jmax (0.0f, harmonicFloat - 1.0f));
        const auto partialTilt = contactFilter * agePartialDamping / std::pow (harmonicFloat, tiltExponent);
        const auto velocityScale = juce::jlimit (0.12f, 4.0f, stiffFrequency / 470.0f);
        const auto attackEmphasis = 1.0f
                                  + strikeAmount * juce::jlimit (0.0f, 3.0f, (harmonicFloat - 1.0f) / 8.0f)
                                  + hardStrike * juce::jlimit (0.0f, 4.0f, (harmonicFloat - 4.0f) / 8.0f);
        const auto touchMask = getHarmonicTouchMask (harmonic, harmonicDivision, harmonicAccuracy);
        const auto amplitude = modalGain * pluckShape * pickupModelShape * aperture * pickupElectricalTilt
                             * partialTilt * velocityScale * attackEmphasis * touchMask * harmonicEnergyScale;
        const auto phase = (harmonic % 2 == 0 ? 0.18f : -0.11f) * harmonicFloat;
        const auto tailDampingScale = juce::jlimit (0.14f, 0.62f, 0.11f + 0.012f * harmonicFloat);

        configureMode (modeIndex++, stiffFrequency, amplitude, decay, phase, tailDampingScale);

        if (modeIndex < modalCount && harmonic >= 4)
        {
            const auto sideFrequency = stiffFrequency * (1.0035f + (0.00025f + 0.00045f * woundAmount) * harmonicFloat);
            const auto sideDecay = std::pow (0.001f, 1.0f / static_cast<float> (sampleRate * decaySeconds * (0.95f - 0.15f * stringAgeAmount)));
            const auto plainSideRegime = juce::jmap (strikeAmount, 0.35f, 1.55f) + hardStrike * 0.55f;
            const auto woundSideRegime = juce::jmap (strikeAmount, 0.45f, 2.20f) + hardStrike * 0.95f;
            const auto sideRegime = plainSideRegime + (woundSideRegime - plainSideRegime) * woundAmount;
            const auto sideAmount = 0.045f + ((0.12f + 0.014f * harmonicFloat) - 0.045f) * woundAmount;
            configureMode (modeIndex++,
                           sideFrequency,
                           amplitude * sideRegime * sideAmount * (0.45f + 0.55f * touchMask) * agePartialDamping,
                           sideDecay,
                           phase + 1.7f,
                           juce::jlimit (0.28f, 0.76f, tailDampingScale + 0.14f));
        }

        if (woundAmount > 0.0f && harmonic >= 2 && harmonic <= 18 && modeIndex < modalCount)
        {
            const auto windingFrequency = static_cast<float> (frequency)
                                        * (harmonicFloat + 0.34f + 0.011f * harmonicFloat * harmonicFloat);
            const auto windingDecaySeconds = (0.72f + 0.055f * harmonicFloat) * (1.0f - 0.22f * stringAgeAmount);
            const auto windingDecay = std::pow (0.001f, 1.0f / static_cast<float> (sampleRate * windingDecaySeconds));
            const auto windingAperture = 1.0f / std::sqrt (harmonicFloat);

            configureMode (modeIndex++,
                           windingFrequency,
                           std::abs (amplitude)
                               * (juce::jmap (strikeAmount, 0.35f, 2.50f) + hardStrike * 1.25f)
                               * (0.12f + 0.008f * harmonicFloat)
                               * windingAperture
                               * woundAmount
                               * (0.35f + 0.65f * touchMask)
                               * (0.85f - 0.30f * stringAgeAmount),
                           windingDecay,
                           phase + 2.35f + 0.29f * harmonicFloat,
                           0.65f);
        }

        if (strikeAmount > 0.18f && harmonic >= 4 && harmonic <= 26 && modeIndex < modalCount)
        {
            const auto chirpFrequency = stiffFrequency * (1.030f + 0.0025f * harmonicFloat + 0.010f * hardStrike);
            const auto chirpSeconds = (juce::jmap (strikeAmount, 0.075f, 0.032f)
                                    + 0.0015f * harmonicFloat) * (1.0f - 0.28f * stringAgeAmount);
            const auto chirpDecay = std::pow (0.001f, 1.0f / static_cast<float> (sampleRate * chirpSeconds));

            configureMode (modeIndex++,
                           chirpFrequency,
                           std::abs (amplitude) * attackModeGain * 0.48f * std::pow (harmonicFloat, 0.72f)
                               * (0.35f + 0.65f * touchMask)
                               * (0.90f - 0.35f * stringAgeAmount),
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

void StringVoice::setAftertouchPressure (int midiNoteNumber, int midiChannel, float pressure) noexcept
{
    if (! active || noteNumber != midiNoteNumber || channel != midiChannel)
        return;

    aftertouchPressureTarget = juce::jlimit (0.0f, 1.0f, pressure);
}

void StringVoice::setMpePitchBend (int midiChannel, float bend) noexcept
{
    if (! active || channel != midiChannel)
        return;

    mpePitchBendTarget = juce::jlimit (-1.0f, 1.0f, bend);
}

void StringVoice::setMpePressure (int midiChannel, float pressure) noexcept
{
    if (! active || channel != midiChannel)
        return;

    mpePressureTarget = juce::jlimit (0.0f, 1.0f, pressure);
}

void StringVoice::setMpeTimbre (int midiChannel, float timbre) noexcept
{
    if (! active || channel != midiChannel)
        return;

    mpeTimbreTarget = juce::jlimit (0.0f, 1.0f, timbre);
}

float StringVoice::getFeedbackCouplingScore (float loopFrequency) const noexcept
{
    if (! active || loopFrequency <= 20.0f || activeModalCount <= 0)
        return 0.0f;

    auto bestLock = 0.0f;
    const auto clampedLoopFrequency = juce::jlimit (20.0f, 6000.0f, loopFrequency);

    for (auto mode = 0; mode < activeModalCount; ++mode)
    {
        const auto modeIndex = static_cast<size_t> (mode);
        const auto modeFrequency = modalFrequency[modeIndex];

        if (modeFrequency <= 20.0f)
            continue;

        const auto frequencyRatio = juce::jlimit (0.001f, 1000.0f, modeFrequency / clampedLoopFrequency);
        const auto centsDistance = std::abs (1200.0f * std::log2 (frequencyRatio));
        const auto loopLock = std::exp (-0.5f * (centsDistance / 132.0f) * (centsDistance / 132.0f));
        const auto modeEnergy = std::abs (modalAmplitude[modeIndex]) * (0.22f + 0.78f * modalHighWeight[modeIndex]);
        bestLock = juce::jmax (bestLock, loopLock * (0.0015f + modeEnergy));
    }

    const auto energyGate = juce::jlimit (0.0f, 1.0f, energy / 0.0010f);
    return bestLock * energyGate * (releasing ? 0.62f : 1.0f);
}

float StringVoice::renderSample (float tailSustain,
                                 float palmMute,
                                 float vibratoDepthCents,
                                 float vibratoSpeedHz,
                                 float vibratoDelaySeconds,
                                 float whammySemitones,
                                 float whammySpread,
                                 float ampFeedback,
                                 float feedbackLoopFrequency,
                                 float feedbackLoopAmount,
                                 float feedbackLoopSignal,
                                 int feedbackFocusedString,
                                 float feedbackStringFocus,
                                 float aftertouchBendSemitones,
                                 float mpePressureAmount,
                                 float mpeTimbreAmount,
                                 float mpePitchBendRange,
                                 float neckSlideSemitones,
                                 float slideFretSteps,
                                 float slideLift) noexcept
{
    if (! active)
        return 0.0f;

    const auto sustainAmount = juce::jlimit (0.0f, 1.0f, tailSustain);
    const auto palmAmount = juce::jlimit (0.0f, 1.0f, palmMute);
    const auto mappedPalmAmount = palmAmount <= 0.5f ? palmAmount * 0.25f
                                                     : 0.125f + (palmAmount - 0.5f) * 1.75f;
    const auto palmCurve = std::pow (mappedPalmAmount, 1.35f);
    auto palmDecay = 1.0f;

    if (palmCurve > 0.0001f)
    {
        const auto muteDecaySeconds = juce::jmap (palmCurve, 2.2f, 0.040f);
        palmDecay = std::pow (0.001f, 1.0f / static_cast<float> (sampleRate * muteDecaySeconds));
    }

    if (releasing)
        modalReleaseDecay = juce::jmin (modalReleaseDecay, 0.99935f);

    const auto heldSeconds = static_cast<float> (samplesSinceStart) / static_cast<float> (sampleRate);
    const auto tailBlend = releasing ? 0.0f : sustainAmount * juce::jlimit (0.0f, 1.0f, (heldSeconds - 0.55f) / 1.60f);
    mpePressure += (mpePressureTarget - mpePressure) * 0.0030f;
    mpeTimbre += (mpeTimbreTarget - mpeTimbre) * 0.0035f;
    const auto expressionPressure = juce::jlimit (0.0f, 1.0f, mpePressure * juce::jlimit (0.0f, 1.0f, mpePressureAmount));
    const auto expressionTimbre = juce::jlimit (0.0f, 1.0f, mpeTimbre * juce::jlimit (0.0f, 1.0f, mpeTimbreAmount));
    const auto pitchRatio = updatePitchRatio (heldSeconds,
                                              vibratoDepthCents,
                                              vibratoSpeedHz,
                                              vibratoDelaySeconds,
                                              whammySemitones,
                                              whammySpread,
                                              aftertouchBendSemitones,
                                              mpePitchBendRange,
                                              neckSlideSemitones,
                                              slideFretSteps,
                                              slideLift);

    const auto feedbackAmount = juce::jlimit (0.0f, 1.0f, ampFeedback);
    const auto loopAmount = juce::jlimit (0.0f, 1.0f, feedbackLoopAmount);
    const auto loopFrequency = juce::jlimit (20.0f, 6000.0f, feedbackLoopFrequency);
    const auto loopSignal = juce::jlimit (-1.0f, 1.0f, feedbackLoopSignal);
    const auto stringFocus = juce::jlimit (0.0f, 1.0f, feedbackStringFocus);
    const auto stringIsFocused = feedbackFocusedString >= 0 && stringIndex == feedbackFocusedString;
    const auto loopStringScale = stringFocus <= 0.0001f
                               ? 1.0f
                               : stringIsFocused ? 1.0f + 1.40f * stringFocus
                                                 : 1.0f - 0.88f * stringFocus;
    const auto feedbackHasAmount = feedbackAmount > 0.0001f;
    const auto loopActive = loopAmount > 0.0001f && feedbackLoopFrequency > 20.0f;
    const auto feedbackActive = feedbackHasAmount || loopActive;
    const auto loopInfluence = loopAmount * juce::jlimit (0.0f, 1.0f, (feedbackAmount - 0.34f) / 0.66f);
    const auto localFeedbackScale = (1.0f - (0.70f + 0.22f * stringFocus) * loopInfluence)
                                  * (1.0f - 0.45f * stringFocus);
    const auto feedbackRampSeconds = 1.10f - 0.78f * feedbackAmount;
    const auto feedbackRise = feedbackHasAmount
                            ? juce::jlimit (0.0f,
                                            1.0f,
                                            (heldSeconds - 0.10f) / juce::jmax (0.22f, feedbackRampSeconds))
                            : 0.0f;
    const auto feedbackFrequency = feedbackActive
                                 ? juce::jlimit (20.0f,
                                                 8000.0f,
                                                 static_cast<float> (juce::MidiMessage::getMidiNoteInHertz (noteNumber))
                                                     * pitchRatio)
                                 : 1.0f;
    if (feedbackActive)
    {
        if (feedbackControlSamplesUntilUpdate <= 0)
            updateFeedbackWeightCache (feedbackHasAmount,
                                       loopActive,
                                       feedbackAmount,
                                       loopAmount,
                                       feedbackFrequency,
                                       loopFrequency);

        --feedbackControlSamplesUntilUpdate;
    }
    else
    {
        cachedFeedbackDrive = 0.0f;
        cachedFeedbackHowl = 0.0f;
        feedbackControlSamplesUntilUpdate = 0;
    }

    const FeedbackRenderContext feedback {
        feedbackActive,
        loopActive,
        feedbackRise,
        releasing ? 0.22f + 0.78f * cachedFeedbackHowl : 1.0f,
        juce::jlimit (0.0f, 1.0f, energy / 0.0012f),
        juce::jlimit (0.08f, 1.0f, energy / 0.00075f),
        localFeedbackScale,
        loopAmount,
        loopSignal,
        loopStringScale
    };

    const auto effectiveSlideLift = juce::jlimit (0.0f, 1.0f, slideLiftEnvelope);
    auto modalOutput = renderModalBank (tailBlend, palmDecay, expressionPressure, expressionTimbre, effectiveSlideLift, feedback);
    modalOutput += renderPickTransient();
    const auto contactOutput = renderContactLayer();

    const auto attackRampSamples = juce::jmax (1.0f, static_cast<float> (sampleRate) * attackRampSeconds);
    modalOutput *= juce::jlimit (0.0f, 1.0f, static_cast<float> (samplesSinceStart) / attackRampSamples);
    modalOutput *= 1.0f - 0.22f * pickHeavyChoke;
    modalOutput *= 1.0f - 0.28f * palmCurve;
    modalOutput *= 1.0f - 0.10f * effectiveSlideLift;
    modalOutput *= 1.0f + 0.10f * expressionPressure + 0.06f * expressionTimbre;
    modalOutput += contactOutput;

    ++samplesSinceStart;
    energy = 0.9994f * energy + 0.0006f * std::abs (modalOutput);

    const auto feedbackHold = 1.0f - 0.78f * feedbackRise * cachedFeedbackHowl - 0.58f * loopAmount;
    const auto energyCutoff = (0.00004f + (0.000008f - 0.00004f) * sustainAmount)
                            * juce::jlimit (0.18f, 1.0f, feedbackHold);

    if (energy < energyCutoff)
    {
        reset();
        return 0.0f;
    }

    return modalOutput * outputGain;
}

float StringVoice::renderModalBank (float tailBlend,
                                    float palmDecay,
                                    float expressionPressure,
                                    float expressionTimbre,
                                    float slideLift,
                                    const FeedbackRenderContext& feedback) noexcept
{
    auto modalOutput = 0.0f;
    auto feedbackHowlOutput = 0.0f;

    for (auto mode = 0; mode < activeModalCount; ++mode)
    {
        const auto modeIndex = static_cast<size_t> (mode);
        const auto highMode = modalHighWeight[modeIndex];
        const auto lowMode = modalLowWeight[modeIndex];
        const auto expressionModeGain = juce::jlimit (0.72f,
                                                      1.42f,
                                                      1.0f
                                                          + expressionPressure * (0.045f + 0.13f * highMode)
                                                          + expressionTimbre * (0.24f * highMode - 0.045f * lowMode));
        modalOutput += modalAmplitude[modeIndex] * modalCosine[modeIndex] * expressionModeGain;

        const auto sinStep = useCachedPitchSteps ? modalPitchSinStep[modeIndex] : modalSinStep[modeIndex];
        const auto cosStep = useCachedPitchSteps ? modalPitchCosStep[modeIndex] : modalCosStep[modeIndex];

        const auto nextSine = modalSine[modeIndex] * cosStep + modalCosine[modeIndex] * sinStep;
        const auto nextCosine = modalCosine[modeIndex] * cosStep - modalSine[modeIndex] * sinStep;

        modalSine[modeIndex] = nextSine;
        modalCosine[modeIndex] = nextCosine;

        const auto normalDecay = modalDecay[modeIndex];
        const auto relaxedDecay = 1.0f - (1.0f - normalDecay) * modalTailDampingScale[modeIndex];
        const auto baseEffectiveDecay = normalDecay + (relaxedDecay - normalDecay) * tailBlend;
        const auto pressureDecayLift = expressionPressure * (0.000045f + 0.000120f * highMode);
        const auto timbreDecayTilt = expressionTimbre * (0.000085f * highMode - 0.000030f * lowMode);
        auto feedbackWeight = 0.0f;
        auto loopWeight = 0.0f;

        if (feedback.feedbackActive)
        {
            feedbackWeight = modalFeedbackWeight[modeIndex]
                           * feedback.feedbackRise
                           * feedback.releaseScale
                           * feedback.feedbackEnergyGate
                           * feedback.localFeedbackScale;

            if (feedback.loopActive)
            {
                loopWeight = modalLoopWeight[modeIndex]
                           * feedback.loopAmount
                           * feedback.loopEnergyGate
                           * feedback.releaseScale
                           * (0.62f + 0.38f * std::abs (feedback.loopSignal))
                           * feedback.loopStringScale;
            }
        }

        const auto feedbackDecayLift = cachedFeedbackDrive * feedbackWeight * (0.000010f + 0.000070f * cachedFeedbackHowl)
                                     + loopWeight * (0.000080f + 0.000300f * feedback.loopAmount);
        const auto effectiveDecay = juce::jlimit (0.90f,
                                                  0.999999f,
                                                  baseEffectiveDecay + pressureDecayLift + timbreDecayTilt + feedbackDecayLift);
        const auto effectivePalmDecay = 1.0f + (palmDecay - 1.0f) * modalPalmWeight[modeIndex];
        const auto slideLiftDamping = juce::jlimit (0.9950f,
                                                    1.0f,
                                                    1.0f - slideLift * (0.00022f + 0.00072f * highMode));
        modalAmplitude[modeIndex] *= effectiveDecay * modalReleaseDecay * effectivePalmDecay * slideLiftDamping;

        if (feedbackWeight + loopWeight > 0.000001f)
        {
            const auto amplitudeSign = modalAmplitude[modeIndex] < 0.0f ? -1.0f : 1.0f;
            const auto feedbackInjection = cachedFeedbackDrive
                                         * feedbackWeight
                                         * (0.0000004f + 0.0000028f * cachedFeedbackHowl)
                                         * (1.0f + 0.28f * expressionPressure);
            const auto loopPhasePush = feedback.loopSignal * modalCosine[modeIndex];
            const auto loopInjection = loopWeight
                                     * (0.0000038f + 0.0000180f * feedback.loopAmount)
                                     * (loopPhasePush >= 0.0f ? loopPhasePush : loopPhasePush * 0.30f);
            modalAmplitude[modeIndex] = juce::jlimit (-1.6f,
                                                      1.6f,
                                                      modalAmplitude[modeIndex] + amplitudeSign * feedbackInjection + loopInjection);
            feedbackHowlOutput += modalCosine[modeIndex]
                                 * (feedbackWeight + loopWeight * (0.55f + 0.95f * std::abs (feedback.loopSignal)))
                                 * (0.35f + 0.65f * highMode);
        }
    }

    if (feedbackHowlOutput != 0.0f)
        modalOutput += feedbackHowlOutput
                     * (cachedFeedbackHowl * (0.00035f + 0.0014f * cachedFeedbackHowl)
                        + feedback.loopAmount * (0.00062f + 0.00235f * feedback.loopAmount));

    return modalOutput;
}

float StringVoice::renderPickTransient() noexcept
{
    if (std::abs (pickTransient) > 0.000001f)
    {
        const auto output = pickTransient;
        pickTransient *= pickTransientDecay;
        return output;
    }

    return 0.0f;
}

float StringVoice::renderContactLayer() noexcept
{
    auto contactOutput = 0.0f;

    if (pickContactSamplesRemaining > 0)
    {
        const auto rawContact = nextNoiseSample();
        const auto contactScratch = rawContact - previousContactNoise * 0.84f;
        previousContactNoise = rawContact;
        pickContactPhase += pickContactPhaseStep * (1.0f + 0.035f * rawContact);
        pickGrindPhase += pickGrindPhaseStep * (1.0f + 0.12f * rawContact);
        pickCoinPhase += pickCoinPhaseStep * (1.0f + 0.18f * rawContact);
        pickHeavyPhase += pickHeavyPhaseStep * (1.0f + 0.035f * rawContact);

        if (pickContactPhase > 6.28318530717958647692f)
            pickContactPhase -= 6.28318530717958647692f;

        if (pickGrindPhase > 6.28318530717958647692f)
            pickGrindPhase -= 6.28318530717958647692f;

        if (pickCoinPhase > 6.28318530717958647692f)
            pickCoinPhase -= 6.28318530717958647692f;

        if (pickHeavyPhase > 6.28318530717958647692f)
            pickHeavyPhase -= 6.28318530717958647692f;

        const auto ring = fastContactSin (pickContactPhase) + 0.32f * fastContactSin (pickContactPhase * 2.37f);
        auto grind = 0.0f;

        if (pickGrindAmount > 0.000001f)
        {
            if (pickSlipCountdown <= 0)
            {
                const auto intervalSeconds = 0.00028f + 0.00085f * (0.5f + 0.5f * nextNoiseSample());
                pickSlipCountdown = juce::jmax (1, static_cast<int> (sampleRate * intervalSeconds));
                pickSlipImpulse += pickGrindAmount * (0.45f + 0.55f * nextNoiseSample());
            }

            const auto grindTone = fastContactSin (pickGrindPhase)
                                 + 0.27f * fastContactSin (pickGrindPhase * 1.91f)
                                 + 0.15f * fastContactSin (pickGrindPhase * 3.07f);
            grind = pickSlipImpulse * grindTone;
            pickSlipImpulse *= pickSlipDecay;
            pickGrindAmount *= pickGrindDecay;
            --pickSlipCountdown;
        }

        auto coin = 0.0f;

        if (pickCoinAmount > 0.000001f)
        {
            if (pickCoinCountdown <= 0)
            {
                const auto intervalSeconds = 0.00011f + 0.00046f * (0.5f + 0.5f * nextNoiseSample());
                pickCoinCountdown = juce::jmax (1, static_cast<int> (sampleRate * intervalSeconds));
                pickCoinImpulse += pickCoinAmount * (0.58f + 0.42f * std::abs (fastContactSin (pickCoinPhase * 0.43f + nextNoiseSample())));
            }

            const auto ridge = std::tanh (3.2f * (fastContactSin (pickCoinPhase)
                                                + 0.42f * fastContactSin (pickCoinPhase * 2.13f)
                                                + 0.25f * fastContactSin (pickCoinPhase * 3.71f)));
            const auto burr = fastAbsSeventhPower (fastContactSin (pickCoinPhase * 0.5f));
            coin = pickCoinImpulse * (0.72f * ridge + 0.28f * burr * contactScratch);
            pickCoinImpulse *= pickCoinImpulseDecay;
            pickCoinAmount *= pickCoinDecay;
            --pickCoinCountdown;
        }

        auto heavy = 0.0f;

        if (pickHeavyAmount > 0.000001f)
        {
            const auto ridgeCarrier = fastContactSin (pickHeavyPhase)
                                    + 0.34f * fastContactSin (pickHeavyPhase * 0.47f + 1.20f)
                                    + 0.21f * fastContactSin (pickHeavyPhase * 1.63f);
            const auto ridgeTeeth = std::tanh (5.8f * (fastContactSin (pickHeavyPhase * 5.0f + 0.55f * ridgeCarrier)
                                                     + 0.24f * fastContactSin (pickHeavyPhase * 8.0f)));
            const auto pressure = 0.72f + 0.28f * fastContactSin (pickHeavyPhase * 0.17f + 0.80f);
            const auto raspTarget = pressure * ridgeCarrier * ridgeTeeth;
            pickHeavyRaspState += 0.24f * (raspTarget - pickHeavyRaspState);
            pickHeavyBodyState += 0.045f * (pickHeavyRaspState - pickHeavyBodyState);
            pickHeavyState += 0.16f * ((pickHeavyRaspState - pickHeavyBodyState * 0.42f) - pickHeavyState);
            heavy = pickHeavyAmount * pickHeavyState;
            pickHeavyAmount *= pickHeavyDecay;
        }

        const auto lightContact = contactScratch * pickContact + ring * pickContactRing + grind + coin;
        contactOutput = softClip (lightContact * (1.0f - pickHeavyChoke) + heavy);
        pickContact *= pickContactDecay;
        pickContactRing *= pickContactRingDecay;
        --pickContactSamplesRemaining;
    }

    if (fingerImpact > 0.000001f)
    {
        const auto rawImpact = nextNoiseSample();
        const auto impactScratch = rawImpact - previousGestureNoise * 0.62f;
        previousGestureNoise = rawImpact;
        fingerImpactPhase += fingerImpactPhaseStep * (1.0f + 0.045f * rawImpact);

        if (fingerImpactPhase > 6.28318530717958647692f)
            fingerImpactPhase -= 6.28318530717958647692f;

        const auto impactTone = fastContactSin (fingerImpactPhase)
                              + 0.30f * fastContactSin (fingerImpactPhase * 2.18f)
                              + 0.14f * fastContactSin (fingerImpactPhase * 3.70f);
        contactOutput += softClip (fingerImpact * (0.68f * impactTone + 0.32f * impactScratch));
        fingerImpact *= fingerImpactDecay;
    }

    if (pullOffSnap > 0.000001f)
    {
        const auto rawRelease = nextNoiseSample();
        const auto releaseScratch = rawRelease - previousGestureNoise * 0.78f;
        previousGestureNoise = rawRelease;
        const auto sidewaysPulse = fastContactSin (0.37f * static_cast<float> (samplesSinceStart + 1))
                                 + 0.45f * fastContactSin (0.19f * static_cast<float> (samplesSinceStart + 5));
        contactOutput += softClip (pullOffSnap * (0.52f * releaseScratch + 0.48f * sidewaysPulse));
        pullOffSnap *= pullOffSnapDecay;
    }

    if (slideFretContact > 0.000001f || slideFretScrape > 0.000001f || slideFretMotionDrive > 0.000001f)
    {
        constexpr auto twoPi = 6.28318530717958647692f;
        const auto motionDrive = juce::jlimit (0.0f, 1.0f, slideFretMotionDrive);
        const auto speedDrive = juce::jlimit (0.0f, 1.0f, slideFretSpeed);
        const auto rawSlide = nextNoiseSample();
        const auto slideFriction = rawSlide - previousSlideFretNoise;
        previousSlideFretNoise += (0.14f + 0.10f * woundAmount) * (rawSlide - previousSlideFretNoise);
        slideFretScrapeState += (0.12f + 0.11f * woundAmount) * (slideFriction - slideFretScrapeState);
        slideFretBodyState += (0.030f + 0.018f * woundAmount) * (slideFretScrapeState - slideFretBodyState);

        if (motionDrive > 0.000001f && slideFretSlipCountdown <= 0)
        {
            const auto intervalSeconds = (0.00024f + 0.00190f * (1.0f - speedDrive))
                                       * (1.0f - 0.42f * woundAmount)
                                       * (0.72f + 0.64f * (0.5f + 0.5f * nextNoiseSample()));
            slideFretSlipCountdown = juce::jmax (1, static_cast<int> (sampleRate * intervalSeconds));
            slideFretImpulse = juce::jlimit (0.0f,
                                             1.30f,
                                             slideFretImpulse
                                                 + motionDrive
                                                      * (0.26f + 0.48f * woundAmount)
                                                      * (0.50f + 0.50f * std::abs (nextNoiseSample())));
        }

        slideFretContactPhase += slideFretContactPhaseStep
                                * (1.0f + 0.018f * rawSlide + 0.035f * speedDrive * slideFretScrapeState);

        if (slideFretContactPhase > twoPi)
            slideFretContactPhase -= twoPi;

        const auto fretTick = fastContactSin (slideFretContactPhase)
                            + 0.32f * fastContactSin (slideFretContactPhase * 2.41f)
                            + 0.16f * fastContactSin (slideFretContactPhase * 4.17f);
        const auto scrapeRidge = fastContactSin (slideFretContactPhase * (0.58f + 0.18f * woundAmount) + 0.55f * rawSlide)
                               + (0.22f + 0.32f * woundAmount)
                                     * fastContactSin (slideFretContactPhase * (1.26f + 0.48f * woundAmount) + 0.31f);
        const auto fretScrape = motionDrive
                              * (slideFretImpulse * scrapeRidge * (1.10f + 0.72f * woundAmount)
                                  + slideFretScrapeState * (0.045f + 0.055f * woundAmount)
                                  + slideFretBodyState * (0.070f + 0.080f * woundAmount));

        contactOutput += softClip (slideFretContact * (0.66f * fretTick
                                                       + 0.24f * slideFretScrapeState
                                                       + 0.10f * slideFretBodyState)
                                 + slideFretScrape * fretScrape);
        slideFretContact *= slideFretContactDecay;
        slideFretScrape *= slideFretScrapeDecay;
        slideFretImpulse *= slideFretImpulseDecay;
        slideFretMotionDrive *= slideFretMotionDriveDecay;
        slideFretSpeed *= slideFretMotionDriveDecay;
        --slideFretSlipCountdown;

        if (slideFretContact < 0.000001f)
            slideFretContact = 0.0f;

        if (slideFretScrape < 0.000001f)
            slideFretScrape = 0.0f;

        if (slideFretImpulse < 0.000001f)
            slideFretImpulse = 0.0f;

        if (slideFretMotionDrive < 0.000001f)
            slideFretMotionDrive = 0.0f;

        if (slideFretSpeed < 0.000001f)
            slideFretSpeed = 0.0f;
    }

    return contactOutput;
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

float StringVoice::fastContactSin (float phase) noexcept
{
    constexpr auto pi = 3.14159265358979323846f;
    constexpr auto twoPi = 6.28318530717958647692f;

    while (phase > pi)
        phase -= twoPi;

    while (phase < -pi)
        phase += twoPi;

    const auto shaped = 1.27323954473516f * phase - 0.405284734569351f * phase * std::abs (phase);
    return 0.225f * (shaped * std::abs (shaped) - shaped) + shaped;
}

float StringVoice::fastAbsSeventhPower (float value) noexcept
{
    const auto magnitude = std::abs (value);
    const auto squared = magnitude * magnitude;
    return squared * squared * squared * magnitude;
}

void StringVoice::configureMode (int index, float frequency, float amplitude, float decay, float phase, float tailDampingScale) noexcept
{
    const auto clampedIndex = static_cast<size_t> (juce::jlimit (0, modalCount - 1, index));
    const auto clampedFrequency = juce::jlimit (20.0f, static_cast<float> (sampleRate * 0.45), frequency);
    const auto step = 2.0f * 3.14159265358979323846f * clampedFrequency / static_cast<float> (sampleRate);
    constexpr auto twoPi = 6.28318530717958647692f;
    const auto modePosition = static_cast<float> (clampedIndex) / static_cast<float> (modalCount - 1);
    const auto highWeight = std::pow (modePosition, 0.68f);

    modalSine[clampedIndex] = std::sin (phase);
    modalCosine[clampedIndex] = std::cos (phase);
    modalSinStep[clampedIndex] = std::sin (step);
    modalCosStep[clampedIndex] = std::cos (step);
    modalPhaseStep[clampedIndex] = step;
    modalAmplitude[clampedIndex] = amplitude;
    modalDecay[clampedIndex] = juce::jlimit (0.90f, 0.999999f, decay);
    modalTailDampingScale[clampedIndex] = juce::jlimit (0.05f, 1.0f, tailDampingScale);
    modalModePosition[clampedIndex] = modePosition;
    modalHighWeight[clampedIndex] = highWeight;
    modalLowWeight[clampedIndex] = 1.0f - highWeight;
    modalPalmWeight[clampedIndex] = 0.78f + 0.22f * modePosition;
    modalFrequency[clampedIndex] = step * static_cast<float> (sampleRate) / twoPi;
    activeModalCount = juce::jmax (activeModalCount, static_cast<int> (clampedIndex) + 1);
}

float StringVoice::getHarmonicTouchMask (int harmonic, int harmonicDivision, float harmonicAccuracy) const noexcept
{
    if (harmonicDivision <= 1)
        return 1.0f;

    const auto accuracy = juce::jlimit (0.0f, 1.0f, harmonicAccuracy);
    const auto selectedMode = harmonic % harmonicDivision == 0;
    const auto normalBlend = 1.0f - accuracy;
    const auto rejectedLeak = 0.045f + 0.42f * (1.0f - accuracy);
    const auto selectedGain = 0.95f + 0.38f * accuracy;
    const auto touchedMask = selectedMode ? selectedGain : rejectedLeak;

    return normalBlend + accuracy * touchedMask;
}

float StringVoice::getBridgeIntonationRatio (const FretboardAssignment& assignment, float bridgeIntonation) const noexcept
{
    if (assignment.fret <= 0)
        return 1.0f;

    constexpr std::array<float, 6> maxSaddleOffsets {
        -0.0042f, 0.0037f, -0.0032f, 0.0028f, -0.0024f, 0.0022f
    };
    const auto amount = std::pow (juce::jlimit (0.0f, 1.0f, bridgeIntonation), 1.15f);
    const auto stringOffset = maxSaddleOffsets[static_cast<size_t> (juce::jlimit (0, 5, assignment.stringIndex))] * amount;
    const auto idealLengthRatio = std::pow (2.0f, -static_cast<float> (assignment.fret) / 12.0f);
    const auto actualLengthRatio = idealLengthRatio + stringOffset;

    if (actualLengthRatio <= 0.05f)
        return 1.0f;

    return juce::jlimit (0.970f, 1.030f, idealLengthRatio * (1.0f + stringOffset) / actualLengthRatio);
}

float StringVoice::getFretPressureRatio (const FretboardAssignment& assignment, float fretPressure) const noexcept
{
    if (assignment.fret <= 0)
        return 1.0f;

    constexpr std::array<float, 6> maxStringCents {
        38.0f, 32.0f, 26.0f, 18.0f, 14.0f, 11.0f
    };

    const auto amount = std::pow (juce::jlimit (0.0f, 1.0f, fretPressure), 1.05f);
    const auto fretFactor = std::pow (juce::jlimit (0.0f, 1.0f, static_cast<float> (assignment.fret) / 24.0f), 0.72f);
    const auto cents = maxStringCents[static_cast<size_t> (juce::jlimit (0, 5, assignment.stringIndex))]
                     * amount
                     * fretFactor;

    return std::pow (2.0f, cents / 1200.0f);
}

float StringVoice::getWhammyRatio (float whammySemitones, float whammySpread) const noexcept
{
    constexpr std::array<float, 6> stringResponseOffset {
        -0.055f, -0.030f, -0.010f, 0.015f, 0.035f, 0.055f
    };

    const auto spread = juce::jlimit (0.0f, 1.0f, whammySpread);
    const auto clampedBend = juce::jlimit (-36.0f, 24.0f, whammySemitones);
    const auto response = 1.0f
                        + spread * stringResponseOffset[static_cast<size_t> (juce::jlimit (0, 5, stringIndex))];
    const auto adjustedSemitones = clampedBend * response;

    return std::pow (2.0f, adjustedSemitones / 12.0f);
}

float StringVoice::getEffectiveSlideFretSteps (float slideFretSteps) noexcept
{
    const auto control = juce::jlimit (0.0f, 1.0f, slideFretSteps);

    if (control <= 0.10f)
        return control * 9.0f;

    return 0.90f + (control - 0.10f) * (0.10f / 0.90f);
}

float StringVoice::getFretSteppedSlideSemitones (float neckSlideSemitones, float slideFretSteps, float slideLift) noexcept
{
    const auto clampedSlide = juce::jlimit (-12.0f, 12.0f, neckSlideSemitones);
    const auto amount = getEffectiveSlideFretSteps (slideFretSteps) * (1.0f - juce::jlimit (0.0f, 1.0f, slideLift));

    if (amount <= 0.0001f || std::abs (clampedSlide) <= 0.0001f)
        return clampedSlide;

    const auto lowerFret = std::floor (clampedSlide);
    const auto fraction = clampedSlide - lowerFret;
    const auto transitionWidth = 0.46f + (0.16f - 0.46f) * amount;
    const auto transitionStart = 0.5f - transitionWidth * 0.5f;
    const auto transitionEnd = 0.5f + transitionWidth * 0.5f;
    const auto transition = juce::jlimit (0.0f,
                                          1.0f,
                                          (fraction - transitionStart) / (transitionEnd - transitionStart));
    const auto smoothTransition = transition * transition * (3.0f - 2.0f * transition);
    const auto steppedSlide = lowerFret + smoothTransition;

    return clampedSlide + (steppedSlide - clampedSlide) * amount;
}

float StringVoice::getSlideLiftRiseSeconds (float slideLift) noexcept
{
    const auto amount = juce::jlimit (0.0f, 1.0f, slideLift);
    const auto remainingPressure = 1.0f - amount;
    return 0.050f + 1.80f * remainingPressure * remainingPressure;
}

void StringVoice::updateSlideFretContact (float neckSlideSemitones, float slideFretSteps, float slideLift) noexcept
{
    const auto clampedSlide = juce::jlimit (-12.0f, 12.0f, neckSlideSemitones);
    const auto liftAmount = juce::jlimit (0.0f, 1.0f, slideLift);
    const auto currentFret = static_cast<int> (std::floor (clampedSlide + 0.5f));
    slideMotionActivity *= slideMotionActivityDecay;

    if (! slideFretStateInitialized)
    {
        previousNeckSlideSemitones = clampedSlide;
        previousSlideFret = currentFret;
        slideFretStateInitialized = true;
        slideLiftEnvelope = 0.0f;
        return;
    }

    const auto slideDelta = clampedSlide - previousNeckSlideSemitones;
    const auto slideSpeedSemitonesPerSecond = std::abs (slideDelta)
                                            * static_cast<float> (sampleRate)
                                            / static_cast<float> (pitchControlUpdateInterval);
    const auto speedScale = juce::jlimit (0.0f, 1.0f, slideSpeedSemitonesPerSecond / 36.0f);
    const auto crossedFrets = currentFret >= previousSlideFret
                             ? currentFret - previousSlideFret
                             : previousSlideFret - currentFret;
    const auto movementActivity = juce::jlimit (0.0f,
                                                1.0f,
                                                std::abs (slideDelta) * 220.0f
                                                    + (crossedFrets > 0 ? 0.40f : 0.0f));
    slideMotionActivity = juce::jmax (slideMotionActivity, movementActivity);

    const auto liftTarget = liftAmount * juce::jlimit (0.0f, 1.0f, slideMotionActivity / 0.025f);

    if (liftTarget > slideLiftEnvelope)
    {
        const auto controlTicks = juce::jmax (1.0f,
                                              static_cast<float> (sampleRate)
                                                  * getSlideLiftRiseSeconds (liftAmount)
                                                  / static_cast<float> (pitchControlUpdateInterval));
        slideLiftEnvelope = juce::jmin (liftTarget, slideLiftEnvelope + juce::jmax (0.000001f, liftAmount / controlTicks));
    }
    else
    {
        const auto controlTicks = juce::jmax (1.0f,
                                              static_cast<float> (sampleRate)
                                                  * (0.120f + 0.280f * (1.0f - liftAmount))
                                                  / static_cast<float> (pitchControlUpdateInterval));
        slideLiftEnvelope = juce::jmax (liftTarget, slideLiftEnvelope - 1.0f / controlTicks);
    }

    const auto frettedAmount = getEffectiveSlideFretSteps (slideFretSteps) * (1.0f - slideLiftEnvelope);

    const auto physicalSlideContact = juce::jlimit (0.0f,
                                                    1.0f,
                                                    frettedAmount + slideLiftEnvelope * (0.25f + 0.55f * liftAmount));
    const auto motionDrive = speedScale * physicalSlideContact;

    if (motionDrive > 0.0001f)
    {
        constexpr auto twoPi = 6.28318530717958647692f;
        const auto currentFrequency = activeModalCount > 0 && modalFrequency[0] > 0.0f
                                    ? modalFrequency[0] * cachedPitchRatio
                                    : static_cast<float> (juce::MidiMessage::getMidiNoteInHertz (noteNumber));
        const auto squeakFrequency = currentFrequency
                                   * (16.0f + 8.0f * woundAmount)
                                   * (0.82f + 0.42f * speedScale)
                                   + 260.0f * speedScale;
        slideFretContactPhaseStep = twoPi
                                  * juce::jlimit (520.0f,
                                                   static_cast<float> (sampleRate * 0.36),
                                                   squeakFrequency)
                                  / static_cast<float> (sampleRate);
        slideFretMotionDrive = juce::jmax (slideFretMotionDrive, motionDrive);
        slideFretSpeed = juce::jmax (slideFretSpeed, speedScale);

        const auto continuousScrape = motionDrive * (0.00009f + 0.00028f * woundAmount);
        slideFretScrape = juce::jlimit (0.0f, 0.060f, slideFretScrape + continuousScrape);
        slideFretImpulse = juce::jlimit (0.0f,
                                         1.30f,
                                         slideFretImpulse + continuousScrape * (16.0f + 22.0f * woundAmount));
    }

    if (frettedAmount > 0.0001f && crossedFrets > 0 && std::abs (slideDelta) > 0.00001f)
    {
        const auto crossingScale = static_cast<float> (juce::jmin (crossedFrets, 4));
        const auto crossingSpeedScale = juce::jlimit (0.35f, 1.0f, speedScale);
        const auto directionScale = slideDelta >= 0.0f ? 1.0f : 0.86f;
        const auto tickAmount = frettedAmount * crossingScale * crossingSpeedScale * directionScale * (0.0080f + 0.0060f * woundAmount);
        const auto scrapeAmount = frettedAmount * crossingScale * (0.0025f + 0.0045f * woundAmount);

        slideFretContact = juce::jlimit (0.0f, 0.090f, slideFretContact + tickAmount);
        slideFretScrape = juce::jlimit (0.0f, 0.060f, slideFretScrape + scrapeAmount);
        slideFretImpulse = juce::jlimit (0.0f, 1.30f, slideFretImpulse + scrapeAmount * (10.0f + 13.0f * woundAmount));
    }

    if (slideLiftEnvelope > 0.0001f && std::abs (slideDelta) > 0.00001f)
    {
        const auto liftedScrape = slideLiftEnvelope * speedScale * (0.0016f + 0.0038f * woundAmount);
        slideFretScrape = juce::jlimit (0.0f, 0.060f, slideFretScrape + liftedScrape);
        slideFretImpulse = juce::jlimit (0.0f, 1.30f, slideFretImpulse + liftedScrape * (8.0f + 10.0f * woundAmount));
    }

    previousNeckSlideSemitones = clampedSlide;
    previousSlideFret = currentFret;
}

float StringVoice::updatePitchRatio (float heldSeconds,
                                     float vibratoDepthCents,
                                     float vibratoSpeedHz,
                                     float vibratoDelaySeconds,
                                     float whammySemitones,
                                     float whammySpread,
                                     float aftertouchBendSemitones,
                                     float mpePitchBendRange,
                                     float neckSlideSemitones,
                                     float slideFretSteps,
                                     float slideLift) noexcept
{
    constexpr auto twoPi = 6.28318530717958647692f;
    const auto clampedVibratoSpeed = juce::jlimit (0.0f, 14.0f, vibratoSpeedHz);

    aftertouchPressure += (aftertouchPressureTarget - aftertouchPressure) * 0.0025f;
    mpePitchBend += (mpePitchBendTarget - mpePitchBend) * 0.0065f;

    if (pitchControlSamplesUntilUpdate <= 0)
    {
        const auto clampedVibratoDepth = juce::jlimit (0.0f, 90.0f, vibratoDepthCents);
        const auto clampedVibratoDelay = juce::jlimit (0.0f, 2.0f, vibratoDelaySeconds);
        const auto vibratoEnvelope = clampedVibratoDelay <= 0.0001f
                                   ? 1.0f
                                   : heldSeconds <= clampedVibratoDelay
                                       ? 0.0f
                                       : juce::jlimit (0.0f, 1.0f, (heldSeconds - clampedVibratoDelay) / clampedVibratoDelay);
        const auto vibratoCents = clampedVibratoDepth > 0.0f && vibratoEnvelope > 0.0f
                                ? clampedVibratoDepth * vibratoEnvelope * std::sin (vibratoPhase)
                                : 0.0f;
        const auto aftertouchRatio = aftertouchPressure != 0.0f && aftertouchBendSemitones != 0.0f
                                   ? std::pow (2.0f, aftertouchPressure * aftertouchBendSemitones / 12.0f)
                                   : 1.0f;
        const auto mpePitchRatio = mpePitchBend != 0.0f && mpePitchBendRange != 0.0f
                                 ? std::pow (2.0f, mpePitchBend * mpePitchBendRange / 12.0f)
                                 : 1.0f;
        updateSlideFretContact (neckSlideSemitones, slideFretSteps, slideLift);
        const auto shapedNeckSlide = getFretSteppedSlideSemitones (neckSlideSemitones, slideFretSteps, slideLiftEnvelope);
        const auto neckSlideRatio = shapedNeckSlide != 0.0f ? std::pow (2.0f, shapedNeckSlide / 12.0f) : 1.0f;
        const auto vibratoRatio = vibratoCents != 0.0f ? std::pow (2.0f, vibratoCents / 1200.0f) : 1.0f;
        const auto whammyRatio = whammySemitones != 0.0f ? getWhammyRatio (whammySemitones, whammySpread) : 1.0f;
        const auto baseFrequency = activeModalCount > 0 && modalFrequency[0] > 0.0f
                                 ? modalFrequency[0]
                                 : 20.0f;
        const auto minPitchRatio = 20.0f / baseFrequency;
        const auto maxPitchRatio = static_cast<float> (sampleRate * 0.45) / baseFrequency;

        cachedPitchRatio = juce::jlimit (minPitchRatio,
                                         maxPitchRatio,
                                         vibratoRatio
                                             * whammyRatio
                                             * aftertouchRatio
                                             * mpePitchRatio
                                             * neckSlideRatio);
        updatePitchStepCache (cachedPitchRatio);
        pitchControlSamplesUntilUpdate = pitchControlUpdateInterval;
    }

    vibratoPhase += twoPi * clampedVibratoSpeed / static_cast<float> (sampleRate);

    while (vibratoPhase > twoPi)
        vibratoPhase -= twoPi;

    --pitchControlSamplesUntilUpdate;
    return cachedPitchRatio;
}

void StringVoice::updatePitchStepCache (float pitchRatio) noexcept
{
    useCachedPitchSteps = std::abs (pitchRatio - 1.0f) > 0.000001f;

    if (! useCachedPitchSteps)
        return;

    for (auto mode = 0; mode < activeModalCount; ++mode)
    {
        const auto modeIndex = static_cast<size_t> (mode);
        constexpr auto twoPi = 6.28318530717958647692f;
        const auto clampedFrequency = juce::jlimit (20.0f,
                                                    static_cast<float> (sampleRate * 0.45),
                                                    modalFrequency[modeIndex] * pitchRatio);
        const auto phaseStep = twoPi * clampedFrequency / static_cast<float> (sampleRate);
        modalPitchSinStep[modeIndex] = std::sin (phaseStep);
        modalPitchCosStep[modeIndex] = std::cos (phaseStep);
    }
}

void StringVoice::updateFeedbackWeightCache (bool feedbackHasAmount,
                                             bool loopActive,
                                             float feedbackAmount,
                                             float loopAmount,
                                             float feedbackFrequency,
                                             float loopFrequency) noexcept
{
    cachedFeedbackDrive = feedbackHasAmount ? std::pow (feedbackAmount, 1.45f) : 0.0f;
    cachedFeedbackHowl = feedbackHasAmount
                       ? std::pow (juce::jlimit (0.0f, 1.0f, (feedbackAmount - 0.55f) / 0.45f), 1.20f)
                       : 0.0f;
    const auto feedbackSustain = feedbackHasAmount
                               ? std::pow (juce::jlimit (0.0f, 1.0f, feedbackAmount / 0.72f), 1.30f)
                               : 0.0f;

    for (auto mode = 0; mode < activeModalCount; ++mode)
    {
        const auto modeIndex = static_cast<size_t> (mode);
        const auto modeFrequency = modalFrequency[modeIndex];
        auto feedbackWeight = 0.0f;
        auto loopWeight = 0.0f;

        if (feedbackHasAmount)
        {
            const auto harmonicRatio = modeFrequency / feedbackFrequency;
            const auto nearestHarmonic = juce::jlimit (1, 16, static_cast<int> (std::round (harmonicRatio)));
            const auto harmonicDistance = std::abs (harmonicRatio - static_cast<float> (nearestHarmonic));
            const auto harmonicLock = std::exp (-18.0f * harmonicDistance * harmonicDistance);
            const auto sustainTargetWeight = nearestHarmonic == 1 ? 0.30f
                                          : nearestHarmonic == 2 ? 1.00f
                                          : nearestHarmonic == 3 ? 0.78f
                                          : nearestHarmonic == 4 ? 0.58f
                                          : nearestHarmonic == 5 ? 0.42f
                                                                  : 0.16f;
            const auto howlTargetWeight = nearestHarmonic == 2 ? 0.90f
                                       : nearestHarmonic == 3 ? 1.00f
                                       : nearestHarmonic == 4 ? 0.78f
                                       : nearestHarmonic == 5 ? 0.52f
                                                               : 0.10f;
            feedbackWeight = harmonicLock * (sustainTargetWeight * feedbackSustain
                                             + howlTargetWeight * cachedFeedbackHowl);
        }

        if (loopActive)
        {
            const auto frequencyRatio = juce::jlimit (0.001f, 1000.0f, modeFrequency / loopFrequency);
            const auto centsDistance = std::abs (1200.0f * std::log2 (frequencyRatio));
            const auto loopLock = std::exp (-0.5f * (centsDistance / 118.0f) * (centsDistance / 118.0f));
            const auto overtoneFavor = modeFrequency > feedbackFrequency * 1.35f
                                     ? 1.0f
                                     : 0.20f + 0.36f * loopAmount;
            loopWeight = loopLock * overtoneFavor;
        }

        modalFeedbackWeight[modeIndex] = feedbackWeight;
        modalLoopWeight[modeIndex] = loopWeight;
    }

    feedbackControlSamplesUntilUpdate = feedbackControlUpdateInterval;
}

} // namespace guitar_ag
