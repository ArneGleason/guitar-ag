# Plan 0050 — Whammy Pitch Wheel

## Goal

Add a standard MIDI pitch-wheel mode that behaves like a global whammy bar while leaving MPE per-note pitch bend for a later milestone.

## Scope

- Move build/version identity from the crowded footer to the header.
- Add a Whammy editor section.
- Add an enable switch for pitch-wheel whammy behavior.
- Add up/down bend range parameters.
- Add a string-spread parameter for imperfect string response during larger bends.
- Extend the offline renderer so whammy behavior can be rendered without a DAW.

## Out of Scope

- MPE per-note pitch bend.
- Per-string whammy calibration tables.
- Detailed bridge/spring/nut/saddle physics.
- Whammy-bar return hysteresis or tuning drift.

## Implementation Notes

- Pitch wheel is handled immediately like mod wheel, even when note lookahead is enabled.
- The bend range defaults are +6 semitones upward and -12 semitones downward.
- `Whammy String Spread` is a controlled imperfection layer, not a full tremolo-bridge simulator.

## Verification

- Build `GuitarAGOfflineRender`.
- Build `GuitarAG_VST3`.
- Render at least one calibration file with pitch wheel centered and one with pitch wheel offset.
- Install the VST3 bundle for Bitwig auditioning.
