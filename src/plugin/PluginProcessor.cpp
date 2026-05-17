#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BuildInfo.h"

#include <cmath>

namespace
{
juce::String getMidiNoteName (int noteNumber)
{
    if (noteNumber < 0 || noteNumber > 127)
        return "none";

    static constexpr const char* names[] { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    return juce::String (names[noteNumber % 12]) + juce::String (noteNumber / 12 - 1);
}

juce::String getStringName (int stringIndex)
{
    static constexpr const char* names[] { "lowE", "A", "D", "G", "B", "highE" };
    return stringIndex >= 0 && stringIndex < 6 ? juce::String (names[stringIndex]) : "none";
}

juce::String getDiagnosticEventTypeName (int eventType)
{
    switch (eventType)
    {
        case 1: return "incoming_midi";
        case 2: return "assignment";
        case 3: return "note_off";
        case 4: return "panic_reset";
        default: break;
    }

    return "unknown";
}

juce::String getIncomingMidiKindName (const guitar_ag::AudioEngine::DiagnosticEvent& event)
{
    if (event.type != 1)
        return {};

    if (event.hostNoteNumber >= 0)
    {
        if (event.controllerNumber == 160)
            return "poly_aftertouch";

        return event.velocity >= 0 ? "note_on" : "note_off";
    }

    if (event.controllerNumber >= 0 && event.controllerNumber <= 127)
        return "cc";

    if (event.controllerNumber == 224)
        return "pitch_wheel";

    if (event.controllerNumber == 208)
        return "channel_pressure";

    return "other";
}
} // namespace

GuitarAgAudioProcessor::GuitarAgAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "GuitarAGParameters", createParameterLayout())
{
    tailSustainParameter = parameters.getRawParameterValue (tailSustainParameterId);
    inputOctaveParameter = parameters.getRawParameterValue (inputOctaveParameterId);
    panicResetParameter = parameters.getRawParameterValue (panicResetParameterId);
    pickStiffnessParameter = parameters.getRawParameterValue (pickStiffnessParameterId);
    pickTextureParameter = parameters.getRawParameterValue (pickTextureParameterId);
    pickBiteParameter = parameters.getRawParameterValue (pickBiteParameterId);
    pickStrokeParameter = parameters.getRawParameterValue (pickStrokeParameterId);
    strumSpeedParameter = parameters.getRawParameterValue (strumSpeedParameterId);
    strumBalanceParameter = parameters.getRawParameterValue (strumBalanceParameterId);
    playerFeelParameter = parameters.getRawParameterValue (playerFeelParameterId);
    playerFeelRecoveryParameter = parameters.getRawParameterValue (playerFeelRecoveryParameterId);
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

    static constexpr std::array<int, guitar_ag::AudioEngine::stringCount> defaultOpenNotes { 40, 45, 50, 55, 59, 64 };

    for (auto stringIndex = 0; stringIndex < static_cast<int> (defaultOpenNotes.size()); ++stringIndex)
    {
        const auto index = static_cast<size_t> (stringIndex);
        stringOpenNote[index].store (defaultOpenNotes[index], std::memory_order_relaxed);
        stringMapperNote[index].store (-1, std::memory_order_relaxed);
        stringMapperChannel[index].store (-1, std::memory_order_relaxed);
        stringMapperFret[index].store (-1, std::memory_order_relaxed);
        stringVoiceNote[index].store (-1, std::memory_order_relaxed);
        stringVoiceChannel[index].store (-1, std::memory_order_relaxed);
        stringVoiceFret[index].store (-1, std::memory_order_relaxed);
        stringFlags[index].store (0, std::memory_order_relaxed);
    }
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
    const auto secondsString = [] (float value, int)
    {
        return juce::String (value, 2) + " s";
    };
    const auto secondsValue = [] (const juce::String& text)
    {
        return text.getFloatValue();
    };
    const auto strumBalanceString = [] (float value, int)
    {
        const auto reduction = juce::roundToInt (std::abs (value) * 94.0f);

        if (reduction <= 0)
            return juce::String ("Balanced");

        return juce::String (value > 0.0f ? "Up -" : "Down -") + juce::String (reduction) + "%";
    };
    const auto strumBalanceValue = [] (const juce::String& text)
    {
        const auto numericText = text.retainCharacters ("0123456789.-");

        if (numericText.isEmpty())
            return 0.0f;

        auto value = numericText.getFloatValue();

        if (text.containsIgnoreCase ("up"))
            value = std::abs (value) / 94.0f;
        else if (text.containsIgnoreCase ("down"))
            value = -std::abs (value) / 94.0f;
        else if (std::abs (value) > 1.0f)
            value /= 100.0f;

        return juce::jlimit (-1.0f, 1.0f, value);
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

    layout.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { inputOctaveParameterId, 1 },
        "Input Octave",
        juce::StringArray { "MIDI E2=40", "DAW E2=52" },
        1));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { panicResetParameterId, 1 },
        "Panic Reset",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 1.0f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float value, int)
            {
                return value >= 0.5f ? juce::String ("Reset") : juce::String ("Ready");
            })
            .withValueFromStringFunction ([] (const juce::String& text)
            {
                return text.containsIgnoreCase ("reset") ? 1.0f : 0.0f;
            })));

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
        0.25f,
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
        juce::ParameterID { strumSpeedParameterId, 1 },
        "Strum Speed",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.10f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { strumBalanceParameterId, 1 },
        "Strum Balance",
        juce::NormalisableRange<float> { -1.0f, 1.0f, 0.001f, 1.0f },
        -0.13f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction (strumBalanceString)
            .withValueFromStringFunction (strumBalanceValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { playerFeelParameterId, 1 },
        "Player Feel",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f, 1.0f },
        0.5f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction (percentString)
            .withValueFromStringFunction (percentValue)));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { playerFeelRecoveryParameterId, 1 },
        "Feel Recovery",
        juce::NormalisableRange<float> { 0.10f, 8.0f, 0.01f, 0.62f },
        2.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("s")
            .withStringFromValueFunction (secondsString)
            .withValueFromStringFunction (secondsValue)));

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
    const auto inputOctaveMode = inputOctaveParameter != nullptr ? juce::roundToInt (inputOctaveParameter->load()) : 1;
    audioEngine.setInputTransposeSemitones (inputOctaveMode == 1 ? -12 : 0);
    const auto panicResetIsHigh = panicResetParameter != nullptr && panicResetParameter->load() >= 0.5f;

    if (panicResetRequested.exchange (false) || (panicResetIsHigh && ! panicResetParameterWasHigh))
        audioEngine.panicReset();

    panicResetParameterWasHigh = panicResetIsHigh;

    audioEngine.setTailSustain (tailSustainParameter != nullptr ? tailSustainParameter->load() : 1.0f);
    audioEngine.setPickStiffness (pickStiffnessParameter != nullptr ? pickStiffnessParameter->load() : 0.5f);
    audioEngine.setPickTexture (pickTextureParameter != nullptr ? pickTextureParameter->load() : 0.25f);
    audioEngine.setPickBite (pickBiteParameter != nullptr ? pickBiteParameter->load() : 0.5f);
    audioEngine.setPickStrokeMode (pickStrokeParameter != nullptr ? juce::roundToInt (pickStrokeParameter->load()) : 2);
    audioEngine.setStrumSpeed (strumSpeedParameter != nullptr ? strumSpeedParameter->load() : 0.10f);
    audioEngine.setStrumBalance (strumBalanceParameter != nullptr ? strumBalanceParameter->load() : -0.13f);
    audioEngine.setPlayerFeel (playerFeelParameter != nullptr ? playerFeelParameter->load() : 0.5f);
    audioEngine.setPlayerFeelRecoverySeconds (playerFeelRecoveryParameter != nullptr ? playerFeelRecoveryParameter->load() : 2.0f);

    if (playerFeelResetRequested.exchange (false))
        audioEngine.resetPlayerFeel();

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

    const auto meters = audioEngine.getPlayerFeelMeters();
    playerFeelCognitiveMeter.store (meters.cognitiveLoad, std::memory_order_relaxed);
    playerFeelDexterityMeter.store (meters.dexterityLoad, std::memory_order_relaxed);
    playerFeelEnduranceMeter.store (meters.endurance, std::memory_order_relaxed);
    storeStringStatuses (audioEngine.getStringStatuses());
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

