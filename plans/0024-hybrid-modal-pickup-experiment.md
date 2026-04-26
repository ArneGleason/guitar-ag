# Plan 0024 — Hybrid Modal Pickup Experiment

## Goal

Run a bold experiment loop to escape the local search around additive wound-string noise/ring layers.

The hypothesis is that realistic electric-guitar DI needs a coupled string-plus-pickup system: two-polarization string motion, sparse inharmonic metal side modes, pickup aperture/position filtering, and pickup/cable resonance. A single plucked delay loop or a clean modal bank may each miss important emergent behavior.

## Scope

- Keep KS015's dual-polarization delay-loop core.
- Try an explicit modal string bank offline as a falsification candidate.
- If pure modal is too sterile, keep modal behavior only as a low-level residual.
- Add a simple pickup/cable-style resonant DI stage in `ElectricGuitarTone`.
- Use the offline renderer and perceptual analyzer before asking for Bitwig audition.

## Out of Scope

- No fretting model.
- No MPE routing.
- No amp/cab simulation.
- No UI controls.
- No sample playback.

## Results

2026-04-25:

- Pure modal replacement rendered and analyzed, but moved farther from the DI references: mean full log-spectral distance about 32.20 dB.
- Hybrid modal residual without pickup-circuit change was close to KS015 and not compelling enough by itself: about 30.44 dB.
- Hybrid modal residual plus pickup/cable resonance improved over KS015 in the current analyzer: about 29.34 dB versus KS015 at about 30.16 dB.
- The candidate label is `StringVoice KS-016 HybridModal Pickup-002`.

## Verification

- [x] Offline renderer builds.
- [x] Offline calibration render produced.
- [x] Perceptual, calibration, and harmonic-evolution analyses ran.
- [x] VST3 installed for Bitwig audition.
- [ ] Human listening check: does pickup resonance add useful twang/electricity, or does it merely EQ the same synthetic source?
