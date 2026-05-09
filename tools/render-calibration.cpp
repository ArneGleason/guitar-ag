#include "../src/dsp/AudioEngine.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
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
                 "[--fret-pressure 0.0] [--aftertouch-bend 2.0] [--neck-slide 0.0] [--aftertouch 0.0] "
                 "[--lookahead-ms 0] [--finger-noise 0.0] [--legato-articulation 0.0] [--amp-feedback 0.0] "
                 "[--feedback-return-distorted 1] "
                 "[--vibrato-speed 5.5] [--vibrato-depth 0.0] [--vibrato-delay-ms 0] "
                 "[--mpe-mode 0] [--mpe-bend-range 48.0] [--mpe-pressure-amount 0.65] [--mpe-cc74-amount 0.65] "
                 "[--channel-pressure 0.0] [--cc74 0.0] "
                 "[--pitch-wheel 0.0] [--whammy-up 6.0] [--whammy-down 12.0] [--whammy-spread 0.35] "
                 "[--pickup-position 0.39] [--pickup-model 0] [--perf-report]\n";
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

            if (! message.isNoteOnOrOff()
                && ! message.isAftertouch()
                && ! message.isChannelPressure()
                && ! message.isPitchWheel()
                && ! (message.isController() && message.getControllerNumber() == 74))
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
    auto neckSlide = 0.0f;
    auto aftertouch = 0.0f;
    auto lookaheadMs = 0.0f;
    auto fingerNoise = 0.0f;
    auto legatoArticulation = 0.0f;
    auto ampFeedback = 0.0f;
    auto feedbackReturnDistorted = true;
    auto vibratoSpeed = 5.5f;
    auto vibratoDepth = 0.0f;
    auto vibratoDelayMs = 0.0f;
    auto mpeMode = false;
    auto mpeBendRange = 48.0f;
    auto mpePressureAmount = 0.65f;
    auto mpeCc74Amount = 0.65f;
    auto channelPressure = 0.0f;
    auto cc74 = 0.0f;
    auto pitchWheel = 0.0f;
    auto whammyUp = 6.0f;
    auto whammyDown = 12.0f;
    auto whammySpread = 0.35f;
    auto pickupPosition = 0.39f;
    auto pickupModel = 0;
    auto perfReport = false;

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
        else if (argument == "--neck-slide" && hasValue)
        {
            neckSlide = juce::String (argv[++i]).getFloatValue();
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
        else if (argument == "--legato-articulation" && hasValue)
        {
            legatoArticulation = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--amp-feedback" && hasValue)
        {
            ampFeedback = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--feedback-return-distorted" && hasValue)
        {
            feedbackReturnDistorted = juce::String (argv[++i]).getIntValue() != 0;
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
        else if (argument == "--mpe-mode" && hasValue)
        {
            mpeMode = juce::String (argv[++i]).getIntValue() != 0;
        }
        else if (argument == "--mpe-bend-range" && hasValue)
        {
            mpeBendRange = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--mpe-pressure-amount" && hasValue)
        {
            mpePressureAmount = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--mpe-cc74-amount" && hasValue)
        {
            mpeCc74Amount = juce::String (argv[++i]).getFloatValue();
        }
        else if (argument == "--channel-pressure" && hasValue)
        {
            channelPressure = juce::jlimit (0.0f, 1.0f, juce::String (argv[++i]).getFloatValue());
        }
        else if (argument == "--cc74" && hasValue)
        {
            cc74 = juce::jlimit (0.0f, 1.0f, juce::String (argv[++i]).getFloatValue());
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
        else if (argument == "--perf-report")
        {
            perfReport = true;
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

    if (channelPressure > 0.0001f || cc74 > 0.0001f)
    {
        for (const auto& event : events)
        {
            if (event.message.isNoteOn())
            {
                const auto expressionSample = event.sample + static_cast<int> (std::round (sampleRate * 0.25));

                if (channelPressure > 0.0001f)
                {
                    const auto pressureValue = juce::jlimit (0, 127, juce::roundToInt (channelPressure * 127.0f));
                    events.push_back ({ expressionSample,
                                        juce::MidiMessage::channelPressureChange (event.message.getChannel(), pressureValue) });
                }

                if (cc74 > 0.0001f)
                {
                    const auto cc74Value = juce::jlimit (0, 127, juce::roundToInt (cc74 * 127.0f));
                    events.push_back ({ expressionSample,
                                        juce::MidiMessage::controllerEvent (event.message.getChannel(), 74, cc74Value) });
                }

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
    engine.setNeckSlideSemitones (neckSlide);
    engine.setLookaheadSamples (static_cast<int> (std::round (sampleRate * static_cast<double> (lookaheadMs) / 1000.0)));
    engine.setFingerNoise (fingerNoise);
    engine.setLegatoArticulation (legatoArticulation);
    engine.setAmpFeedback (ampFeedback);
    engine.setFeedbackReturnDistorted (feedbackReturnDistorted);
    engine.setVibratoSpeed (vibratoSpeed);
    engine.setVibratoDepth (vibratoDepth);
    engine.setVibratoDelay (vibratoDelayMs / 1000.0f);
    engine.setMpeEnabled (mpeMode);
    engine.setMpePitchBendRange (mpeBendRange);
    engine.setMpePressureAmount (mpePressureAmount);
    engine.setMpeTimbreAmount (mpeCc74Amount);
    engine.setWhammyEnabled (true);
    engine.setWhammyUpSemitones (whammyUp);
    engine.setWhammyDownSemitones (whammyDown);
    engine.setWhammySpread (whammySpread);
    engine.setPickupPosition (pickupPosition);
    engine.setPickupModel (pickupModel);
    engine.reset();

    guitar_ag::AudioEngine::PerformanceStats performanceStats;
    auto totalRenderSeconds = 0.0;
    auto maxBlockRenderSeconds = 0.0;

    if (perfReport)
    {
        performanceStats.reset();
        engine.setPerformanceStats (&performanceStats);
    }

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

        const auto blockStartTime = perfReport ? std::chrono::steady_clock::now()
                                               : std::chrono::steady_clock::time_point {};
        engine.render (block, midi);

        if (perfReport)
        {
            const auto blockEndTime = std::chrono::steady_clock::now();
            const auto blockSeconds = std::chrono::duration<double> (blockEndTime - blockStartTime).count();
            totalRenderSeconds += blockSeconds;
            maxBlockRenderSeconds = std::max (maxBlockRenderSeconds, blockSeconds);
        }
    }

    engine.setPerformanceStats (nullptr);

    outputFile.getParentDirectory().createDirectory();
    output.applyGain (gain);

    if (! writeWav (outputFile, output, sampleRate))
    {
        std::cerr << "Could not write WAV file: " << outputFile.getFullPathName() << "\n";
        return 1;
    }

    std::cout << "Rendered " << events.size() << " MIDI note events to "
              << outputFile.getFullPathName() << "\n";

    if (perfReport)
    {
        const auto renderedSeconds = sampleRate > 0.0
                                   ? static_cast<double> (performanceStats.renderedSamples) / sampleRate
                                   : 0.0;
        const auto realtimeFactor = totalRenderSeconds > 0.0
                                  ? renderedSeconds / totalRenderSeconds
                                  : 0.0;
        const auto averageBlockRenderMs = performanceStats.renderedSamples > 0
                                        ? 1000.0 * totalRenderSeconds
                                            / static_cast<double> ((performanceStats.renderedSamples + blockSize - 1)
                                                                   / blockSize)
                                        : 0.0;

        std::cout << std::fixed << std::setprecision (3)
                  << "Performance report:\n"
                  << "  rendered audio: " << renderedSeconds << " s, "
                  << performanceStats.renderedSamples << " samples\n"
                  << "  render time: " << totalRenderSeconds << " s, "
                  << realtimeFactor << "x realtime\n"
                  << "  block render: avg " << averageBlockRenderMs << " ms, max "
                  << (1000.0 * maxBlockRenderSeconds) << " ms\n"
                  << "  string voices: avg " << performanceStats.getAverageActiveVoices()
                  << ", max " << performanceStats.maxActiveVoices
                  << ", voice-samples " << performanceStats.activeVoiceSamples << "\n"
                  << "  finger-noise voices: avg " << performanceStats.getAverageActiveFingerNoiseVoices()
                  << ", max " << performanceStats.maxActiveFingerNoiseVoices
                  << ", voice-samples " << performanceStats.activeFingerNoiseSamples << "\n";
    }

    return 0;
}
