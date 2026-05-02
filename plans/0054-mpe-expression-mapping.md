# Plan 0054 — MPE Pressure And CC74 Mapping

## Goal

Finish the next small MPE expression step after per-note pitch bend: route pressure and CC74/timbre to the intended active voice.

## Scope

- Add MPE/channel-pressure routing by MIDI channel.
- Add CC74/timbre routing by MIDI channel.
- Add simple VST controls for pressure amount and CC74 amount.
- Keep normal MIDI fallback usable: on a normal one-channel keyboard, pressure/CC74 act like channel-scoped/global expression.
- Keep the mapping conservative so the base tone does not change unless expression data is sent.

## Out Of Scope

- Full MPE zone negotiation.
- Master-channel/global MPE semantics.
- Voice/channel debug UI.
- Hammer-ons, pull-offs, tapping, or phrase-level interpretation.

## Implementation Notes

Pressure should add a modest held-note lift: sustain, level, and upper-mode intensity.

CC74 should behave as a timbre lane: higher values lean the held voice brighter and more bridge-like without changing unrelated voices.

## Verification

- Build `GuitarAG_VST3`.
- Build `GuitarAGOfflineRender`.
- Render baseline, pressure, and CC74 diagnostics.
- In Bitwig, enable MPE and verify pressure/CC74 on one chord tone changes only that tone when notes are on separate member channels.
