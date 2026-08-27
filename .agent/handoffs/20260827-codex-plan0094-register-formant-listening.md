# Plan 0094 Register/Formant Listening Handoff — 2026-08-27

## Branch and Safety Boundary

- Branch: `codex/0094-register-formant-anchor`
- Base: Plan 0093 verdict commit `6a358af`
- Register anchoring and compensation compile only into the offline renderer.
- Both Windows Release targets build.
- Default calibration remains byte-identical at
  `C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`.
- Anchor zero reproduces the Plan 0093 medium hybrid exactly.

## Comparison

Directory:
`C:\code\github\guitar-ag\build-vs2022-x64\diagnostics\eg094-register-anchor`

1. `1_current-harmonic-envelope.wav`
2. `2_register-anchor-35.wav`
3. `3_register-anchor-65.wav`
4. `4_register-anchor-100.wav`
5. `5_anchor-65-signed-difference.wav` (`1 - 3`)

All files are stereo 48 kHz and 424,800 samples. Track 3 plus track 5
reconstructs track 1 with infinite measured audio PSNR. Whole-file source levels
are within 0.7 dB after audition-only register compensation.

## Note Times and Questions

- `0.0–0.85` E2 — tone anchor; it is effectively unchanged before A/B matching.
- `1.2–2.05` A2 and `2.4–3.25` D3 — does correction begin naturally?
- `3.6–4.45` G3 and `4.8–5.65` B3 — does instrument scale improve without
  losing a clean metal-string identity?
- `6.0–6.85` E4 — does the high string stop sounding like an uncorrected upward
  pitch shift, or does it merely become dull/hollow?
- Track 5 — does the changed/removed 65% energy contain the small, glassy,
  pitch-shifted identity, or useful guitar-string character that should remain?

