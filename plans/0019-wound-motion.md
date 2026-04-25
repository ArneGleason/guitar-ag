# Plan 0019 — Wound Motion

## Goal

Try a lower-string moving-partial and roughness layer using KS011 as the practical base.

KS012 showed that all-pass phase movement inside the wound-string feedback core moved one phase metric but dulled the wound strings and collapsed moving upper-harmonic behavior. KS013 should avoid that failure mode by removing the wound all-pass stage and adding a separate wound-only motion layer that feeds upper partials, resonance, pickup readout, and only a very light loop injection.

## Scope

- Keep KS011 wound interaction and excitation behavior.
- Remove the KS012 wound all-pass feedback stage.
- Add a wound-only moving partial/roughness layer for E2, A2, and D3.
- Use broad, slightly non-integer resonances to add moving upper content without a global tone change.
- Leave G3, B3, and E4 intentionally unchanged.
- Update the visible model label.
- Build and install the VST3 for another calibration render.

## Out of Scope

- Do not add fretting.
- Do not add MPE.
- Do not add UI controls.
- Do not add amp/cab simulation.
- Do not change the calibration MIDI.

## Implementation Notes

The wound motion layer is separate from the main feedback damping path:

- it only activates for MIDI notes 40, 45, and 50
- it uses four broad resonators around 5.65x, 7.90x, 10.85x, and 13.75x the note frequency
- weighting moves from higher partials toward lower/mid partials over roughly 300-500 ms
- the drive is motion-coupled and includes a small high-passed/slow roughness component
- the output is mixed strongly into pickup readout, lightly into moving resonance, and very lightly into the loop

## Verification

The human should be able to verify:

- [x] The plugin UI shows `StringVoice KS-013 WoundMotion`.
- [x] The plugin still loads in Bitwig.
- [x] The calibration MIDI renders successfully.
- [x] Analysis shows whether E2/A2/D3 regain air/upper energy and moving upper-harmonic behavior compared with KS012.

## Completion Notes

2026-04-25:

- Built and installed `StringVoice KS-013 WoundMotion` at commit `b6fc1ba`.
- Analyzed `/Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-ks013-woundmotion-b6fc1ba-single-note-calibration.wav`.
- The plain-string notes stayed effectively unchanged.
- The lower/wound air and moving upper-harmonic metrics improved, but subjective listening found the added layer too white-noise-like and synthetic.
- Result: useful architecture, but the stochastic roughness drive needs to be reduced and retuned toward metallic ring.

