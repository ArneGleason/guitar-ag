#include "PluginProcessor.h"
#include "PluginEditor.h"

GuitarAgAudioProcessor::GuitarAgAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

void GuitarAgAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    audioEngine.prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
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
    audioEngine.render (buffer, midiMessages);
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
    return 0.0;
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
    juce::XmlElement state ("GuitarAGState");
    state.setAttribute ("version", JucePlugin_VersionString);
    copyXmlToBinary (state, destData);
}

void GuitarAgAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ignoreUnused (data, sizeInBytes);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarAgAudioProcessor();
}
