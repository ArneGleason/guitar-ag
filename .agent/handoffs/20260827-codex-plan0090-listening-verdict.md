# Plan 0090 Human Listening Verdict — 2026-08-27

## Comparison

The human compared these aligned 10.125-second renders in the workbench A/B
tool with loudness matching enabled:

1. `eg090-legacy.wav`
2. `eg090-stateful-preserve.wav`
3. `eg090-stateful-reset.wav`

Saved notes:
`C:\code\github\ag-music-tool-workbench\output\ab_notes\guitar-ag-plan0090-stateful.json`

## Verdict

- Legacy: "Sounds not bad but the attack is kind of spectral chirp and the note body sound a bit glassy."
- Stateful preserve: "Just sound like a synth."
- Stateful reset: "Just sound like a synth."

No timestamp markers were recorded. Both stateful modes failed the overall
guitar-identity gate, so repick-state continuity is not a reason to promote
the current engine.

## Decision and Next Step

- Keep the production VST3 on the legacy modal engine.
- Keep the offline `--string-engine` / `--stateful-repick` harness for research.
- Do not begin physical left-hand/fret-state work on the current stateful voice.
- Next, refine or replace the isolated-note excitation, string loss/dispersion,
  pickup readout, and body response until the stateful candidate reads as a
  useful electric-guitar DI in a focused ear test.
- An amp-sim repeat was not documented in this verdict and remains optional;
  it should not overturn the clean-source failure.
