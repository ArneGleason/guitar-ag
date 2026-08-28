# Plan 0097 — Production Tone Promotion Gate

## Goal

Decide whether the consolidated offline tone recipe is robust enough to replace
the current production-equivalent legacy tone in the VST3/default path.

## Accepted Candidate

- modal pick force: 1.75x
- direct pick texture: 12%
- texture event density: 2.5x
- amplitude/formant anchor: 35%
- modal decay anchor: 0% (harmonic-number decay)
- fixed-Hz side-mode restoration: 2x

## Scope

- Compare the current production-equivalent legacy path first against the exact
  accepted offline candidate.
- Use one stitched, aligned program containing:
  1. ordinary-pick E2-to-E4 open-string sweep
  2. deep/flexible/textured E2-to-E4 open-string sweep
  3. compact picked riff, cross-string arpeggio, upper-register melody, and chord
- Include the signed full-output difference `candidate - current`.
- Keep all non-tone performance settings identical between current and candidate.
- Make no VST3/default DSP changes in this plan.

## Verification

- [x] Generate and inspect the compact promotion phrase MIDI.
- [x] Current and candidate section renders are deterministic.
- [x] Stitched A/B files are stereo 48 kHz, aligned, and equal length.
- [x] The current track plus signed difference reconstructs the candidate exactly.
- [x] Measure whole-file and per-section levels so loudness matching is
      interpretable.
- [ ] Human decides whether the candidate wins across ordinary, deep, and phrase
      contexts—not merely one isolated-note regime.

## Decision Gate

- If the candidate is preferable across all sections, implement the accepted
  topology/values in the production voice as a separate buildable milestone.
- If it helps only the deep-pick failure case, retain the mechanism but design a
  gesture-dependent mapping instead of replacing production defaults globally.
- If the phrase or ordinary-pick sections regress, keep the controls offline and
  identify the failing component before promotion.

## Prepared Listening Set — 2026-08-27

The three files are stereo 48 kHz, 1,291,200 samples, and 26.9 seconds:

1. current production-equivalent legacy path
2. consolidated accepted offline candidate
3. signed full-output difference `2 - 1`

The program is stitched from deterministic section renders:

- `0.00–8.85` — ordinary-pick E2/A2/D3/G3/B3/E4 sweep
- `8.85–17.70` — deep/flexible/textured version of the same sweep
- `17.70–26.90` — compact riff/arpeggio/melody/chord phrase

Candidate-minus-current mean level is consistently +1.0/+0.9/+1.1 dB across
ordinary/deep/phrase sections, so the A/B tool's whole-file loudness matching
does not tilt one section differently from another. The signed difference is
-40.3 dB mean / -17.1 dB peak before matching. Track 1 plus track 3 reconstructs
track 2 with infinite measured audio PSNR.

SHA-256:

- current: `18D35D92304F452B95AF6BE64B3EBFBED2E9C50465B20837B0D887230C6A2DA9`
- candidate: `77DC804FC3F5EC57F0180CBECCC900A36476D2E53B3E0EB07E61C5F0CB40D750`
- signed difference: `85B85B71B21A783BE2C8BBB7CFBDF4A16614A265A996F449DD366E478BA7CE3D`
