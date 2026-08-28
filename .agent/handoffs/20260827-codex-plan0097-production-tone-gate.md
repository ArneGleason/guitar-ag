# Plan 0097 Production Tone Gate Handoff — 2026-08-27

## Branch and Safety Boundary

- Branch: `codex/0097-production-tone-promotion-gate`
- Base: Plan 0096 verdict commit `e9a28ae`
- No VST3 or production DSP changed.
- The candidate uses the complete accepted offline recipe: 1.75x modal force,
  12% texture at 2.5x density, 35% amplitude anchoring, harmonic-number decay,
  and 2x side-mode restoration.

## Comparison

Directory:
`C:\code\github\guitar-ag\build-vs2022-x64\diagnostics\eg097-promotion-gate`

1. `1_current-production-equivalent.wav`
2. `2_accepted-offline-candidate.wav`
3. `3_candidate-minus-current-signed-difference.wav` (`2 - 1`)

All three files are stereo 48 kHz, 1,291,200 samples, and 26.9 seconds. Track 1
plus track 3 reconstructs track 2 with infinite measured audio PSNR. The
candidate is consistently about 1 dB louder than current across all three
sections; use the A/B tool's default loudness matching.

## Timeline and Questions

- `0.00–8.85` ordinary open strings. Notes start at 0.00/1.20/2.40/3.60/4.80/
  6.00 seconds. Does the candidate preserve a normal pick while improving
  wound/plain scale?
- `8.85–17.70` deep/flexible/textured open strings. Notes start at 8.85/10.05/
  11.25/12.45/13.65/14.85 seconds. Does the candidate replace the woody digital
  attack without flattening the high strings?
- `17.70–26.90` compact phrase: riff at 17.70, arpeggio at 19.45, upper melody at
  21.45, and chord at 23.70. Does the candidate remain articulate and guitar-like
  during note changes and polyphony?
- Track 3 is the signed total change. Loudness matching boosts it substantially;
  judge its character, not its apparent mix level.

Promotion requires the candidate to be preferable across all three contexts.

## Human Verdict

With loudness matching on, the human confirmed that track 2 works. This passes
the ordinary, deep-pick, and compact-phrase promotion gate. Implement the exact
accepted recipe in production as a separate milestone and preserve track 1's
recipe as an offline regression option.