void GuitarAgAudioProcessor::requestPlayerFeelReset() noexcept
{
    playerFeelResetRequested.store (true);
}

void GuitarAgAudioProcessor::requestPanicReset() noexcept
{
    panicResetRequested.store (true);
}

GuitarAgAudioProcessor::PlayerFeelMeterSnapshot GuitarAgAudioProcessor::getPlayerFeelMeters() const noexcept
{
    return { playerFeelCognitiveMeter.load (std::memory_order_relaxed),
             playerFeelDexterityMeter.load (std::memory_order_relaxed),
             playerFeelEnduranceMeter.load (std::memory_order_relaxed) };
}

void GuitarAgAudioProcessor::storeStringStatuses (
    const std::array<guitar_ag::AudioEngine::StringStatus, guitar_ag::AudioEngine::stringCount>& statuses) noexcept
{
    for (auto stringIndex = 0; stringIndex < static_cast<int> (statuses.size()); ++stringIndex)
    {
        const auto index = static_cast<size_t> (stringIndex);
        const auto& status = statuses[index];
        auto flags = 0;

        if (status.mapperOccupied)
            flags |= 1;

        if (status.voiceActive)
            flags |= 2;

        stringOpenNote[index].store (status.openNote, std::memory_order_relaxed);
        stringMapperNote[index].store (status.mapperNoteNumber, std::memory_order_relaxed);
        stringMapperChannel[index].store (status.mapperChannel, std::memory_order_relaxed);
        stringMapperFret[index].store (status.mapperFret, std::memory_order_relaxed);
        stringVoiceNote[index].store (status.voiceNoteNumber, std::memory_order_relaxed);
        stringVoiceChannel[index].store (status.voiceChannel, std::memory_order_relaxed);
        stringVoiceFret[index].store (status.voiceFret, std::memory_order_relaxed);
        stringFlags[index].store (flags, std::memory_order_release);
    }
}

