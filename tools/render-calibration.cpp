#include "../src/dsp/AudioEngine.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

namespace
{

struct MidiEvent
{
    int sample = 0;
    juce::MidiMessage message;
};

void printUsage()
{
    std::cout << "Usage: GuitarAGOfflineRender --midi <input.mid> --output <output.wav> "
                 "[--sample-rate 48000] [--block-size 512] [--tail-seconds 2.0] [--gain 1.0] "
                 "[--sustain 1.0] [--pick-stiffness 0.5] [--pick-texture 0.5] [--palm-mute 0.0] "
                 "[--harmonic-touch 0.0] [--string-age 0.0] [--bridge-intonation 0.0] "
                 "[--fret-pressure 0.0] [--aftertouch-bend 2.0] [--aftertouch 0.0] "
                 "[--lookahead-ms 0] [--finger-noise 0.0] "
                 "[--vibrato-speed 5.5] [--vibrato-depth 0.0] [--vibrato-delay-ms 0] "
                 "[--pitch-wheel 0.0] [--whammy-up 6.0] [--whammy-down 12.0] [--whammy-spread 0.35] "
                 "[--pickup-position 0.39] [--pickup-model 0]\n";
}

bool readMidiEvents (const juce::File& midiFile, double sampleRate, std::vector<MidiEvent>& events, int& lastEventSample)
{
    juce::FileInputStream input (midiFile);

    if (! input.openedOk())
        return false;

    juce::MidiFile parsedMidi;

    if (! parsedMidi.readFrom (input))
        return false;

    parsedMidi.convertTimestampTicksToSeconds();
    events.clear();
    lastEventSample = 0;

    for (auto trackIndex = 0; trackIndex < parsedMidi.getNumTracks(); ++trackIndex)
    {
        const auto* track = parsedMidi.getTrack (trackIndex);

        if (track == nullptr)
            continue;

        for (auto eventIndex = 0; eventIndex < track->getNumEvents(); ++eventIndex)
        {
            const auto* event = track->getEventPointer (eventIndex);

            if (event == nullptr)
                continue;

            const auto& message = event->message;

            if (! message.isNoteOnOrOff() && ! message.isAftertouch())
                continue;

            const auto sample = static_cast<int> (std::round (message.getTimeStamp() * sampleRate));
            events.push_back ({ sample, message });
            lastEventSample = std::max (lastEventSample, sample);
        }
    }

    std::sort (events.begin(), events.end(), [] (const auto& left, const auto& right)
    {
        return left.sample < right.sample;
    });

    return true;
}

bool writeWav (const juce::File& outputFile, juce::AudioBuffer<float>& audio, double sampleRate)
{
    outputFile.deleteFile();

    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::FileOutputStream> outputStream (outputFile.createOutputStream());

    if (outputStream == nullptr || ! outputStream->openedOk())
        return false;

    std::unique_ptr<juce::AudioFormatWriter> writer (
        wavFormat.createWriterFor (outputStream.get(),
                                   sampleRate,
                                   static_cast<unsigned int> (audio.getNumChannels()),
                                   24,
                                   {},
                                   0));

    if (writer == nullptr)
        return false;

    outputStream.release();
    return writer->writeFromAudioSampleBuffer (audio, 0, audio.getNumSamples());
}

} // namespace

