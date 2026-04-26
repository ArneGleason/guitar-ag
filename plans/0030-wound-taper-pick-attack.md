# Plan 0030 — Wound Taper and Pick Attack

## Goal

Respond to chord/arpeggio listening feedback after KS021 by tapering wound-string behavior per physical string, reducing overly aggressive velocity response, and making attack less hammer-like.

## Scope

- Keep the existing fretboard mapper.
- Add per-string wound amount:
  - low E: 1.00
  - A: 0.67
  - D: 0.33
  - plain strings: 0.00
- Use wound amount to blend string behavior instead of one wound/plain switch.
- Remap full MIDI velocity into the previous 0-68% strike-character range.
- Reduce pick transient and attack chirp intensity.
- Add a short onset ramp to soften impact-like starts.

## Non-Goals

- No new pickup model.
- No compressor.
- No alternate tunings.
- No full pick/string contact simulation yet.

## Result

Implemented as `StringVoice KS-022 WoundTaperPick`.