std::array<GuitarAgAudioProcessor::StringStatusSnapshot, guitar_ag::AudioEngine::stringCount>
GuitarAgAudioProcessor::getStringStatuses() const noexcept
{
    std::array<StringStatusSnapshot, guitar_ag::AudioEngine::stringCount> statuses {};

    for (auto stringIndex = 0; stringIndex < static_cast<int> (statuses.size()); ++stringIndex)
    {
        const auto index = static_cast<size_t> (stringIndex);
        const auto flags = stringFlags[index].load (std::memory_order_acquire);
        auto& status = statuses[index];

        status.openNote = stringOpenNote[index].load (std::memory_order_relaxed);
        status.mapperNoteNumber = stringMapperNote[index].load (std::memory_order_relaxed);
        status.mapperChannel = stringMapperChannel[index].load (std::memory_order_relaxed);
        status.mapperFret = stringMapperFret[index].load (std::memory_order_relaxed);
        status.voiceNoteNumber = stringVoiceNote[index].load (std::memory_order_relaxed);
        status.voiceChannel = stringVoiceChannel[index].load (std::memory_order_relaxed);
        status.voiceFret = stringVoiceFret[index].load (std::memory_order_relaxed);
        status.mapperOccupied = (flags & 1) != 0;
        status.voiceActive = (flags & 2) != 0;
    }

    return statuses;
}

