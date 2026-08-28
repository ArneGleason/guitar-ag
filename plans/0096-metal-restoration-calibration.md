# Plan 0096 — Metal Restoration Amount Calibration

## Goal

Find the lowest side-mode restoration factor that preserves the positive Plan
0095 combined tone while keeping its accepted harmonic-number decay and 35%
amplitude/formant envelope fixed.

## Listening Basis

- Human loudness-matched listening called the Plan 0095 combined 6x candidate
  "pretty good."
- The 6x factor was selected for audibility after literal 1x restoration measured
  only -60.6 dB mean on E4; it is not a physical calibration.
- The decay-only and metal-only Plan 0095 tracks received no written verdict, so
  include 0x as a real boundary rather than assuming the metal term is necessary.

## Scope

- Keep 1.75x modal force, 12% direct texture, and 2.5x density fixed.
- Keep the 35% amplitude/formant anchor fixed.
- Keep harmonic-number decay fixed.
- Compare the selected 6x reference against 0x, 2x, and 4x restoration.
- Include the isolated metal contribution `6x - 0x`.
- Make no DSP or VST3 changes; this plan calibrates an existing offline control.

## Verification

- [x] The selected 6x render exactly reproduces Plan 0095 track 4.
- [x] Repeated 2x and 4x renders are deterministic.
- [x] Every A/B file is stereo 48 kHz, aligned, and equal length.
- [x] The 0x render plus the isolated 6x contribution reconstructs the selected
      reference exactly.
- [x] Human selected 2x as good. Use it as the calibrated restoration amount;
      do not retain the deliberately exaggerated 6x probe.

## Decision Gate

- If 0x is as good, remove side-mode restoration from the foundation.
- If 2x or 4x is as good as 6x, use the lower factor.
- If only 6x retains the improvement, keep it as the offline foundation but defer
  production promotion until measured guitar references can calibrate the amount.

## Prepared Listening Set — 2026-08-27

All tracks use harmonic-number decay, the accepted 35% amplitude envelope, and
the accepted medium hybrid attack. They are stereo 48 kHz, 424,800 samples, and
8.85 seconds:

1. selected 6x restoration reference
2. decay-only 0x boundary
3. 2x restoration
4. 4x restoration
5. isolated metal contribution `1 - 2`

The four full mixes are within 0.1 dB whole-file mean and within 0.2 dB at each
note, so amount is not confounded with a broad level change. The isolated 6x
contribution measures -47.8 dB mean / -25.6 dB peak before A/B matching. Track 2
plus track 5 reconstructs track 1 with infinite measured audio PSNR.

Track 1 exactly reproduces the accepted Plan 0095 candidate at SHA-256
`D0D4B3E6AFD28CAEAA56CDCF5C27D6CCED6294F7EE7EFEFEDE8C007260F5019F`.
The 2x and 4x hashes repeat exactly at
`ABF261ECD386B652755244D6A63786E6DB4A8899E1948DDF263750EC770AC041`
and `800F077B090ABBE4765ABE4A4F4ECC0206435EB5C482979D30773DE01454DECD`.

## Human Listening Verdict — 2026-08-27

With loudness matching on, 2x restoration was "good." It is the selected amount
for the consolidated offline tone, replacing the 6x audibility probe. The exact
selected render is track 3 at SHA-256
`ABF261ECD386B652755244D6A63786E6DB4A8899E1948DDF263750EC770AC041`.

The accepted offline recipe is now 1.75x modal pick force, 12% direct texture at
2.5x density, 35% amplitude/formant anchoring, harmonic-number decay, and 2x
fixed-Hz side-mode restoration. It still requires a broader end-to-end promotion
gate before any VST3/default change.
