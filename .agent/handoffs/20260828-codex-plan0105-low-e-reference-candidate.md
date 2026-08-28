# Plan 0105 Low-E Reference Candidate Handoff

Date: 2026-08-28
Lead: Codex on unassigned Windows PC
Branch: `codex/0105-low-e-reference-candidate`
Implementation commit: `1d0a99ed3b41614f2fbd591d7d0606a5c60ba725`

## Outcome

Implemented an offline-only, evidence-driven low-E candidate without changing
the production VST3 model. The selected renderer settings are fixed pluck
position 0.14 and 1.50x long-lived harmonic-body decay time, evaluated at the
capture-matched Humbucker/neck pickup setting.

The candidate improves the measured low-E residual but is intentionally not
promoted. The downstroke centroid moves from 537 to 367 Hz versus 262 Hz in the
reference, and sub-500 Hz energy moves from 64.0% to 82.3% versus 90.8%. Decay
moves from -2.75 to -1.68 dB/s. Alternate context also improves; upstroke
spectrum improves less and retains a high-band residual.

## Human Gate

Listen to the ignored local files under:

```text
build-vs2022-x64/diagnostics/eg0105-low-e-reference-candidate/listening
```

`01-current-eg089-neck-humbucker.wav` and
`02-reference-candidate-p014-decay150.wav` are 18.9-second, -18 dBFS RMS-matched
tracks. Files 03 and 04 concatenate both playback orders to reduce order bias.
Judge whether the candidate is more guitar-like rather than merely darker and
longer. Do not promote it or expand captures until the verdict is explicit.

## Verification

- Windows Release `GuitarAG_VST3` and `GuitarAGOfflineRender` build.
- The VST3 target excludes `GUITAR_AG_ENABLE_OFFLINE_ABLATION` and contains no
  new CLI strings or parameter exposure.
- Default and explicit-neutral pickup-matched renderer output exactly reproduce
  Plan 0104 SHA-256
  `BF1A101EFF7EF0407590C9C15F8B41AD4CF4117607E7467AF8462AF1A3975DFE`.
- Reference-capture script unit tests: 3 passed.
- New analysis/listening scripts compile and ran end-to-end on the approved
  evidence and rendered WAVs.
- CTest is configured with no C++ tests in this build tree.

## Reviewer Focus

- Confirm the new setters, fields, and math remain behind the offline compile
  definition.
- Challenge whether a fixed 0.14 position suppresses useful direction/velocity
  geometry even if this focused A/B sounds better.
- Treat upstroke high-band performance as an explicit limitation.
- Do not interpret the fretting-hand-damped reference as palm mute or request a
  damping implementation as part of this candidate.
