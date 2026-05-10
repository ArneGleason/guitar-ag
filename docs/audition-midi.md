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

## Slide Gesture Audition MIDI

`tests/midi/guitar-ag-slide-gesture-audition.mid` focuses on the phase-1 slide-authoring workflow from `plans/0064-slide-gesture-notation.md`.

Suggested plugin setup:

- `MPE Mode`: On
- `MPE Bend Range`: `48.0 st`
- `MPE Pressure Amount`: nonzero for the final expression-layering section
- `MPE CC74 Amount`: nonzero for the final expression-layering section

At 100 BPM:

- Bar 2: single held-note `Slide To` using MPE pitch bend.
- Bar 7: slide-ins from indefinite lower and upper approach pitches.
- Bar 10: slide throw up, back, below, and back to center.
- Bar 16: descending and ascending slide-outs before release.
- Bar 22: independent MPE chord slide where only the top note moves.
- Bar 28: manual chord-slide proxy, with matching MPE pitch curves on all chord tones. Use this to compare drawn per-note bends against the global `Neck Slide` lane.
- Bar 35: clean held-chord automation bed for drawing the VST `Neck Slide` lane in the host.
- Bar 42: slide plus pressure/CC74 expression layering. Pressure and CC74 are not guitar slide; this checks that they can coexist with pitch slides.

For the `Neck Slide` automation bed, draw the host parameter lane roughly as:

- Bar 35: `0 st`
- Bar 36-37: ramp to `+5 st`
- Bar 38: ramp down to `-2 st`
- Bar 39: return to `0 st`

For the fretted-slide character, start with `Fret Steps` around 10% to 25%. The control is perceptually remapped, so 10% now reaches the former 90% engagement point and the rest of the slider fine-tunes the audible fret-crossing range. Set `Fret Steps` to 0% when you want to compare against the earlier smooth global pitch lane.

For pressure lift during slides, automate `Slide Lift` while `Neck Slide` is moving. 0% keeps the slide fretted. Around 50% lifts pressure gradually over about half a second. 100% lifts quickly, around 50 ms, fading fret-step engagement into lighter string scrape and damping. Use `Squeak Up` and `Squeak Down` under the `Neck Slide` disclosure to balance upward throws and downward returns independently from the older `Finger Noise` approach/release layer.

For offline smoke checks, `--neck-slide-at <seconds>` starts the render neutral and then sends the `--neck-slide` target during playback. Use `--slide-lift` to compare lifted and fully fretted slide motion without drawing host automation.

Regenerate it with:

```bash
scripts/create-slide-gesture-midi.py
```

Offline render example:

```bash
build/GuitarAGOfflineRender_artefacts/Release/GuitarAGOfflineRender \
  --midi tests/midi/guitar-ag-slide-gesture-audition.mid \
  --output build/diagnostics/guitar-ag-slide-gesture-audition.wav \
  --sample-rate 48000 \
  --block-size 512 \
  --tail-seconds 3.0 \
  --mpe-mode 1 \
  --mpe-bend-range 48 \
  --mpe-pressure-amount 1.0 \
  --mpe-cc74-amount 1.0 \
  --neck-slide 0
```

This file intentionally does not contain host plugin automation. The chord-slide proxy section duplicates MPE pitch curves on each member channel. The separate `Neck Slide` automation bed is the clean place to draw the global host-automation lane in a DAW.

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

## Pick Stroke Audition MIDI

`tests/midi/guitar-ag-pick-stroke-audition.mid` focuses on the `Pick Stroke` and `Pick Bite` controls added across EG-072 through EG-074.

Suggested plugin setup:

- `Pick Stroke`: `Alternate`
- `Legato Articulation`: `0%`
- `Pick Bite`: about 20% to 60%
- `String Age`: to taste, often around 35% to 50%
- Optional: `Player Feel` around 35% to 60% for natural feel, with `Feel Recovery` around 2.0 to 2.5 seconds. Use higher `Player Feel` or longer recovery when auditioning deliberately sloppier playing.

