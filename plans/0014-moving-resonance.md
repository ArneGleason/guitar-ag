# Plan 0014 — Moving Resonance

## Goal

Add one isolated time-varying amplitude/resonance property.

The KS007 phase-settling experiment improved phase drift but reduced upper-harmonic energy and made upper-harmonic emphasis more static. This pass intentionally does not carry the KS007 all-pass stage forward. It returns to the KS006-style string path and adds a short moving resonance layer so the next render can test amplitude/resonance movement by itself.

## Scope

- Keep the KS006 steel exciter, pickup readout, and tone stage.
- Remove the KS007 all-pass phase-settling stage from this test build.
- Add three short-lived resonators tied to the active note's harmonic series.
- Shift resonance weighting over the first few hundred milliseconds.
- Update the visible model label.
- Build and install the VST3 for another calibration render.

## Out of Scope

- Do not add MPE.
- Do not add UI controls.
- Do not add amp/cab simulation.
- Do not combine this with the KS007 phase-settling result yet.
- Do not add a full body, bridge, or pickup-circuit model.

## Implementation Notes

The moving resonance layer uses fixed resonators at the 5th, 7th, and 11th harmonics of the active note.

The weight starts higher on the 11th harmonic, passes through the 7th harmonic, and settles toward the 5th harmonic. This is a pragmatic way to test whether moving upper-harmonic emphasis helps the render move toward the Guitar-TECHS DI behavior.

Initial values:

- harmonic resonators: 5th, 7th, 11th
- resonance radii: 0.9895, 0.9880, 0.9860
- resonance envelope: 0.012 to 0.057, scaled by velocity
- resonance movement time: about 360 to 520 ms

## Verification

The human should be able to verify:

- [ ] The plugin UI shows `StringVoice KS-008 MovingRes`.
- [ ] The plugin still loads in Bitwig.
- [ ] The calibration MIDI renders successfully.
- [ ] Harmonic-evolution analysis shows whether moving upper-harmonic emphasis improved without the KS007 phase all-pass.

## Completion Notes

2026-04-25:

- Added a moving upper-harmonic resonance layer to `StringVoice`.
- Removed the KS007 all-pass stage from this isolated experiment.
- Updated model label to `StringVoice KS-008 MovingRes`.
- Verified the Release VST3 target builds.
