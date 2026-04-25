# Plan 0009 — Render and Reference Comparison

## Goal

Compare a rendered Guitar AG calibration WAV against Guitar-TECHS DI single-note references to guide the next sound-modeling step.

## Scope

- Locate the rendered Guitar AG calibration WAV.
- Extract calibration MIDI note timings.
- Add a dependency-free comparison script for render/reference note regions.
- Generate a CSV comparison outside the repo.
- Use the results plus listening feedback to form the next DSP hypothesis.

## Out of Scope

- Do not modify the plugin DSP in this plan.
- Do not add external Python dependencies.
- Do not claim the simple metrics fully describe tone.

## Implementation Notes

Use the current render:

```text
/Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-ks003-e35bf11-single-note-calibration.wav
```

Compare long notes first. Short-note comparison can follow once long-note tone is understood.

## Verification

The human should be able to verify:

- [ ] The comparison script runs on the render and Guitar-TECHS DI reference.
- [ ] The generated CSV is outside the repo.
- [ ] The analysis produces a concrete next-step hypothesis.

## Completion Notes

2026-04-25:

- Found the user render at `/Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-ks003-e35bf11-single-note-calibration.wav`.
- Added `scripts/compare-calibration-render.py`.
- Generated calibration note timings at `/Users/arnegleason/code/reference-audio/Guitar-AG-renders/single-note-calibration-notes.csv`.
- Generated render manifest at `/Users/arnegleason/code/reference-audio/Guitar-AG-renders/render-manifest.csv`.
- Generated comparisons:
  - `/Users/arnegleason/code/reference-audio/Guitar-AG-renders/ks003-vs-p1-di-loudest.csv`
  - `/Users/arnegleason/code/reference-audio/Guitar-AG-renders/ks003-vs-p1-di-harmonics.csv`
- The first harmonic-balance pass supports the listening impression that `StringVoice KS-003` is too smooth/nylon-like: upper-harmonic energy is much lower than the real DI reference for the calibration notes.
