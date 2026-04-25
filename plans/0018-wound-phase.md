# Plan 0018 — Wound Phase

## Goal

Add a wound-only phase/dispersion experiment for the lower open-string notes.

KS011 improved lower-string air and upper energy, but it did not materially change the phase-drift metric. KS012 should test whether a gentle, motion-coupled phase stage inside the wound-string feedback loop makes E2, A2, and D3 behave less like static Karplus-Strong strings while leaving G3, B3, and E4 close to KS011/KS010.

## Scope

- Keep KS011 wound interaction and excitation behavior.
- Add a wound-only all-pass stage inside the string feedback path.
- Move the all-pass coefficient over the first few hundred milliseconds of the note.
- Add a very small motion/noise modulation to the coefficient so the phase behavior is not perfectly deterministic.
- Update the visible model label.
- Build and install the VST3 for another calibration render.

## Out of Scope

- Do not add fretting.
- Do not add MPE.
- Do not add UI controls.
- Do not change the plain-string path intentionally.
- Do not add amp/cab simulation.

## Implementation Notes

The phase stage is deliberately gentler than the earlier global KS007 phase-settling experiment:

- it only activates for MIDI notes 40, 45, and 50
- it uses a first-order all-pass in the feedback path
- the base coefficient settles from roughly 0.032-0.044 to 0.088-0.106
- a decaying, motion-coupled modulation is added to the coefficient
- the coefficient is clamped to 0.015-0.135 for stability

## Verification

The human should be able to verify:

- [x] The plugin UI shows `StringVoice KS-012 WoundPhase`.
- [x] The plugin still loads in Bitwig.
- [x] The calibration MIDI renders successfully.
- [x] Analysis shows whether E2/A2/D3 gain phase drift or evolving harmonic behavior without badly dulling the tone.

## Completion Notes

2026-04-25:

- Built and installed `StringVoice KS-012 WoundPhase` at commit `4586f64`.
- Analyzed `/Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-ks012-woundphase-4586f64-single-note-calibration.wav`.
- Plain-string notes stayed effectively unchanged, so the experiment was isolated correctly.
- Lower/wound mean absolute phase drift improved, but upper/air energy and moving upper-harmonic behavior worsened.
- Result: useful diagnostic, but not a preferred base for the next tone pass.
