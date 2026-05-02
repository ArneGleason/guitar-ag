# Feature Audition MIDI

## Purpose

`tests/midi/guitar-ag-feature-audition.mid` is a structured listening clip for the current Guitar AG VST3.

It is meant for quick DAW auditioning after a build, not for musical beauty. It steps through the parts of the instrument that are easiest to verify from MIDI: velocity response, wound/plain string character, polyphony, short releases, mod-wheel vibrato, key/poly aftertouch bend, MPE pitch bend, MPE pressure, and MPE CC74.

## Suggested Plugin Setup

Use these settings for the first pass:

- `MPE Mode`: On
- `MPE Bend Range`: `48.0 st`
- `MPE Pressure Amount`: about 65% or higher
- `MPE CC74 Amount`: about 65% or higher
- `Aftertouch Bend`: default `+2 st`
- Optional for segment 4: enable `Mod Wheel To Depth` and/or `Mod Wheel To Speed`

The file uses MIDI channels 2-7 for the MPE-style sections. If a DAW collapses all imported MIDI to one channel, the independent MPE sections will not behave independently.

## Segment Map

At 120 BPM:

- Bar 2: open strings, soft then assertive velocity, from low E to high E.
- Bar 7: strummed chords and arpeggios for polyphony, fretboard assignment, and sustain.
- Bar 15: short picked riff for release damping and fast note handling.
- Bar 18: CC1/mod-wheel vibrato ramp. This is audible only if the VST mod-wheel vibrato toggles are enabled.
- Bar 23: key/poly aftertouch bends only the top note of a held chord.
- Bar 28: MPE pitch bend bends the middle note of a held chord while the other notes stay stable.
- Bar 34: MPE channel pressure and CC74 change two different held notes.
- Bar 40: MPE lead line with small slide-ins, bends, and releases.
- Bar 44: wide final chord with a long sustain tail.

## Regenerating

```bash
scripts/create-feature-audition-midi.py
```

## Offline Render Example

```bash
build/GuitarAGOfflineRender_artefacts/Release/GuitarAGOfflineRender \
  --midi tests/midi/guitar-ag-feature-audition.mid \
  --output build/diagnostics/guitar-ag-feature-audition.wav \
  --sample-rate 48000 \
  --block-size 512 \
  --tail-seconds 4.0 \
  --mpe-mode 1 \
  --mpe-bend-range 48 \
  --mpe-pressure-amount 1.0 \
  --mpe-cc74-amount 1.0
```

The offline render is useful for a quick sanity check, but the DAW audition is still the better test for imported MIDI-channel behavior, plugin parameters, and host MPE handling.
