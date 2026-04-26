# Plan 0031 — Clear DI and Velocity Curve

## Goal

Improve KS022 based on listening feedback: add a built-in performance velocity curve and reduce the warm, papery coloration that makes the tone feel less like a clean DI electric guitar.

## Scope

- Keep the fretboard mapper and wound taper.
- Add a simple piecewise velocity curve:
  - 0.00 input -> 0.05 internal floor
  - 0.10 input -> 0.30 internal
  - 0.90 input -> 0.80 internal
  - 1.00 input -> 1.00 internal
- Narrow pickup aperture to preserve more upper harmonic detail.
- Reduce post-mix low-passed body contribution.
- Increase dry/presence balance carefully.

## Non-Goals

- No full electromagnetic pickup model yet.
- No amp/cab or speaker simulation.
- No new UI controls.
- No MPE changes.

## Result

Implemented as `StringVoice KS-023 ClearDI`.
