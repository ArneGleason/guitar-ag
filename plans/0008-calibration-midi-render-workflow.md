# Plan 0008 — Calibration MIDI Render Workflow

## Goal

Create a fixed MIDI file that can be rendered through Guitar AG in Bitwig for repeatable version-to-version and reference-data comparisons.

## Scope

- Add a script to generate canonical calibration MIDI.
- Commit the generated MIDI file.
- Document the Bitwig render workflow.
- Verify the MIDI contents using the existing MIDI extractor.

## Out of Scope

- Do not analyze rendered audio yet.
- Do not modify the plugin DSP.
- Do not add MPE calibration files yet.

## Implementation Notes

The first calibration file should be simple:

- six guitar-open-string pitch references
- sustained notes for tone/decay analysis
- short notes for release-damping analysis
- fixed velocity
- fixed spacing

## Verification

The human should be able to verify:

- [ ] `tests/midi/single-note-calibration.mid` imports into Bitwig.
- [ ] It plays long and short notes through Guitar AG.
- [ ] `docs/render-calibration.md` explains where to save rendered WAVs.
- [ ] The MIDI extractor reports the expected notes and durations.

## Completion Notes

2026-04-25:

- Added `scripts/create-calibration-midi.py`.
- Generated `tests/midi/single-note-calibration.mid`.
- Added `docs/render-calibration.md`.
- Verified the MIDI with `scripts/extract-midi-notes.py`.
- The file contains 12 note events: six 1.5-second notes and six 0.12-second notes at velocity 100.
