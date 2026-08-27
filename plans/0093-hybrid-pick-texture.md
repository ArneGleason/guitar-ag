# Plan 0093 — Hybrid Modal Pick with Dense Surface Texture

## Goal

Build the human-requested attack candidate: Plan 0092's 1.75x modal-force
foundation plus a much quieter version of the current direct pick texture whose
discrete slip events are dense enough to read as material contact rather than a
sparse woody rattle.

## Scope

- Keep all controls and processing offline-only.
- Add a direct-texture mix for modal-pick mode, defaulting to zero so Plan 0092
  renders remain reproducible.
- Add a picked-texture event-density control. Shorten slip intervals while
  reducing individual impulse size by the square root of density to keep level
  changes secondary to texture changes.
- Apply the direct mix to picked transient, contact noise/ring, grind, coin, and
  heavy-contact states. Do not alter later slide/fret contact.
- Compare the 1.75x modal foundation with equal low-level sparse and dense
  blends, plus an isolated dense hybrid contribution.

## Non-Goals

- No VST3 or preset-state changes.
- No register/formant correction in this comparison.
- No finger-noise redesign.
- No sample playback or convolution from a recorded pick.

## Verification

- [x] Windows Release VST3 and offline renderer build.
- [x] Default calibration remains byte-identical.
- [x] Zero direct mix reproduces the Plan 0092 1.75x modal render.
- [x] Hybrid renders repeat byte-identically.
- [x] Dense contribution reconstruction is exact.
- [ ] Human decides mix amount and whether increased density improves texture.

## Prepared Listening Set — 2026-08-27

All files are stereo 48 kHz, 486,000 samples, and 10.125 seconds:

1. selected Plan 0092 modal-force 1.75x foundation
2. foundation plus 12% direct texture at 2.5x event density
3. foundation plus 22% direct texture at 2.5x event density
4. foundation plus 22% direct texture at the original sparse density
5. isolated dense 22% contribution (`3 - 1`)

Track 1 remains byte-identical to the Plan 0092 1.75x render. The hybrid repeats
byte-identically. Track 1 plus track 5 reconstructs track 3 with infinite
measured audio PSNR. Track 5 measures -56.8 dB mean / -31.4 dB peak before the
A/B tool's loudness matching.

## Decision Gate

- If dense low-level texture improves material identity, retain it as the attack
  detail direction and calibrate it against damped-guitar DI measurements.
- If it restores the woody/digital layer even at low level, omit it and derive
  future texture entirely through modal force roughness.