Keep `Legato Articulation` at 0% for the first pass. The file is intended to evaluate picked right-hand behavior, so hammer-on/pull-off substitution would hide the thing being tested.

At 112 BPM:

- Bar 2: open-string low-to-high then high-to-low map.
- Bar 4: same-string tremolo picking on low E and high E.
- Bar 7: adjacent two-string crossings in both directions.
- Bar 12: string-skipping economy-direction checks.
- Bar 16: downstroke then upstroke open-position chord strums.
- Bar 24: compact down/up rhythm-guitar strum groove.
- Bar 29: rolling crosspicking/classical-style arpeggio exercises.
- Bar 32: pedal-tone riff studies with string changes.
- Bar 36: melodic scale fragments across strings.
- Bar 39: mixed finale with strum, run, tremolo, and final chord.

Regenerate it with:

```bash
scripts/create-pick-stroke-audition-midi.py
```

The chord-strum sections sustain each fretted string until that string is struck again or the chord changes. This keeps the MIDI closer to a held left-hand chord shape with right-hand re-articulation instead of short left-hand note gates on every stroke.

EG-078 makes Player Feel less aggressive on these already-staggered strums by treating quick cross-string note clusters as a continuing right-hand stroke. EG-079 adds a separate Auto Strum audition file for simultaneous block chords where the plugin generates the inter-string stroke timing itself.

Offline A/B example:

```bash
build/GuitarAGOfflineRender_artefacts/Release/GuitarAGOfflineRender \
  --midi tests/midi/guitar-ag-pick-stroke-audition.mid \
  --output build/diagnostics/guitar-ag-pick-stroke-alternate.wav \
  --pick-stroke alternate \
  --legato-articulation 0 \
  --pick-bite 0.35 \
  --string-age 0.35 \
  --player-feel 0.50 \
  --player-feel-recovery 2.0 \
  --tail-seconds 2.0
```

Use the header `Export Settings` button to copy a JSON snapshot of the current audition context when reporting useful defaults or suspicious behavior. One human-provided reference snapshot from EG-076 is stored at `docs/audition-settings/20260510-eg076-pick-tone-reference.json`.

## Auto Strum Audition MIDI

`tests/midi/guitar-ag-auto-strum-audition.mid` focuses on the EG-079 `Strum Speed` interpreter.

Suggested plugin setup:

- `Pick Stroke`: start with `Alternate`, then compare forced `Down` and forced `Up`
- `Strum Speed`: compare `0%`, about `40-70%`, and `100%`
- `Player Feel`: around `Pro`/50% if you want the generated stroke to include natural timing and energy variation
- `Legato Articulation`: `0%` for the first pass

At 96 BPM:

- Bar 2: exact same-time block chords. At `Strum Speed = 0%`, these should sound like stacked note-ons; above 0%, Guitar AG should create the right-hand stroke.
- Bar 7: repeated block-chord groove. This checks whether `Pick Stroke = Alternate` feels like down/up hand motion without writing staggered notes into the MIDI.
- Bar 12: partial-string block grips and skipped strings.
- Bar 20: single-note control line. `Strum Speed` should not change this section.
- Bar 24: final held block chord.

Regenerate it with:

```bash
scripts/create-auto-strum-audition-midi.py
```

Offline A/B example:

```bash
build/GuitarAGOfflineRender_artefacts/Release/GuitarAGOfflineRender \
  --midi tests/midi/guitar-ag-auto-strum-audition.mid \
  --output build/diagnostics/guitar-ag-auto-strum-60.wav \
  --pick-stroke alternate \
  --strum-speed 0.60 \
  --player-feel 0.50 \
  --player-feel-recovery 2.0 \
  --tail-seconds 3.0
```

The first implementation only groups note-ons that share the exact same sample. If a host imports or emits chord notes a few samples apart, those notes will behave like normal authored timing until a future tolerance-window pass exists.

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
