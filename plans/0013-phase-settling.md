# Plan 0013 — Phase Settling

## Goal

Add one isolated time-varying string property: harmonic-dependent phase settling.

The KS006 harmonic-evolution analysis showed that Guitar-TECHS DI notes have much more phase/pitch drift across harmonics than the current model. This pass changes only the feedback-loop phase behavior so the next calibration render can test that property in isolation.

## Scope

- Keep the KS006 exciter, pickup readout, and tone stage unchanged.
- Add a first-order all-pass stage in the `StringVoice` feedback loop.
- Move the all-pass coefficient over the first few hundred milliseconds of each note.
- Update the visible model label.
- Build and install the VST3 for another calibration render.

## Out of Scope

- Do not add more static brightness.
- Do not add another pickup model.
- Do not add UI controls.
- Do not implement MPE.
- Do not add amp/cab simulation.
- Do not add a full string stiffness solver.

## Implementation Notes

The all-pass stage is intended to create frequency-dependent phase movement without directly changing amplitude response.

Initial values:

- coefficient starts at about `0.030` to `0.045`, depending on velocity
- coefficient settles to about `0.120` to `0.150`, depending on velocity
- settling time is about `260` to `380` ms
- coefficient is clamped to `[-0.35, 0.35]`

## Verification

The human should be able to verify:

- [ ] The plugin UI shows `StringVoice KS-007 PhaseSettle`.
- [ ] The plugin still loads in Bitwig.
- [ ] The calibration MIDI renders successfully.
- [ ] Harmonic-evolution analysis shows whether phase drift moved closer to the DI reference.

## Completion Notes

2026-04-25:

- Added a time-varying first-order all-pass stage to the string feedback loop.
- Left the KS006 exciter and pickup/tone behavior unchanged.
- Updated model label to `StringVoice KS-007 PhaseSettle`.
- Verified the Release VST3 target builds.
