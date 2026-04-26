# 0049 — Finger Vibrato

## Goal

Add a first fretting-hand pitch vibrato layer before moving into full MPE routing.

## Scope

- Add `Vibrato Speed`, `Vibrato Depth`, and `Vibrato Delay` parameters.
- Add optional MIDI CC1/mod-wheel routing to speed and depth.
- Keep modulation global for now, while preserving per-voice vibrato phase.
- Add a separate editor section for vibrato controls.
- Fix the footer/version text overlap while editing the UI.

## Out of Scope

- MPE per-note pitch bend.
- Whammy-bar behavior.
- Tempo-synced delay values.
- Per-note CC routing.

## Implementation Notes

- `Vibrato Delay` is measured in seconds internally and shown as milliseconds.
- Delay behavior waits for the delay duration, then ramps vibrato in over the same duration.
- Modal oscillator phase increments are modulated per sample by a cents-to-ratio vibrato value.
- CC1 adds to baseline speed/depth only when the corresponding toggle is enabled.

## Verification

- Build `GuitarAGOfflineRender`.
- Render an off-vibrato diagnostic.
- Render a vibrato diagnostic with depth and delay.
- Confirm the WAV files differ.
- Build `GuitarAG_VST3`.
