# Plan 0091 Human Listening Verdict — 2026-08-27

## Branch and Guard

- Branch: `codex/0091-legacy-layer-ablation`
- Diagnostic implementation commit: `a0c09ea`
- VST3 excludes offline ablation code.
- Default legacy calibration remains byte-identical at
  `C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`.

## Attack Verdict

Saved notes:
`C:\code\github\ag-music-tool-workbench\output\ab_notes\guitar-ag-plan0091-attack.json`

- Removing short chirp modes was barely different at the ordinary fixture
  settings. The objectionable chirp occurs with deep pick, flexible stiffness,
  and added texture, so reproduce that regime before tuning.
- Removing explicit transient/contact overlays removed recognizable pick sound.
- The broader modal-body-only render sounded the same as the no-overlay render.
- At 1.924–3.157 seconds, high E sounded like a semi-realistic low-register
  guitar digitally pitch-shifted upward rather than a native high string.

The first residual track was invalid because FFmpeg `amix` summed rather than
subtracted the inputs. Tracks 1–4 were valid. Correct residuals now use explicit
channel subtraction and reconstruct the originals with infinite measured PSNR.

Targeted deep-pick notes:
`C:\code\github\ag-music-tool-workbench\output\ab_notes\guitar-ag-plan0091-deep-pick.json`

- Settings: 100% `Pick Bite`, 10% `Pick Stiffness`, 75% `Pick Texture`.
- The current attack sounded like a pronounced but sparse woody rattle.
- Removing the short chirp modes was again not much different.
- The isolated explicit attack extras sounded like crude digital synthesis, not
  a plausible material interaction.
- Replace the direct-output extras with finite-duration excitation of the modal
  string; do not retune the chirp-mode bank as the primary fix.

## Finger-Noise Verdict

Saved notes:
`C:\code\github\ag-music-tool-workbench\output\ab_notes\guitar-ag-plan0091-finger-noise.json`

The isolated current noise sounded like "plucking the teeth on a stiff plastic
hair comb" and not like fingers moving along guitar strings.

Accepted target description:

- one part less-periodic transverse/vertical bowing on the string
- one part finger-speed-driven friction hiss
- increasing finger depth can engage a restrained pitched squeak
- pitched squeak should depend on string/harmonic position rather than a fixed
  free-running ridge carrier

## Next Step

1. Replace the rejected additive deep-pick extras with modal-coupled excitation.
2. Repair pick entry without low-passing the bright string.
3. Replace global finger noise with motion-caused friction; do not retune the
   existing plastic-comb oscillator.
4. Address high-register modal identity before persistent modal repicks.
