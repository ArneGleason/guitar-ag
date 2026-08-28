# Plan 0105 — Low-E Reference Candidate

## Goal

Test whether the approved low-E reference set can produce an audible realism
gain with one deliberately narrow candidate before asking for more captures.

## Boundary

- Leave the production VST3 model unchanged.
- Match the confirmed neck humbucker with the virtual humbucker at its neck-side
  position.
- Add offline-only controls for fixed physical pluck position and the decay time
  of the long-lived harmonic body modes.
- Do not model the fretting-hand-damped takes as palm mute in this experiment.
- Do not add an independent pick-click layer: Plan 0104's ablations showed that
  the current direct pick/contact layers are negligible relative to the body.
- Choose the candidate from onset-aligned reference measurements, then require a
  loudness-matched human A/B before any production promotion.

## Experiment

1. Render the approved ringing exercises with fixed pluck positions spanning
   0.10–0.22 of speaking length.
2. Select the position that best reduces the reference attack-spectrum residual
   without degrading early-body level.
3. At that position, bracket slower body-mode decay to approach the observed
   -1.0 to -2.1 dB/s reference range.
4. Render a compact current-versus-candidate listening set using identical MIDI,
   pickup, performance controls, and loudness.

## Verification

- [x] Offline renderer accepts and bounds both research controls.
- [x] Default renderer output remains bit-identical with the controls omitted.
- [x] Production VST3 builds without the offline controls.
- [x] Parameter-grid measurements and candidate choice are recorded.
- [x] Current and candidate listening files use identical source MIDI and
      pickup settings and are loudness matched without clipping.
- [ ] The human decides whether the change is perceptible and useful before the
      candidate is promoted or the inventory expands.
