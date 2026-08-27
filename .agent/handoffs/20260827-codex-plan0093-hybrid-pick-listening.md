# Plan 0093 Hybrid Pick Listening Handoff — 2026-08-27

## Branch and Safety Boundary

- Branch: `codex/0093-hybrid-pick-texture`
- Base: Plan 0092 verdict commit `1e6c540`
- New mix/density controls compile only into the offline renderer.
- Both Windows Release targets build.
- Default calibration remains byte-identical at
  `C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`.

## Comparison

Directory:
`C:\code\github\guitar-ag\build-vs2022-x64\diagnostics\eg093-hybrid-pick`

1. `1_modal-force-175.wav` — selected modal foundation
2. `2_hybrid-12pct-dense.wav` — 12% direct texture, 2.5x density
3. `3_hybrid-22pct-dense.wav` — 22% direct texture, 2.5x density
4. `4_hybrid-22pct-sparse.wav` — 22% direct texture, original density
5. `5_added-dense-texture-22pct.wav` — isolated `3 - 1` contribution

All files are stereo 48 kHz and 486,000 samples. Track 1 plus track 5
reconstructs track 3 with infinite measured audio PSNR.

## Focused Listening

- `0:00.0–0:01.2` — low E2: choose whether 12% or 22% adds believable pick
  surface without reclaiming the attack from the modal string.
- `0:02.0–0:03.2` — high E4 attack only; register/body correction is deliberately
  unchanged and will be tested separately.
- `0:04.0–0:06.8` — repeated F2: compare track 3 against track 4 to decide
  whether added density removes the sparse woody-rattle impression.
- Track 5: decide whether the isolated direct detail is now acceptable as a
  subordinate texture. Reject it if it still reads as crude digital synthesis.

## Reference Capture

`docs/pick-contact-reference-capture.md` defines a clean-DI muted/ringing matrix
for measuring real contact envelopes, fixed-Hz band decay, event density, and
variation without shipping or replaying samples.

