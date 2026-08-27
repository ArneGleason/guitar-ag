# Plan 0092 Modal-Pick Listening Handoff — 2026-08-27

## Branch and Safety Boundary

- Branch: `codex/0092-modal-coupled-pick`
- Base: Plan 0091 listening commit `0ea1c1a`
- The modal-pick selector, force scale, state, and render work are compiled only
  into `GuitarAGOfflineRender`.
- The VST3 builds and the normal calibration SHA-256 remains
  `C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`.

## Comparison

Directory:
`C:\code\github\guitar-ag\build-vs2022-x64\diagnostics\eg092-modal-pick`

1. `1_deep-current-additive.wav` — rejected deep-pick reference
2. `2_deep-body-only.wav` — direct picked extras removed
3. `3_deep-modal-force-100.wav` — coupled force at 1.00x
4. `4_deep-modal-force-175.wav` — coupled force at 1.75x
5. `5_deep-modal-contribution-175.wav` — isolated `4 - 2` contribution

All files are stereo 48 kHz and 486,000 samples. Track 2 plus track 5
reconstructs track 4 with infinite measured audio PSNR.

## Focused Listening

- `0:00.0–0:01.2` — low E2: does modal force create a pick/string entry rather
  than a woody click, swell, or phase artifact?
- `0:02.0–0:03.2` — high E4: judge the attack only; the known transposed-low-
  register body problem is intentionally unchanged.
- `0:04.0–0:06.8` — repeated F2: does either force level give useful attack
  variation and material identity without a digital rattle?
- Track 5 at the same times: does the isolated added energy sound like ringing
  string modes, or like another synthetic event despite being state-coupled?

## Decision Gate

If one coupled version improves the full attack and track 5 reads as string
response, refine the modal state/output separation. If both remain synthetic,
stop shaping the plectrum envelope and revisit the modal representation before
production work.

