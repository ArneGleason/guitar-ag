# Plan 0012 — Steel Exciter Harmonics

## Goal

Add a small steel/pick-contact excitation experiment inside `StringVoice`.

The goal is to test the KS005 calibration hypothesis: per-voice pickup readout restored body, but it did not create the missing steel/electric upper partials. This pass changes the string/exciter rather than adding more post-mix filtering.

## Scope

- Add deterministic higher partials to the initial string state.
- Add a short velocity-sensitive pick contact/scrape burst.
- Add a very small soft-clipped slope/contact term in the feedback path.
- Keep pickup readout per voice.
- Update the visible model label.
- Build and install the VST3 for another calibration render.

## Out of Scope

- Do not add UI controls.
- Do not add MPE.
- Do not add amp/cab simulation.
- Do not add a full pick/string collision solver.
- Do not change the render calibration workflow.

## Implementation Notes

This is a pragmatic listening and measurement experiment. The new components should be small enough to avoid turning the DI output into distortion, but present enough to move the sound away from nylon-like smoothness:

- high partials: fixed 5th, 7th, 11th, and 13th harmonic components at note start
- pick contact: short high-passed noise burst scaled by velocity
- slope nonlinearity: small `tanh` term in the delay-line feedback path

## Verification

The human should be able to verify:

- [ ] The plugin UI shows `StringVoice KS-006 SteelExcite`.
- [ ] The plugin still loads in Bitwig.
- [ ] The calibration MIDI renders successfully.
- [ ] The sound is compared against KS005 and Guitar-TECHS DI.

## Completion Notes

2026-04-25:

- Added steel-partial excitation to the initial delay line.
- Added short pick-contact/scrape state to `StringVoice`.
- Added a small soft-clipped slope term to preserve/generate more upper partials.
- Updated model label to `StringVoice KS-006 SteelExcite`.
- Verified the Release VST3 target builds.
