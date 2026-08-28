# Plan 0095 Metal-Ring Listening Handoff — 2026-08-27

## Branch and Safety Boundary

- Branch: `codex/0095-metal-ring-retention`
- Base: Plan 0094 verdict commit `40dd831`
- The two controls compile only into the offline renderer.
- Both Windows Release targets build.
- Default calibration remains byte-identical at
  `C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`.
- Track 1 exactly reproduces the accepted Plan 0094 35% render at
  `94BA731900865E612887B9FE3CE6FB533323C14302D878418FD518F9C5CEBEF7`.

## Comparison

Directory:
`C:\code\github\guitar-ag\build-vs2022-x64\diagnostics\eg095-metal-retention-ab`

1. `1_anchor35-accepted.wav`
2. `2_anchor35-harmonic-decay.wav`
3. `3_anchor35-metal-restore-6x.wav`
4. `4_anchor35-harmonic-decay-metal-restore-6x.wav`
5. `5_combined-added-contribution.wav` (`4 - 1`)

All files are stereo 48 kHz and 424,800 samples. Track 1 plus track 5
reconstructs track 4 with infinite measured audio PSNR. The combined render
repeats byte-identically. Source means are -29.6/-28.7/-29.5/-28.6/-41.6 dB;
use the A/B tool's default loudness matching.

## What Changed

- Track 2 restores harmonic-number decay while leaving the accepted 35%
  amplitude/formant envelope untouched.
- Track 3 retains Plan 0094 decay but restores only the lost detuned side-mode
  energy above a smooth 650–2500 Hz gate, weighted toward plain strings.
- Track 4 combines those independent changes.
- `1x` literal side-mode restoration was -60.6 dB mean on E4. `6x` is a
  deliberately audible diagnostic, not a proposed production value.

## Note Times and Questions

- `0.0–0.85` E2 — control: all full mixes should be effectively identical.
- `1.2–2.05` A2 and `2.4–3.25` D3 — does either change begin too early or alter
  the accepted lower-register identity?
- `3.6–4.45` G3 and `4.8–5.65` B3 — first key decision: does track 2 retain
  natural metal sustain, and does track 3 add ring or synthetic chime/buzz?
- `6.0–6.85` E4 — main decision: which full mix keeps the improved scale while
  restoring brightness and a clean metal-string identity?
- Track 5 — the total energy added by track 4. Does it sound like useful string
  persistence/ring, or like a synthesized layer that should be rejected?