juce::String GuitarAgAudioProcessor::exportSettingsJson() const
{
    auto* root = new juce::DynamicObject();
    root->setProperty ("plugin", "Guitar AG");
    root->setProperty ("version", JucePlugin_VersionString);
    root->setProperty ("model", GUITAR_AG_MODEL_LABEL);

    auto* params = new juce::DynamicObject();
    const auto add = [params] (const char* id, const std::atomic<float>* parameter)
    {
        if (parameter != nullptr)
            params->setProperty (id, parameter->load());
    };

    add (tailSustainParameterId, tailSustainParameter);
    add (inputOctaveParameterId, inputOctaveParameter);
    add (panicResetParameterId, panicResetParameter);
    add (pickStiffnessParameterId, pickStiffnessParameter);
    add (pickTextureParameterId, pickTextureParameter);
    add (pickBiteParameterId, pickBiteParameter);
    add (pickStrokeParameterId, pickStrokeParameter);
    add (strumSpeedParameterId, strumSpeedParameter);
    add (strumBalanceParameterId, strumBalanceParameter);
    add (playerFeelParameterId, playerFeelParameter);
    add (playerFeelRecoveryParameterId, playerFeelRecoveryParameter);
    add (palmMuteParameterId, palmMuteParameter);
    add (harmonicTouchParameterId, harmonicTouchParameter);
    add (stringAgeParameterId, stringAgeParameter);
    add (bridgeIntonationParameterId, bridgeIntonationParameter);
    add (fretPressureParameterId, fretPressureParameter);
    add (lookaheadParameterId, lookaheadParameter);
    add (fingerNoiseParameterId, fingerNoiseParameter);
    add (legatoArticulationParameterId, legatoArticulationParameter);
    add (ampFeedbackParameterId, ampFeedbackParameter);
    add (feedbackReturnDistortedParameterId, feedbackReturnDistortedParameter);
    add (vibratoSpeedParameterId, vibratoSpeedParameter);
    add (vibratoDepthParameterId, vibratoDepthParameter);
    add (vibratoDelayParameterId, vibratoDelayParameter);
    add (vibratoModWheelSpeedParameterId, vibratoModWheelSpeedParameter);
    add (vibratoModWheelDepthParameterId, vibratoModWheelDepthParameter);
    add (mpeEnabledParameterId, mpeEnabledParameter);
    add (mpePitchBendRangeParameterId, mpePitchBendRangeParameter);
    add (mpePressureAmountParameterId, mpePressureAmountParameter);
    add (mpeTimbreAmountParameterId, mpeTimbreAmountParameter);
    add (whammyEnabledParameterId, whammyEnabledParameter);
    add (whammyUpRangeParameterId, whammyUpRangeParameter);
    add (whammyDownRangeParameterId, whammyDownRangeParameter);
    add (whammySpreadParameterId, whammySpreadParameter);
    add (aftertouchBendParameterId, aftertouchBendParameter);
    add (neckSlideParameterId, neckSlideParameter);
    add (slideFretStepsParameterId, slideFretStepsParameter);
    add (slideLiftParameterId, slideLiftParameter);
    add (slideSqueakParameterId, slideSqueakParameter);
    add (slideSqueakDownParameterId, slideSqueakDownParameter);
    add (pickupPositionParameterId, pickupPositionParameter);
    add (pickupModelParameterId, pickupModelParameter);

    const auto meters = getPlayerFeelMeters();
    auto* meterObject = new juce::DynamicObject();
    meterObject->setProperty ("cognitiveLoad", meters.cognitiveLoad);
    meterObject->setProperty ("dexterityLoad", meters.dexterityLoad);
    meterObject->setProperty ("endurance", meters.endurance);

    root->setProperty ("parameters", juce::var (params));
    root->setProperty ("playerFeelMeters", juce::var (meterObject));
    return juce::JSON::toString (juce::var (root), false);
}

