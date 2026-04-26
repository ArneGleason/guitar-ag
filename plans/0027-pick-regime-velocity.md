# Plan 0027 — Pick Regime Velocity

## Goal

Make velocity change the character of the picked string, not only loudness.

KS017 has a promising electric-string ring, but listening feedback says it behaves like the same old string plucked gently with a soft pick at every velocity. This experiment should make low velocities sound tentative/soft and high velocities sound like the string is struck harder.

## Scope

- Add a velocity-ladder MIDI file for open strings.
- Keep the KS017 modal-cluster core.
- Map velocity into pluck position, contact width, harmonic tilt, side-mode strength, and short-lived attack modes.
- Render a velocity-ladder WAV for audition.
- Add a small analyzer for the velocity ladder.

## Out of Scope

- No MPE.
- No fretting model.
- No amp/cab simulation.
- No UI controls.
- No ML critic yet.

## Verification

- [x] Velocity-ladder MIDI exists and extracts 36 note events.
- [x] Offline renderer builds.
- [x] KS018 single-note calibration render succeeds.
- [x] KS018 velocity-ladder render succeeds.
- [x] Velocity-ladder analysis shows velocity changes more than RMS.
- [x] VST3 installed for Bitwig audition.

## Completion Notes

2026-04-25:

- Added `tests/midi/velocity-ladder-open-strings.mid`.
- Added `scripts/analyze-velocity-ladder.py`.
- Implemented first velocity-regime mapping as `StringVoice KS-018 PickRegimes`.
- Offline render for velocity-ladder audition:

```text
/Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-ks018-pickregimes-dirty-velocity-ladder-open-strings.wav
```

- Velocity-ladder summary:

```text
/Users/arnegleason/code/reference-audio/Guitar-AG-renders/ks018-pickregimes-dirty-velocity-ladder-summary.md
```
