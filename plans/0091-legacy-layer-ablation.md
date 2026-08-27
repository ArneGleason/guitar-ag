# Plan 0091 — Legacy Attack and Finger-Layer Ablation

## Goal

Improve the accepted legacy modal guitar sound by first identifying whether its
spectral-chirp attack and mirror-like glassiness come from the modal string body,
the explicit attack modes, the post-modal pick/contact layers, or the separate
global finger-noise generator.

## Premise

Human listening rejected the Plan 0090 stateful waveguide as synth-like. The
next work should diagnose and improve the engine that already reads more like a
guitar rather than iterate the rejected replacement.

The current legacy render is explicitly layered:

1. modal string bank, including short chirp modes
2. separate pick-transient output
3. separate per-voice contact output
4. separate global finger-noise voices mixed before the shared tone stage

`String Age` changes both modal damping and the overlay layers, so it can hide
glassiness without identifying its source.

## Scope

- Add offline-only switches for the legacy attack-mode, pick-transient, and
  per-voice contact layers.
- Keep the VST3 compilation and all default offline output unchanged.
- Create aligned, equal-length ablation renders and subtraction residuals.
- Use one focused fixture for picked attacks and one for finger approach/release
  noises.
- Route the comparisons through the workbench A/B tool with loudness matching.

## Out of Scope

- No replacement string engine.
- No production parameter or UI change.
- No modal-state preservation yet.
- No subjective retuning until the human identifies which layer is responsible.

## Verification

- [x] Windows Release VST3 and offline renderer build.
- [x] Default legacy calibration output remains byte-identical (`C67DCE0C...48B`).
- [x] Every A/B file is aligned, equal length, and distinctly named.
- [x] Residual files contain the aligned output-domain difference introduced by each ablation.
- [ ] Human records a verdict on attack origin and finger-noise translation.

## Decision Gate

- If removing an overlay fixes the perceived attack/glassiness, redesign that
  overlay around energy injected into the modal string or a less independently
  tonal contact response.
- If the string-only body remains glassy after the attack settles, investigate
  modal spacing, fixed phase relationships, decay regularity, and subtle
  time-varying/coupled behavior without using `String Age` as a blanket low-pass.
- Do not proceed to persistent modal repicks until the isolated picked-note
  entry and body pass this listening gate.
