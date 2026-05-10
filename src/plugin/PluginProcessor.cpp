#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cmath>

GuitarAgAudioProcessor::GuitarAgAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "GuitarAGParameters", createParameterLayout())
{
    tailSustainParameter = parameters.getRawParameterValue (tailSustainParameterId);
    pickStiffnessParameter = parameters.getRawParameterValue (pickStiffnessParameterId);
    pickTextureParameter = parameters.getRawParameterValue (pickTextureParameterId);
    pickBiteParameter = parameters.getRawParameterValue (pickBiteParameterId);
    pickStrokeParameter = parameters.getRawParameterValue (pickStrokeParameterId);
    palmMuteParameter = parameters.getRawParameterValue (palmMuteParameterId);
    harmonicTouchParameter = parameters.getRawParameterValue (harmonicTouchParameterId);
    stringAgeParameter = parameters.getRawParameterValue (stringAgeParameterId);
    bridgeIntonationParameter = parameters.getRawParameterValue (bridgeIntonationParameterId);
    fretPressureParameter = parameters.getRawParameterValue (fretPressureParameterId);
    lookaheadParameter = parameters.getRawParameterValue (lookaheadParameterId);
    fingerNoiseParameter = parameters.getRawParameterValue (fingerNoiseParameterId);
    legatoArticulationParameter = parameters.getRawParameterValue (legatoArticulationParameterId);
    ampFeedbackParameter = parameters.getRawParameterValue (ampFeedbackParameterId);
    feedbackReturnDistortedParameter = parameters.getRawParameterValue (feedbackReturnDistortedParameterId);
    vibratoSpeedParameter = parameters.getRawParameterValue (vibratoSpeedParameterId);
    vibratoDepthParameter = parameters.getRawParameterValue (vibratoDepthParameterId);
    vibratoDelayParameter = parameters.getRawParameterValue (vibratoDelayParameterId);
    vibratoModWheelSpeedParameter = parameters.getRawParameterValue (vibratoModWheelSpeedParameterId);
    vibratoModWheelDepthParameter = parameters.getRawParameterValue (vibratoModWheelDepthParameterId);
    mpeEnabledParameter = parameters.getRawParameterValue (mpeEnabledParameterId);
    mpePitchBendRangeParameter = parameters.getRawParameterValue (mpePitchBendRangeParameterId);
    mpePressureAmountParameter = parameters.getRawParameterValue (mpePressureAmountParameterId);
    mpeTimbreAmountParameter = parameters.getRawParameterValue (mpeTimbreAmountParameterId);
    whammyEnabledParameter = parameters.getRawParameterValue (whammyEnabledParameterId);
    whammyUpRangeParameter = parameters.getRawParameterValue (whammyUpRangeParameterId);
    whammyDownRangeParameter = parameters.getRawParameterValue (whammyDownRangeParameterId);
    whammySpreadParameter = parameters.getRawParameterValue (whammySpreadParameterId);
    aftertouchBendParameter = parameters.getRawParameterValue (aftertouchBendParameterId);
    neckSlideParameter = parameters.getRawParameterValue (neckSlideParameterId);
    slideFretStepsParameter = parameters.getRawParameterValue (slideFretStepsParameterId);
    slideLiftParameter = parameters.getRawParameterValue (slideLiftParameterId);
    slideSqueakParameter = parameters.getRawParameterValue (slideSqueakParameterId);
    slideSqueakDownParameter = parameters.getRawParameterValue (slideSqueakDownParameterId);
    pickupPositionParameter = parameters.getRawParameterValue (pickupPositionParameterId);
    pickupModelParameter = parameters.getRawParameterValue (pickupModelParameterId);
}

