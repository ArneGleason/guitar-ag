# Plan 0084 — Chord Legato Guard

## Goal

Prevent legal block-chord tones from being converted into same-string legato gestures when `Legato Articulation` is raised.

The immediate audition case is Bitwig piano-roll `E2+B2` with `Input Octave = DAW E2=52`, which should become native guitar `E2+B2` and ring as open low E plus A-string fret 2.

## Implementation

- Keep the input-octave correction as a MIDI-ingestion step only.
- Preserve the chord-aware fretboard preview from Plan 0082.
- When Auto Strum/chord grouping provides an explicit string assignment, bypass legato-source lookup for that generated note-on.
- Require a recent note to have existed briefly before it can become a hammer-on, pull-off, or tap source.

## Verification

- Offline E2+B2 fixture in `DAW E2=52` mode, default controls: max active string voices remains 2.
- Same fixture with `Legato Articulation = 100%`: max active string voices is 2 rather than collapsing to 1.
- Build and install the VST3 so Bitwig can confirm `E2+B2` rings as two legal strings.