juce::String GuitarAgAudioProcessor::exportDiagnosticsJson() const
{
    auto* root = new juce::DynamicObject();
    root->setProperty ("plugin", "Guitar AG");
    root->setProperty ("version", JucePlugin_VersionString);
    root->setProperty ("model", GUITAR_AG_MODEL_LABEL);
    root->setProperty ("commit", GUITAR_AG_GIT_COMMIT);
    root->setProperty ("sampleRate", currentSampleRate);
    root->setProperty ("maxEvents", guitar_ag::AudioEngine::maxDiagnosticEvents);

    auto* params = new juce::DynamicObject();
    if (inputOctaveParameter != nullptr)
        params->setProperty ("inputOctave", inputOctaveParameter->load());
    if (legatoArticulationParameter != nullptr)
        params->setProperty ("legatoArticulation", legatoArticulationParameter->load());
    if (strumSpeedParameter != nullptr)
        params->setProperty ("strumSpeed", strumSpeedParameter->load());
    if (strumBalanceParameter != nullptr)
        params->setProperty ("strumBalance", strumBalanceParameter->load());
    if (neckSlideParameter != nullptr)
        params->setProperty ("neckSlide", neckSlideParameter->load());
    if (lookaheadParameter != nullptr)
        params->setProperty ("lookahead", lookaheadParameter->load());
    root->setProperty ("parameters", juce::var (params));

    juce::Array<juce::var> stringArray;
    const auto stringStatuses = getStringStatuses();

    for (auto stringIndex = 0; stringIndex < static_cast<int> (stringStatuses.size()); ++stringIndex)
    {
        const auto& status = stringStatuses[static_cast<size_t> (stringIndex)];
        auto* stringObject = new juce::DynamicObject();
        stringObject->setProperty ("stringIndex", stringIndex);
        stringObject->setProperty ("stringName", getStringName (stringIndex));
        stringObject->setProperty ("openNote", status.openNote);
        stringObject->setProperty ("openName", getMidiNoteName (status.openNote));
        stringObject->setProperty ("mapperOccupied", status.mapperOccupied);
        stringObject->setProperty ("mapperNote", status.mapperNoteNumber);
        stringObject->setProperty ("mapperNoteName", getMidiNoteName (status.mapperNoteNumber));
        stringObject->setProperty ("mapperChannel", status.mapperChannel);
        stringObject->setProperty ("mapperFret", status.mapperFret);
        stringObject->setProperty ("voiceActive", status.voiceActive);
        stringObject->setProperty ("voiceNote", status.voiceNoteNumber);
        stringObject->setProperty ("voiceNoteName", getMidiNoteName (status.voiceNoteNumber));
        stringObject->setProperty ("voiceChannel", status.voiceChannel);
        stringObject->setProperty ("voiceFret", status.voiceFret);
        stringArray.add (juce::var (stringObject));
    }

    root->setProperty ("strings", juce::var (stringArray));

    std::array<guitar_ag::AudioEngine::DiagnosticEvent, guitar_ag::AudioEngine::maxDiagnosticEvents> events {};
    auto eventCount = 0;
    audioEngine.copyDiagnosticEvents (events, eventCount);
    root->setProperty ("eventCount", eventCount);

    juce::Array<juce::var> eventArray;

    for (auto index = 0; index < eventCount; ++index)
    {
        const auto& event = events[static_cast<size_t> (index)];
        auto* eventObject = new juce::DynamicObject();
        eventObject->setProperty ("sequence", static_cast<double> (event.sequence));
        eventObject->setProperty ("type", getDiagnosticEventTypeName (event.type));

        if (event.type == 1)
            eventObject->setProperty ("midiKind", getIncomingMidiKindName (event));

        eventObject->setProperty ("sample", static_cast<double> (event.sample));
        eventObject->setProperty ("seconds", currentSampleRate > 0.0 ? static_cast<double> (event.sample) / currentSampleRate : 0.0);
        eventObject->setProperty ("blockSample", event.blockSample);
        eventObject->setProperty ("channel", event.channel);
        eventObject->setProperty ("hostNote", event.hostNoteNumber);
        eventObject->setProperty ("hostNoteName", getMidiNoteName (event.hostNoteNumber));
        eventObject->setProperty ("engineNote", event.engineNoteNumber);
        eventObject->setProperty ("engineNoteName", getMidiNoteName (event.engineNoteNumber));
        eventObject->setProperty ("velocity", event.velocity);
        eventObject->setProperty ("controller", event.controllerNumber);
        eventObject->setProperty ("controllerValue", event.controllerValue);
        eventObject->setProperty ("stringIndex", event.stringIndex);
        eventObject->setProperty ("stringName", getStringName (event.stringIndex));
        eventObject->setProperty ("fret", event.fret);
        eventObject->setProperty ("preferredString", event.preferredString);
        eventObject->setProperty ("strumPreferredString", event.strumPreferredString);
        eventObject->setProperty ("legatoSourceString", event.legatoSourceString);
        eventObject->setProperty ("stolenString", event.stolenString);
        eventObject->setProperty ("stolenNote", event.stolenNoteNumber);
        eventObject->setProperty ("stolenNoteName", getMidiNoteName (event.stolenNoteNumber));
        eventObject->setProperty ("mapperMaskBefore", event.mapperMaskBefore);
        eventObject->setProperty ("mapperMaskAfter", event.mapperMaskAfter);
        eventObject->setProperty ("voiceMaskBefore", event.voiceMaskBefore);
        eventObject->setProperty ("voiceMaskAfter", event.voiceMaskAfter);
        eventObject->setProperty ("dropSemitones", event.dropSemitones);
        eventObject->setProperty ("inputTransposeSemitones", event.inputTransposeSemitones);
        eventObject->setProperty ("neckSlideSemitones", event.neckSlideSemitones);
        eventObject->setProperty ("legatoArticulation", event.legatoArticulation);
        eventArray.add (juce::var (eventObject));
    }

    root->setProperty ("events", juce::var (eventArray));
    return juce::JSON::toString (juce::var (root), false);
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
