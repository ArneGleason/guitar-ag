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
    palmMuteParameter = parameters.getRawParameterValue (palmMuteParameterId);
    harmonicTouchParameter = parameters.getRawParameterValue (harmonicTouchParameterId);
    stringAgeParameter = parameters.getRawParameterValue (stringAgeParameterId);
    bridgeIntonationParameter = parameters.getRawParameterValue (bridgeIntonationParameterId);
    fretPressureParameter = parameters.getRawParameterValue (fretPressureParameterId);
    lookaheadParameter = parameters.getRawParameterValue (lookaheadParameterId);
    fingerNoiseParameter = parameters.getRawParameterValue (fingerNoiseParameterId);
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

    layout.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { pickupModelParameterId, 1 },
        "Pickup Model",
        juce::StringArray { "Single Coil", "Humbucker", "Humbucker OOP" },
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
    audioEngine.setPalmMute (palmMuteParameter != nullptr ? palmMuteParameter->load() : 0.0f);
    audioEngine.setHarmonicTouch (harmonicTouchParameter != nullptr ? harmonicTouchParameter->load() : 0.0f);
    audioEngine.setStringAge (stringAgeParameter != nullptr ? stringAgeParameter->load() : 0.0f);
    audioEngine.setBridgeIntonation (bridgeIntonationParameter != nullptr ? bridgeIntonationParameter->load() : 0.0f);
    audioEngine.setFretPressure (fretPressureParameter != nullptr ? fretPressureParameter->load() : 0.0f);
    const auto newLatencySamples = getLookaheadSamples();

    if (newLatencySamples != currentLatencySamples)
    {
        currentLatencySamples = newLatencySamples;
        setLatencySamples (currentLatencySamples);
    }

    audioEngine.setLookaheadSamples (currentLatencySamples);
    audioEngine.setFingerNoise (fingerNoiseParameter != nullptr ? fingerNoiseParameter->load() : 0.0f);
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
