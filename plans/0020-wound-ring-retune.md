# Plan 0020 — Wound Ring Retune

## Goal

Retune the KS013 wound-motion layer so it reads more like metallic wound-string ring and less like a short white-noise envelope.

The user's KS013 listening feedback is the primary driver: lower strings gained spectral activity, but the added layer sounded synthetic because the tail was too hiss-like. The analysis supported that KS013 improved air and moving harmonic metrics, so the next step should not remove the moving-partial architecture. It should reduce stochastic noise drive and make the resonant/string-coupled part carry more of the sound.

## Scope

- Keep the KS013 separate wound-motion architecture.
- Reduce high-passed noise and slow random texture drive.
- Drive the wound-motion layer more from string slope and wound interaction output.
- Increase resonator radii so the layer rings more than hisses.
- Reduce direct pickup and loop coupling so the layer is less exposed.
- Update the visible model label.
- Build and install the VST3 for another calibration render.

## Out of Scope

- Do not add fretting.
- Do not add MPE.
- Do not add UI controls.
- Do not add amp/cab simulation.
- Do not change the calibration MIDI.

## Implementation Notes

Initial retune:

- high-passed noise drive: 0.055, down from 0.35
- slow texture drive: 0.10, down from 0.28
- resonator radii increased to 0.9935, 0.9928, 0.9918, and 0.9905
- motion envelope lowered to 0.006-0.026, scaled by velocity
- decay lengthened to about 0.999925-0.99994
- loop injection reduced to 0.018
- pickup readout mix reduced to 0.52

## Verification

The human should be able to verify:

- [x] The plugin UI shows `StringVoice KS-014 WoundRing`.
- [x] The plugin still loads in Bitwig.
- [x] The calibration MIDI renders successfully.
- [x] The lower strings sound less like a short hiss envelope and more like metallic/string-coupled ring.

## Completion Notes

2026-04-25:

- Built and installed `StringVoice KS-014 WoundRing` at commit `4188b0a`.
- Analyzed `/Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-ks014-woundring-4188b0a-single-note-calibration.wav`.
- KS014 reduced the KS013 air/noise boost but did not create a meaningfully more realistic open-string sound.
- Result: stop retuning this additive wound-layer branch and try a more fundamental string/pickup experiment.