int main (int argc, char* argv[])
{
    juce::File midiFile;
    juce::File outputFile;
    auto sampleRate = 48000.0;
    auto blockSize = 512;
    auto tailSeconds = 2.0;
    auto gain = 1.0f;
    auto sustain = 1.0f;
    auto pickStiffness = 0.5f;
    auto pickTexture = 0.5f;
    auto palmMute = 0.0f;
    auto harmonicTouch = 0.0f;
    auto stringAge = 0.0f;
    auto bridgeIntonation = 0.0f;
    auto fretPressure = 0.0f;
    auto aftertouchBend = 2.0f;
    auto aftertouch = 0.0f;
    auto lookaheadMs = 0.0f;
    auto fingerNoise = 0.0f;
    auto vibratoSpeed = 5.5f;
    auto vibratoDepth = 0.0f;
    auto vibratoDelayMs = 0.0f;
    auto pitchWheel = 0.0f;
    auto whammyUp = 6.0f;
    auto whammyDown = 12.0f;
    auto whammySpread = 0.35f;
    auto pickupPosition = 0.39f;
    auto pickupModel = 0;

    for (auto i = 1; i < argc; ++i)
    {
        const juce::String argument (argv[i]);
        const auto hasValue = i + 1 < argc;

        if (argument == "--midi" && hasValue)
        {
            midiFile = juce::File (argv[++i]);
        }
        else if (argument == "--output" && hasValue)
        {
            outputFile = juce::File (argv[++i]);
        }
        else if (argument == "--sample-rate" && hasValue)
        {
            sampleRate = juce::String (argv[++i]).getDoubleValue();
        }
        else if (argument == "--block-size" && hasValue)
        {
            blockSize = juce::jmax (1, juce::String (argv[++i]).getIntValue());
        }
        else if (argument == "--tail-seconds" && hasValue)
        {
            tailSeconds = juce::jmax (0.0, juce::String (argv[++i]).getDoubleValue());
        }
        else if (argument == "--gain" && hasValue)
        {
            gain = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--sustain" && hasValue)
        {
            sustain = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--pick-stiffness" && hasValue)
        {
            pickStiffness = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--pick-texture" && hasValue)
        {
            pickTexture = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--palm-mute" && hasValue)
        {
            palmMute = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--harmonic-touch" && hasValue)
        {
            harmonicTouch = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--string-age" && hasValue)
        {
            stringAge = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--bridge-intonation" && hasValue)
        {
            bridgeIntonation = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--fret-pressure" && hasValue)
        {
            fretPressure = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--aftertouch-bend" && hasValue)
        {
            aftertouchBend = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--aftertouch" && hasValue)
        {
            aftertouch = juce::jlimit (0.0f, 1.0f, juce::String (argv[++i]).getFloatValue());
        }
        else if (argument == "--lookahead-ms" && hasValue)
        {
            lookaheadMs = juce::jlimit (0.0f, 250.0f, juce::String (argv[++i]).getFloatValue());
        }
        else if (argument == "--finger-noise" && hasValue)
        {
            fingerNoise = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--vibrato-speed" && hasValue)
        {
            vibratoSpeed = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--vibrato-depth" && hasValue)
        {
            vibratoDepth = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--vibrato-delay-ms" && hasValue)
        {
            vibratoDelayMs = juce::jlimit (0.0f, 2000.0f, juce::String (argv[++i]).getFloatValue());
        }
        else if (argument == "--pitch-wheel" && hasValue)
        {
            pitchWheel = juce::jlimit (-1.0f, 1.0f, juce::String (argv[++i]).getFloatValue());
        }
        else if (argument == "--whammy-up" && hasValue)
        {
            whammyUp = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--whammy-down" && hasValue)
        {
            whammyDown = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--whammy-spread" && hasValue)
        {
            whammySpread = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--pickup-position" && hasValue)
        {
            pickupPosition = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--pickup-model" && hasValue)
        {
            pickupModel = juce::String (argv[++i]).getIntValue();
        }
        else
        {
            printUsage();
            return 2;
        }
    }

    if (! midiFile.existsAsFile() || outputFile == juce::File())
    {
        printUsage();
        return 2;
    }

    std::vector<MidiEvent> events;
    auto lastEventSample = 0;

    if (! readMidiEvents (midiFile, sampleRate, events, lastEventSample))
    {
        std::cerr << "Could not read MIDI file: " << midiFile.getFullPathName() << "\n";
        return 1;
    }

    if (std::abs (pitchWheel) > 0.0001f)
    {
        const auto pitchWheelValue = juce::jlimit (0, 16383, juce::roundToInt (8192.0f + pitchWheel * 8191.0f));
        events.push_back ({ 0, juce::MidiMessage::pitchWheel (1, pitchWheelValue) });
        std::sort (events.begin(), events.end(), [] (const auto& left, const auto& right)
        {
            return left.sample < right.sample;
        });
    }

    if (aftertouch > 0.0001f)
    {
        for (const auto& event : events)
        {
            if (event.message.isNoteOn())
            {
                const auto touchSample = event.sample + static_cast<int> (std::round (sampleRate * 0.25));
                const auto touchValue = juce::jlimit (0, 127, juce::roundToInt (aftertouch * 127.0f));
                events.push_back ({ touchSample,
                                    juce::MidiMessage::aftertouchChange (event.message.getChannel(),
                                                                         event.message.getNoteNumber(),
                                                                         touchValue) });
                break;
            }
        }

        std::sort (events.begin(), events.end(), [] (const auto& left, const auto& right)
        {
            return left.sample < right.sample;
        });
    }

    const auto totalSamples = lastEventSample + static_cast<int> (std::round (tailSeconds * sampleRate));
    juce::AudioBuffer<float> output (2, totalSamples);
    output.clear();

    guitar_ag::AudioEngine engine;
    engine.prepare (sampleRate, blockSize, output.getNumChannels());
    engine.setTailSustain (sustain);
    engine.setPickStiffness (pickStiffness);
    engine.setPickTexture (pickTexture);
    engine.setPalmMute (palmMute);
    engine.setHarmonicTouch (harmonicTouch);
    engine.setStringAge (stringAge);
    engine.setBridgeIntonation (bridgeIntonation);
    engine.setFretPressure (fretPressure);
    engine.setAftertouchBendSemitones (aftertouchBend);
    engine.setLookaheadSamples (static_cast<int> (std::round (sampleRate * static_cast<double> (lookaheadMs) / 1000.0)));
    engine.setFingerNoise (fingerNoise);
    engine.setVibratoSpeed (vibratoSpeed);
    engine.setVibratoDepth (vibratoDepth);
    engine.setVibratoDelay (vibratoDelayMs / 1000.0f);
    engine.setWhammyEnabled (true);
    engine.setWhammyUpSemitones (whammyUp);
    engine.setWhammyDownSemitones (whammyDown);
    engine.setWhammySpread (whammySpread);
    engine.setPickupPosition (pickupPosition);
    engine.setPickupModel (pickupModel);
    engine.reset();

    auto eventIndex = static_cast<size_t> (0);

    for (auto blockStart = 0; blockStart < totalSamples; blockStart += blockSize)
    {
        const auto samplesThisBlock = std::min (blockSize, totalSamples - blockStart);
        juce::AudioBuffer<float> block (output.getArrayOfWritePointers(), output.getNumChannels(), blockStart, samplesThisBlock);
        juce::MidiBuffer midi;

        while (eventIndex < events.size()
               && events[eventIndex].sample >= blockStart
               && events[eventIndex].sample < blockStart + samplesThisBlock)
        {
            midi.addEvent (events[eventIndex].message, events[eventIndex].sample - blockStart);
            ++eventIndex;
        }

        engine.render (block, midi);
    }

    outputFile.getParentDirectory().createDirectory();
    output.applyGain (gain);

    if (! writeWav (outputFile, output, sampleRate))
    {
        std::cerr << "Could not write WAV file: " << outputFile.getFullPathName() << "\n";
        return 1;
    }

    std::cout << "Rendered " << events.size() << " MIDI note events to "
              << outputFile.getFullPathName() << "\n";
    return 0;
}
