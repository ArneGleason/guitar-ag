# Plan 0052 — MPE Pitch Bend

## Goal

Implement the original MVP proof point: hold a chord and bend one note independently through MPE note expression.

## Scope

- Add an explicit `MPE Mode` parameter.
- Add a visible `MPE Bend Range` parameter.
- Default MPE bend range to ±48 semitones for Bitwig compatibility.
- Route pitch wheel messages by MIDI channel when MPE mode is enabled.
- Preserve global pitch-wheel whammy behavior when MPE mode is disabled.
- Smooth per-voice MPE pitch bend.
- Extend the offline renderer with MPE mode and bend range flags.

## Out of Scope

- Full MPE zone negotiation.
- Master-channel/global MPE messages.
- MPE pressure and CC74 timbre mapping.
- UI voice/channel debug display.

## Verification

- Build `GuitarAGOfflineRender`.
- Build `GuitarAG_VST3`.
- Render a baseline and MPE-bent diagnostic and confirm the audio changes.
- Manually verify in Bitwig that a note-expression pitch bend on one chord tone leaves the other chord tones stable.
