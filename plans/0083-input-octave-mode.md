# Plan 0083 — Input Octave Mode

## Goal

Let the human author guitar notes using the DAW's visible guitar octave names even when the DAW labels MIDI octaves one octave lower than scientific pitch.

## Scope

- Add an `Input Octave` plugin parameter.
- Apply the selected note-number correction before fretboard mapping and voice routing.
- Keep a standard MIDI mode for existing files and external workflows.
- Add the same control to the offline renderer for calibration renders.

## Result

- `Input Octave = MIDI E2=40` leaves MIDI note numbers unchanged.
- `Input Octave = DAW E2=52` subtracts 12 semitones before note-on, note-off, key/poly aftertouch, Player Feel, finger-noise assignment, and Auto Strum grouping.
- New plugin instances default to `DAW E2=52` for the current Bitwig audition workflow.
- The visible model label is `StringVoice EG-083 InputOctave`.

## Verification

- Build `GuitarAG_VST3` and `GuitarAGOfflineRender`.
- Confirm the editor header shows `StringVoice EG-083 InputOctave`.
- In Bitwig, with `Input Octave = DAW E2=52`, piano-roll E2 should sound like the low guitar E that previously required Bitwig E1.
- E2+B2 in DAW-label mode should ring as low E plus B on the A string.
- Existing MIDI files authored around MIDI note 40 should still work with `Input Octave = MIDI E2=40`.
