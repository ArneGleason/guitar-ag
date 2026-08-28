# Plan 0096 Metal Calibration Listening Handoff — 2026-08-27

## Branch and Safety Boundary

- Branch: `codex/0096-metal-restoration-calibration`
- Base: Plan 0095 verdict commit `e36b35a`
- No DSP, VST3, preset, or renderer code changed in this plan.
- Track 1 exactly reproduces the human-selected Plan 0095 candidate at
  `D0D4B3E6AFD28CAEAA56CDCF5C27D6CCED6294F7EE7EFEFEDE8C007260F5019F`.

## Comparison

Directory:
`C:\code\github\guitar-ag\build-vs2022-x64\diagnostics\eg096-metal-calibration`

1. `1_selected-metal-restore-6x.wav`
2. `2_decay-only-0x.wav`
3. `3_metal-restore-2x.wav`
4. `4_metal-restore-4x.wav`
5. `5_added-metal-at-6x.wav` (`1 - 2`)

All files are stereo 48 kHz and 424,800 samples. The full mixes are within
0.1 dB whole-file mean and 0.2 dB at every note. Track 2 plus track 5
reconstructs track 1 with infinite measured audio PSNR. The isolated contribution
is -47.8 dB mean / -25.6 dB peak before loudness matching.

## Note Times and Questions

- `0.0–0.85` E2 — control: all full mixes are effectively identical.
- `1.2–2.05` A2 and `2.4–3.25` D3 — make sure restoration does not disturb the
  accepted lower-register identity.
- `3.6–4.45` G3 and `4.8–5.65` B3 — does 2x or 4x retain the useful ring already,
  or does only 6x avoid flattening?
- `6.0–6.85` E4 — primary amount decision. Select the lowest of 0x/2x/4x/6x
  that still has the "pretty good" metal brightness and scale.
- Track 5 — judge character only. Loudness matching greatly boosts this quiet
  contribution; do not interpret its playback level as its actual mix level.
