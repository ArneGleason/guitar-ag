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

## Offline Performance Report

Add `--perf-report` to the offline renderer to print timing and voice-activity diagnostics:

```bash
build/GuitarAGOfflineRender_artefacts/Release/GuitarAGOfflineRender \
  --midi tests/midi/guitar-ag-player-articulation-audition.mid \
  --output build/diagnostics/guitar-ag-player-articulation-perf.wav \
  --sample-rate 48000 \
  --block-size 512 \
  --tail-seconds 2.0 \
  --legato-articulation 0.75 \
  --amp-feedback 1.0 \
  --perf-report
```

The report includes rendered seconds, render time, realtime factor, average/max block render time, average/max active string voices, and finger-noise voice activity. It is intended for comparing CPU-sensitive changes before installing a new VST3 build.

## Player Articulation Audition MIDI

`tests/midi/guitar-ag-player-articulation-audition.mid` focuses on the `Legato Articulation` control added in `StringVoice EG-048 PlayerArtic`.

It contains marked sections for:

- picked reference phrases
- ascending hammer-on candidates
- descending pull-off candidates
- mixed hammer-on/pull-off licks
- guitar-like arpeggios with string changes
- right-hand tap and pull-off cycles
- a fast tap-arpeggio flourish

Suggested audition pass:

- Render or play once with `Legato Articulation` at 0%.
- Repeat at about 35% for pull-offs and simple hammer-ons.
- Repeat at about 65% for longer legato chains.
- Repeat at 100% for tap-heavy behavior.

Regenerate it with:

```bash
scripts/create-player-articulation-midi.py
```

Offline A/B example:

```bash
build/GuitarAGOfflineRender_artefacts/Release/GuitarAGOfflineRender \
  --midi tests/midi/guitar-ag-player-articulation-audition.mid \
  --output build/diagnostics/guitar-ag-player-articulation-100.wav \
  --legato-articulation 1.0 \
  --tail-seconds 2.0
```

## Amp Feedback Audition

`Amp Feedback` is easiest to judge on longer held notes and then on the player-articulation file.

Suggested audition pass:

- Start with `Amp Feedback` at 0% to confirm the pickup/articulation baseline.
- Try 25-40% for subtle loud-rig sustain.
- Try 60-80% for obvious harmonic emphasis and early loop takeover.
- Use 100% as the stress-test/high-feedback sound rather than the default musical setting.
- For EG-050 and later, hold single notes long enough for a dominant feedback band to emerge; very short legato phrases may not give the loop time to take over.
- For EG-055 and later, also listen on held chords: the high-feedback loop should favor one physical string/harmonic more than the others.
- For EG-056 and later, listen to the start of picked chords separately from the sustain: feedback should stay down during the attack, then bloom toward the slider setting after the chord settles.
- `Distorted Return` defaults on in EG-057 and later because it sounds more like a clipped amp signal driving the guitar.
- Toggle `Distorted Return` off when you want to compare the cleaner return or diagnose the feedback loop.

Offline A/B examples:

```bash
build/GuitarAGOfflineRender_artefacts/Release/GuitarAGOfflineRender \
  --midi tests/midi/single-note-calibration.mid \
  --output build/diagnostics/guitar-ag-eg049-single-feedback100.wav \
  --amp-feedback 1.0 \
  --tail-seconds 5.0
```

```bash
build/GuitarAGOfflineRender_artefacts/Release/GuitarAGOfflineRender \
  --midi tests/midi/guitar-ag-player-articulation-audition.mid \
  --output build/diagnostics/guitar-ag-eg050-feedback100.wav \
  --legato-articulation 1.0 \
  --amp-feedback 1.0 \
  --feedback-return-distorted 1 \
  --tail-seconds 3.5
```
