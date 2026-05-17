# Plan 0085 — Slide Panic Reset

## Goal

Fix the stuck-feeling state reported after automating `Neck Slide` up to about `+2.4 st` and back to `0.0 st` in Bitwig.

`Neck Slide` should remain a post-assignment pitch gesture. Once it returns to zero and the host stops or rewinds, later notes should be interpreted from the native guitar model again: E2 is low E, and E2+B2 is open low E plus A-string fret 2.

## Implementation

- Snap the smoothed `Neck Slide` engine value to exact zero when the parameter target is neutral.
- Handle standard MIDI panic/reset messages:
  - CC120 All Sound Off;
  - CC121 Reset All Controllers;
  - CC123-127 All Notes Off family.
- On panic, clear active voices, queued note events, articulation notes, finger-noise assignments, feedback focus, and fretboard occupancy.
- Before interpreting a new note-on group, reconcile fretboard occupancy against the actual active string voices so a naturally dead or host-stopped voice cannot leave a string marked occupied forever.
- Keep the `Input Octave` translation before all fretboard logic.

## Verification

- Build `GuitarAGOfflineRender` and `GuitarAG_VST3` Release.
- Render the E2+B2 fixture in DAW-octave mode and confirm max active string voices reaches 2.
- In Bitwig, automate `Neck Slide` up and back to `0.0 st`, stop/rewind, then play E2+B2 with `Input Octave = DAW E2=52`.
