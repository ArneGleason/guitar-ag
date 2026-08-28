# Plan 0095 — Upper-Register Metal Ring Retention

## Goal

Keep the Plan 0094 35% register/formant improvement while identifying why it
progressively loses metal ring and brightness on the upper strings.

## Listening Basis

- Human loudness-matched listening found the 35% anchor much better than the
  current harmonic-number envelope.
- The same candidate flattened as pitch rose and lost clean metal-string ring.
- Plan 0094 anchors both amplitude shaping and modal decay with one control, so
  the verdict cannot distinguish a duller spectrum from shorter bright-mode
  persistence.

## Scope

- Fix the accepted attack at 1.75x modal force, 12% direct texture, and 2.5x
  texture density.
- Fix the amplitude/formant envelope at the accepted 35% anchor.
- Add an offline-only independent register decay anchor. By default it follows
  the envelope anchor exactly so Plan 0094 renders remain reproducible.
- Add an offline-only metal-restoration factor that restores only the energy
  lost from inharmonic side modes, with a smooth fixed-Hz gate and reduced
  effect on wound strings. `1x` is literal restoration; higher factors are an
  explicit diagnostic exaggeration so the contribution is not inaudible.
- Compare anchored versus harmonic-number decay, with and without the labelled
  side-mode diagnostic, as a 2x2 listening set.
- Include the signed contribution added by the combined candidate.

## Non-Goals

- No VST3, preset, or default-render changes.
- No broadband EQ, main-harmonic brightness restoration, or chirp-mode changes.
- No production claim for the audition-only register level compensation.
- No finger-noise changes.

## Verification

- [x] Windows Release VST3 and offline renderer build.
- [x] Default calibration remains byte-identical.
- [x] Default decay-follow behavior reproduces Plan 0094's 35% render exactly.
- [x] Candidate renders repeat byte-identically.
- [x] Every A/B file is aligned/equal length and the signed contribution
      reconstructs the combined candidate exactly.
- [x] Human selected the combined harmonic-decay plus 6x side-mode candidate as
      "pretty good." No separate verdict was recorded for the two single-axis
      candidates, so their individual necessity remains unproven.

## Decision Gate

- If harmonic-number decay restores ring without reviving the wrong scale, keep
  amplitude and decay coordinates separate for a later string/gauge model.
- If side-mode restoration adds believable metal identity, retain it as a
  physically motivated observation detail and calibrate down from the audible
  diagnostic factor.
- If neither helps, inspect pickup/material resonances or per-string modal
  density instead of broadly undoing the accepted 35% envelope.

## Prepared Listening Set — 2026-08-27

All tracks use the six-open-string Plan 0094 fixture and the accepted attack.
They are stereo 48 kHz, 424,800 samples, and 8.85 seconds:

1. accepted 35% envelope and 35% decay anchor
2. accepted 35% envelope with harmonic-number decay
3. accepted 35% envelope/decay plus 6x lost-side-mode restoration
4. harmonic-number decay plus the same 6x side-mode restoration
5. signed added contribution `4 - 1`

Literal 1x lost-side-mode restoration measured -60.6 dB mean on E4 and was too
small for a useful listening slot. The 6x diagnostic measures roughly -45 dB
there while still affecting only the detuned side modes. It is deliberately an
audibility probe, not a proposed production value.

Whole-file source means are -29.6, -28.7, -29.5, -28.6, and -41.6 dB. The A/B
tool's loudness matching controls the modest decay-related level shift. The
combined candidate repeats at SHA-256
`D0D4B3E6AFD28CAEAA56CDCF5C27D6CCED6294F7EE7EFEFEDE8C007260F5019F`.
Track 1 exactly reproduces Plan 0094's accepted render at
`94BA731900865E612887B9FE3CE6FB533323C14302D878418FD518F9C5CEBEF7`,
and track 1 plus track 5 reconstructs track 4 with infinite measured audio PSNR.

## Human Listening Verdict — 2026-08-27

With loudness matching on, the combined track 4 was "pretty good." This passes
the combined direction as the next offline tone foundation. Because 6x was an
audibility probe rather than a physical calibration, hold harmonic-number decay
fixed and compare 0x/2x/4x/6x restoration next. Choose the lowest factor that
preserves the improvement. Do not infer from the single combined verdict that
decay-only or metal-only was independently rejected.
