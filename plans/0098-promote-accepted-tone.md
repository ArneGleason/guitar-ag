# Plan 0098 — Promote Accepted Tone

## Goal

Move the complete human-approved Plan 0097 tone recipe from offline ablation
controls into the production `StringVoice`, while preserving an exact offline
path back to the former EG-088 tone.

## Production Recipe

- modal pick force: 1.75x
- direct pick texture: 12%
- texture event density: 2.5x
- amplitude/formant anchor: 35%
- modal decay anchor: 0% (harmonic-number decay)
- fixed-Hz/plain-string-weighted side-mode restoration: 2x

## Scope and Safety Boundary

- Compile the accepted excitation, register envelope, decay split, side-mode
  restoration, and register compensation into both VST3 and offline builds.
- Make the accepted values fixed internal defaults; add no VST parameters or
  preset fields.
- Keep the existing individual offline ablation controls.
- Add `--legacy-tone-recipe previous` as the exact EG-088 regression path.
- Update the visible model identity to `StringVoice EG-089 ModalFormantTone`.

## Verification

- [x] Windows Release `GuitarAG_VST3` and `GuitarAGOfflineRender` build.
- [x] New no-flags default exactly matches the accepted Plan 0097 ordinary,
      deep-pick, and phrase section renders.
- [x] `--legacy-tone-recipe previous` exactly matches the former-production
      ordinary, deep-pick, and phrase section renders.
- [x] The new single-note calibration render repeats byte-identically at SHA-256
      `32AF0A4AE9A04B3E77F74161E963D4AE3E8431433012D642EA13A96C2C5C47D7`.
- [x] The previous recipe reproduces the canonical EG-088 single-note SHA-256
      `C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`.
- [ ] Install the built VST3 and verify the EG-089 identity and tone in the DAW.

## Human Gate

Plan 0097 already supplied the comparative listening gate: the human confirmed
that track 2 works across ordinary picking, deep/flexible/textured picking, and
the compact phrase. This implementation milestone requires only a DAW sanity
check that the installed plugin presents that exact promoted behavior.
