# Plan 0056 — Feature Audition MIDI

## Goal

Create a single structured MIDI file that can be dragged into a DAW to audition the current Guitar AG feature set.

## Scope

- Add a generated MIDI file under `tests/midi/`.
- Add a script so the MIDI can be regenerated after future edits.
- Include segment markers and a guide document with suggested plugin setup.
- Cover features that MIDI can actually drive: note dynamics, polyphony, release behavior, CC1, key/poly aftertouch, MPE pitch bend, channel pressure, and CC74.

## Out Of Scope

- Direct VST parameter automation inside the MIDI file.
- Preset creation.
- Host-specific Bitwig project files.
- New DSP or articulation behavior.

## Result

Implemented as:

- `scripts/create-feature-audition-midi.py`
- `tests/midi/guitar-ag-feature-audition.mid`
- `docs/audition-midi.md`

The file is intended to be used with `MPE Mode` on and `MPE Bend Range` set to `48.0 st`.
