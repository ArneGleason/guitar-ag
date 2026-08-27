# Plan 0092 — Modal-Coupled Pick Excitation

## Goal

Test whether the accepted legacy modal body gains a more plausible pick entry
when the plectrum changes modal string state instead of producing independent
audio through `renderPickTransient()` and the picked portion of
`renderContactLayer()`.

## Listening Basis

Plan 0091 established that:

- short chirp modes are a minor contribution at ordinary and deep-pick settings
- the explicit deep/flexible/textured attack extras sound like a sparse woody
  rattle and crude digital synthesis
- removing those extras also removes recognizable pick identity

The next experiment therefore changes the path by which pick energy enters the
sound. It is not another retune of the rejected overlay.

## Scope

- Keep the production VST3 and default offline render byte-identical.
- Add one offline-only `modal` pick-excitation selector alongside the current
  `additive` path.
- Add an offline-only force scale so the first listening pass can distinguish a
  flawed excitation shape from an otherwise useful contribution that is merely
  too quiet.
- In modal mode, suppress the direct picked transient/contact output while
  retaining non-pick gesture and slide contact behavior.
- Apply a short deterministic plectrum-force envelope to each active modal
  oscillator's quadrature state. Couple the force by the already configured
  mode amplitude so excitation inherits pluck position, pickup response,
  aperture, string construction, and decay.
- Let stiffness control contact duration, bite control force, and texture add
  bounded non-periodic force roughness rather than an audible ridge oscillator.
- Prepare a deep-pick A/B containing current, body-only, two modal-coupled force
  levels, and the isolated stronger modal contribution.

## Non-Goals

- No production parameter or preset-state change.
- No high-register modal revoice in this pass.
- No finger-noise redesign in this pass.
- No persistent modal state or repick carryover.

## Verification

- [x] Windows Release VST3 and offline renderer build.
- [x] Default legacy calibration remains byte-identical.
- [x] Modal-coupled renders repeat byte-identically.
- [x] A/B files are aligned, equal length, distinctly named, and reconstruction-
      checked where subtraction is used.
- [x] Human listening decides whether modal coupling improves material/string
      identity at deep-pick settings.

## Prepared Listening Set — 2026-08-27

All five files are stereo 48 kHz, 486,000 samples, and 10.125 seconds:

1. current additive deep-pick reference
2. modal body with the direct picked extras removed
3. modal-coupled pick at 1.00x force
4. modal-coupled pick at 1.75x force
5. isolated 1.75x modal contribution (`4 - 2`)

The isolated contribution reconstructs track 4 when added to track 2 with
infinite measured audio PSNR. The initial 1.00x implementation contribution was
about -50.0 dB mean / -32.1 dB peak; 1.75x reaches -45.2 dB mean / -27.2 dB
peak, so the listening set can separate shape/character from insufficient level.

## Listening Verdict — 2026-08-27

The 1.75x modal-force render is the promising foundation. It does not yet carry
enough pick-surface texture by itself. The requested next candidate is the 1.75x
modal render plus a much smaller amount of the current additive attack, with the
additive texture events made denser so they read as continuous material contact
rather than a sparse woody rattle.

This is a qualified positive result for coupling attack energy into the modal
state, not acceptance of the current candidate for production. Keep the direct
layer subordinate and verify its isolated hybrid contribution.

## Decision Gate

- If the isolated modal contribution sounds like string vibration and the full
  attack improves, refine this coupled model in a later production-safe pass.
- If it remains an oscillator-like artifact, stop tuning attack envelopes and
  revisit the modal state/output representation before production changes.
