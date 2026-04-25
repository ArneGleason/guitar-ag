# Plan 0011 — Per-Voice Pickup Readout

## Goal

Move the pickup-position experiment from a fixed post-mix comb into each string voice.

The goal is to keep the useful electric-guitar harmonic direction from `StringVoice KS-004 Pickup-001` while avoiding the thin/body loss caused by one fixed delay after all notes are mixed.

## Scope

- Add a simple pickup readout point inside `StringVoice`.
- Scale pickup offset with the active note's delay length.
- Keep the post-mix `ElectricGuitarTone` stage as lighter conditioning.
- Update the visible model label.
- Build and install the VST3 for another calibration render.
- Document the model and expected listening test.

## Out of Scope

- Do not add UI controls.
- Do not add multiple pickups or pickup switching.
- Do not implement a full magnetic pickup/circuit model.
- Do not add amp/cab simulation.
- Do not implement MPE.

## Implementation Notes

The pickup readout should remain deliberately simple:

- choose a fixed normalized pickup position per voice
- read the delay line at that offset
- blend displacement-like and velocity-like components
- leave final gain conservative enough for DAW testing

This keeps the experiment real-time safe and reversible while making the pickup behavior depend on each note's effective string length.

## Verification

The human should be able to verify:

- [ ] The plugin UI shows `StringVoice KS-005 Pickup-Voice`.
- [ ] The plugin still loads in Bitwig.
- [ ] The calibration MIDI renders successfully.
- [ ] The sound is compared against the KS004 render and Guitar-TECHS DI.

## Completion Notes

2026-04-25:

- Added a per-voice pickup readout offset at about 0.18 of each voice delay length.
- Blended pickup displacement and velocity-like readout inside `StringVoice`.
- Simplified `ElectricGuitarTone` into post-mix high-pass/presence/body conditioning.
- Updated model label to `StringVoice KS-005 Pickup-Voice`.
- Verified the Release VST3 target builds.