juce::AudioProcessorValueTreeState::ParameterLayout GuitarAgAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> layout;
    const auto percentString = [] (float value, int)
    {
        return juce::String (juce::roundToInt (value * 100.0f)) + "%";
    };
    const auto percentValue = [] (const juce::String& text)
    {
        return text.getFloatValue() / 100.0f;
    };
    const auto hzString = [] (float value, int)
    {
        return juce::String (value, 1) + " Hz";
    };
    const auto hzValue = [] (const juce::String& text)
    {
        return text.getFloatValue();
    };
    const auto centsString = [] (float value, int)
    {
        return juce::String (juce::roundToInt (value)) + " c";
    };
    const auto centsValue = [] (const juce::String& text)
    {
        return text.getFloatValue();
    };
    const auto semitoneString = [] (float value, int)
    {
        return juce::String (value, 1) + " st";
    };
    const auto semitoneValue = [] (const juce::String& text)
    {
        return text.getFloatValue();
    };
    const auto msString = [] (float value, int)
    {
        return juce::String (juce::roundToInt (value * 1000.0f)) + " ms";
    };
    const auto msValue = [] (const juce::String& text)
    {
        return text.getFloatValue() / 1000.0f;
    };

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { tailSustainParameterId, 1 },
        "Sustain",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        1.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { pickStiffnessParameterId, 1 },
        "Pick Stiffness",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.5f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { stringAgeParameterId, 1 },
        "String Age",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { bridgeIntonationParameterId, 1 },
        "Bridge Intonation",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { fretPressureParameterId, 1 },
        "Fret Pressure",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { aftertouchBendParameterId, 1 },
        "Aftertouch Bend",
        juce::NormalisableRange<float> { -12.0f, 12.0f, 0.1f, 1.0f },
        2.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("st")
            .withStringFromValueFunction (semitoneString)
            .withValueFromStringFunction (semitoneValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { neckSlideParameterId, 1 },
        "Neck Slide",
        juce::NormalisableRange<float> { -12.0f, 12.0f, 0.1f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("st")
            .withStringFromValueFunction (semitoneString)
            .withValueFromStringFunction (semitoneValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { slideFretStepsParameterId, 1 },
        "Fret Steps",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.65f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { slideLiftParameterId, 1 },
        "Slide Lift",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { slideSqueakParameterId, 1 },
        "Slide Squeak Up",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.20f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { slideSqueakDownParameterId, 1 },
        "Slide Squeak Down",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.20f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { lookaheadParameterId, 1 },
        "Lookahead",
        juce::StringArray { "Off", "150 ms", "250 ms" },
        0));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { fingerNoiseParameterId, 1 },
        "Finger Noise",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { legatoArticulationParameterId, 1 },
        "Legato Articulation",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ampFeedbackParameterId, 1 },
        "Amp Feedback",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { feedbackReturnDistortedParameterId, 1 },
        "Feedback Return Distorted",
        true));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { vibratoSpeedParameterId, 1 },
        "Vibrato Speed",
        juce::NormalisableRange<float> { 0.10f, 12.0f, 0.01f, 0.72f },
        5.5f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("Hz")
            .withStringFromValueFunction (hzString)
            .withValueFromStringFunction (hzValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { vibratoDepthParameterId, 1 },
        "Vibrato Depth",
        juce::NormalisableRange<float> { 0.0f, 60.0f, 0.1f, 0.85f },
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("c")
            .withStringFromValueFunction (centsString)
            .withValueFromStringFunction (centsValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { vibratoDelayParameterId, 1 },
        "Vibrato Delay",
        juce::NormalisableRange<float> { 0.0f, 2.0f, 0.001f, 0.60f },
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("ms")
            .withStringFromValueFunction (msString)
            .withValueFromStringFunction (msValue)));

    layout.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { vibratoModWheelSpeedParameterId, 1 },
        "Mod Wheel To Vibrato Speed",
        false));

    layout.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { vibratoModWheelDepthParameterId, 1 },
        "Mod Wheel To Vibrato Depth",
        false));

    layout.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { mpeEnabledParameterId, 1 },
        "MPE Mode",
        false));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { mpePitchBendRangeParameterId, 1 },
        "MPE Bend Range",
        juce::NormalisableRange<float> { 0.0f, 96.0f, 0.1f, 0.72f },
        48.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("st")
            .withStringFromValueFunction (semitoneString)
            .withValueFromStringFunction (semitoneValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { mpePressureAmountParameterId, 1 },
        "MPE Pressure Amount",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.65f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { mpeTimbreAmountParameterId, 1 },
        "MPE CC74 Amount",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.65f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { whammyEnabledParameterId, 1 },
        "Pitch Wheel Whammy",
        true));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { whammyUpRangeParameterId, 1 },
        "Whammy Up Range",
        juce::NormalisableRange<float> { 0.0f, 24.0f, 0.1f, 0.75f },
        6.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("st")
            .withStringFromValueFunction (semitoneString)
            .withValueFromStringFunction (semitoneValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { whammyDownRangeParameterId, 1 },
        "Whammy Down Range",
        juce::NormalisableRange<float> { 0.0f, 36.0f, 0.1f, 0.75f },
        12.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("st")
            .withStringFromValueFunction (semitoneString)
            .withValueFromStringFunction (semitoneValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { whammySpreadParameterId, 1 },
        "Whammy String Spread",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.35f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { pickupModelParameterId, 1 },
        "Pickup Model",
        juce::StringArray { "Single Coil", "Humbucker", "Singles OOP" },
        0));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { pickupPositionParameterId, 1 },
        "Pickup Position",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.39f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { pickTextureParameterId, 1 },
        "Pick Texture",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.5f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { pickBiteParameterId, 1 },
        "Pick Bite",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.5f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { pickStrokeParameterId, 1 },
        "Pick Stroke",
        juce::StringArray { "Down", "Up", "Alternate" },
        2));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { palmMuteParameterId, 1 },
        "Palm Mute",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { harmonicTouchParameterId, 1 },
        "Harmonic Touch",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    return { layout.begin(), layout.end() };
}

void GuitarAgAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    currentLatencySamples = getLookaheadSamples();
    setLatencySamples (currentLatencySamples);
    audioEngine.prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    audioEngine.setLookaheadSamples (currentLatencySamples);
}

void GuitarAgAudioProcessor::releaseResources()
{
    audioEngine.reset();
}

bool GuitarAgAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto output = layouts.getMainOutputChannelSet();
    return output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo();
}

void GuitarAgAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    audioEngine.setTailSustain (tailSustainParameter != nullptr ? tailSustainParameter->load() : 1.0f);
    audioEngine.setPickStiffness (pickStiffnessParameter != nullptr ? pickStiffnessParameter->load() : 0.5f);
    audioEngine.setPickTexture (pickTextureParameter != nullptr ? pickTextureParameter->load() : 0.5f);
    audioEngine.setPickBite (pickBiteParameter != nullptr ? pickBiteParameter->load() : 0.5f);
    audioEngine.setPickStrokeMode (pickStrokeParameter != nullptr ? juce::roundToInt (pickStrokeParameter->load()) : 2);
    audioEngine.setPalmMute (palmMuteParameter != nullptr ? palmMuteParameter->load() : 0.0f);
    audioEngine.setHarmonicTouch (harmonicTouchParameter != nullptr ? harmonicTouchParameter->load() : 0.0f);
    audioEngine.setStringAge (stringAgeParameter != nullptr ? stringAgeParameter->load() : 0.0f);
    audioEngine.setBridgeIntonation (bridgeIntonationParameter != nullptr ? bridgeIntonationParameter->load() : 0.0f);
    audioEngine.setFretPressure (fretPressureParameter != nullptr ? fretPressureParameter->load() : 0.0f);
    audioEngine.setAftertouchBendSemitones (aftertouchBendParameter != nullptr ? aftertouchBendParameter->load() : 2.0f);
    audioEngine.setNeckSlideSemitones (neckSlideParameter != nullptr ? neckSlideParameter->load() : 0.0f);
    audioEngine.setSlideFretSteps (slideFretStepsParameter != nullptr ? slideFretStepsParameter->load() : 0.65f);
    audioEngine.setSlideLift (slideLiftParameter != nullptr ? slideLiftParameter->load() : 0.0f);
    audioEngine.setSlideSqueak (slideSqueakParameter != nullptr ? slideSqueakParameter->load() : 0.20f);
    audioEngine.setSlideSqueakDown (slideSqueakDownParameter != nullptr ? slideSqueakDownParameter->load() : 0.20f);
    const auto newLatencySamples = getLookaheadSamples();

    if (newLatencySamples != currentLatencySamples)
    {
        currentLatencySamples = newLatencySamples;
        setLatencySamples (currentLatencySamples);
    }

    audioEngine.setLookaheadSamples (currentLatencySamples);
    audioEngine.setFingerNoise (fingerNoiseParameter != nullptr ? fingerNoiseParameter->load() : 0.0f);
    audioEngine.setLegatoArticulation (legatoArticulationParameter != nullptr ? legatoArticulationParameter->load() : 0.0f);
    audioEngine.setAmpFeedback (ampFeedbackParameter != nullptr ? ampFeedbackParameter->load() : 0.0f);
    audioEngine.setFeedbackReturnDistorted (feedbackReturnDistortedParameter != nullptr
                                            && feedbackReturnDistortedParameter->load() >= 0.5f);
    audioEngine.setVibratoSpeed (vibratoSpeedParameter != nullptr ? vibratoSpeedParameter->load() : 5.5f);
    audioEngine.setVibratoDepth (vibratoDepthParameter != nullptr ? vibratoDepthParameter->load() : 0.0f);
    audioEngine.setVibratoDelay (vibratoDelayParameter != nullptr ? vibratoDelayParameter->load() : 0.0f);
    audioEngine.setVibratoModWheelSpeedEnabled (vibratoModWheelSpeedParameter != nullptr
                                                && vibratoModWheelSpeedParameter->load() >= 0.5f);
    audioEngine.setVibratoModWheelDepthEnabled (vibratoModWheelDepthParameter != nullptr
                                                && vibratoModWheelDepthParameter->load() >= 0.5f);
    audioEngine.setMpeEnabled (mpeEnabledParameter != nullptr && mpeEnabledParameter->load() >= 0.5f);
    audioEngine.setMpePitchBendRange (mpePitchBendRangeParameter != nullptr ? mpePitchBendRangeParameter->load() : 48.0f);
    audioEngine.setMpePressureAmount (mpePressureAmountParameter != nullptr ? mpePressureAmountParameter->load() : 0.65f);
    audioEngine.setMpeTimbreAmount (mpeTimbreAmountParameter != nullptr ? mpeTimbreAmountParameter->load() : 0.65f);
    audioEngine.setWhammyEnabled (whammyEnabledParameter == nullptr || whammyEnabledParameter->load() >= 0.5f);
    audioEngine.setWhammyUpSemitones (whammyUpRangeParameter != nullptr ? whammyUpRangeParameter->load() : 6.0f);
    audioEngine.setWhammyDownSemitones (whammyDownRangeParameter != nullptr ? whammyDownRangeParameter->load() : 12.0f);
    audioEngine.setWhammySpread (whammySpreadParameter != nullptr ? whammySpreadParameter->load() : 0.35f);
    audioEngine.setPickupPosition (pickupPositionParameter != nullptr ? pickupPositionParameter->load() : 0.39f);
    audioEngine.setPickupModel (pickupModelParameter != nullptr ? juce::roundToInt (pickupModelParameter->load()) : 0);
    audioEngine.render (buffer, midiMessages);
}

