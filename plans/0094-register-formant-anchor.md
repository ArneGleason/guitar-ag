# Plan 0094 — Register/Formant Envelope Anchor

## Goal

Test the human diagnosis that upper notes sound like the low-register model was
naively pitch-shifted without formant correction. Keep string partial frequencies
pitch-relative while anchoring more of the modal amplitude/decay envelope in
absolute frequency.

## Listening Basis

- The low E2 in the focused fixture sounds relatively natural.
- High E4 sounds like a smaller, sped-up version of that voice rather than a
  plausible higher guitar string.
- Current contact filtering, partial tilt, string-age damping, pickup electrical
  tilt, and decay curvature are largely functions of harmonic number. Their
  spectral envelope therefore rises in Hz with the fundamental.

## Scope

- Keep the accepted Plan 0093 medium attack fixed: 1.75x modal force plus 12%
  direct texture at 2.5x density.
- Add one offline-only `0..1` register anchor.
- At zero, preserve the current harmonic-index envelope exactly.
- Above zero, interpolate the envelope index geometrically toward absolute
  frequency referenced to low E2 (82.4069 Hz).
- Apply the anchored index to contact-width filtering, partial tilt, string-age
  damping, pickup electrical tilt, and modal decay curvature.
- Keep modal frequencies, pickup-position nodes/aperture, pluck geometry, and
  harmonic attack emphasis pitch-relative.
- Compare progressive 35%, 65%, and 100% anchors plus the energy removed by the
  65% candidate.

## Non-Goals

- No production/VST3 or preset changes.
- No new resonances, EQ, cabinet, or blanket high-note low-pass.
- No attack-mix or finger-noise changes.
- No claim that one fixed envelope is the final model for all gauges/strings.

## Verification

- [x] Windows Release VST3 and offline renderer build.
- [x] Default calibration remains byte-identical.
- [x] Anchor 0 reproduces the Plan 0093 medium hybrid exactly.
- [x] Anchored renders repeat byte-identically.
- [x] Every A/B file is aligned/equal length and the signed-difference residual
      reconstructs its source exactly.
- [ ] Human decides whether anchoring fixes scale/identity without merely making
      upper notes dull.

## Prepared Listening Set — 2026-08-27

The fixture plays E2, A2, D3, G3, B3, and E4 at 0.0, 1.2, 2.4, 3.6,
4.8, and 6.0 seconds. Each note lasts 0.85 seconds. All five renders are stereo
48 kHz, 424,800 samples, and 8.85 seconds:

1. current harmonic-number envelope
2. 35% absolute-frequency anchor
3. 65% absolute-frequency anchor
4. 100% absolute-frequency anchor
5. signed output difference `1 - 3`

The first literal implementation made E4 up to about 14 dB quieter. A bounded
register-dependent audition compensation now keeps per-note mean levels within
about 1 dB of the baseline while leaving the envelope change intact. Whole-file
source levels are within 0.7 dB. Low E2 is effectively unchanged; its 65% signed
difference measured -91 dB. Track 3 plus track 5 reconstructs track 1 with
infinite measured audio PSNR.

## Decision Gate

- If a partial anchor improves high strings while retaining low-string identity,
  use it as the basis for a string/gauge-aware production envelope.
- If all anchors merely darken or hollow the notes, reject this scalar correction
  and separate string state from pickup/body observation more fundamentally.
