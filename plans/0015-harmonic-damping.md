# Plan 0015 — Harmonic Damping

## Goal

Add one isolated harmonic-dependent damping property.

The KS008 moving-resonance experiment improved spectral shape/air but did not create enough time movement by itself. This pass keeps KS008 intact and adds a dynamic high-frequency feedback damping tilt so the next render can test whether partial-dependent decay moves closer to the DI references.

## Scope

- Keep the KS008 moving resonance layer.
- Add a feedback-path damping tilt that separates low and high components.
- Start nearly neutral at note-on and gradually damp the high component more strongly.
- Update the visible model label.
- Build and install the VST3 for another calibration render.

## Out of Scope

- Do not add MPE.
- Do not add UI controls.
- Do not add amp/cab simulation.
- Do not add phase-settling all-pass behavior.
- Do not add new excitation or pickup models.

## Implementation Notes

The feedback sample is split with a one-pole lowpass. The high component is multiplied by a gain that starts near `1.0` and settles lower over the first few hundred milliseconds.

Initial values:

- high feedback gain start: `0.9995`
- high feedback gain target: about `0.980` to `0.984`, depending on velocity
- transition time: about `420` to `620` ms
- split lowpass coefficient: `0.18`

## Verification

The human should be able to verify:

- [ ] The plugin UI shows `StringVoice KS-009 HarmDamp`.
- [ ] The plugin still loads in Bitwig.
- [ ] The calibration MIDI renders successfully.
- [ ] Harmonic-evolution analysis shows whether upper harmonic decay and early-to-late spectral contrast changed without adding phase settling.

## Completion Notes

2026-04-25:

- Added a dynamic feedback damping tilt to `StringVoice`.
- Left KS008 moving resonance in place.
- Did not reintroduce KS007 phase settling.
- Updated model label to `StringVoice KS-009 HarmDamp`.
- Verified the Release VST3 target builds.
