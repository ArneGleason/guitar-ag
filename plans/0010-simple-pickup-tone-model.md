# Plan 0010 — Simple Pickup Tone Model

## Goal

Add the first electric-guitar-style pickup/tone shaping stage after the string voices.

The goal is to test whether a simple pickup-position readout and presence shaping moves the sound toward steel/electric character compared with `StringVoice KS-003`.

## Scope

- Add a small post-string tone component.
- Keep it fixed and parameter-free for the first test.
- Update the visible model label.
- Build and install the VST3 for the same calibration render workflow.
- Document the model and expected listening test.

## Out of Scope

- Do not add UI controls.
- Do not implement pickup switching or multiple pickups.
- Do not add amp/cab simulation.
- Do not implement MPE.
- Do not change the string voice unless needed for integration.

## Implementation Notes

This is a pragmatic tone experiment, not a final pickup simulation.

The first model should include:

- a bridge-ish pickup position delay/comb readout
- a string-velocity-style component
- a little high-pass/presence emphasis
- conservative output gain

## Verification

The human should be able to verify:

- [ ] The plugin UI shows the new model label.
- [ ] The plugin still loads in Bitwig.
- [ ] The calibration MIDI renders successfully.
- [ ] The sound is compared against the previous KS-003 render and Guitar-TECHS DI.

## Completion Notes

2026-04-25:

- Added `ElectricGuitarTone` after mixed string voices.
- Added a fixed pickup delay/comb readout, velocity-style difference component, and presence shaping.
- Updated model label to `StringVoice KS-004 Pickup-001`.