int GuitarAgAudioProcessor::getLookaheadSamples() const noexcept
{
    const auto mode = lookaheadParameter != nullptr ? juce::roundToInt (lookaheadParameter->load()) : 0;
    const auto seconds = mode == 1 ? 0.150 : mode == 2 ? 0.250 : 0.0;
    return static_cast<int> (std::round (currentSampleRate * seconds));
}

juce::AudioProcessorEditor* GuitarAgAudioProcessor::createEditor()
{
    return new GuitarAgAudioProcessorEditor (*this);
}

bool GuitarAgAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String GuitarAgAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarAgAudioProcessor::acceptsMidi() const
{
    return true;
}

bool GuitarAgAudioProcessor::producesMidi() const
{
    return false;
}

bool GuitarAgAudioProcessor::isMidiEffect() const
{
    return false;
}

double GuitarAgAudioProcessor::getTailLengthSeconds() const
{
    return 12.0;
}

int GuitarAgAudioProcessor::getNumPrograms()
{
    return 1;
}

int GuitarAgAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GuitarAgAudioProcessor::setCurrentProgram (int)
{
}

const juce::String GuitarAgAudioProcessor::getProgramName (int)
{
    return {};
}

void GuitarAgAudioProcessor::changeProgramName (int, const juce::String&)
{
}

void GuitarAgAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    state.setProperty ("version", JucePlugin_VersionString, nullptr);
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void GuitarAgAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
        parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarAgAudioProcessor();
}
