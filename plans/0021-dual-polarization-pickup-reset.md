# Plan 0021 — Dual Polarization Pickup Reset

## Goal

Try a bold open-string reset: replace the current single-motion Karplus-Strong core plus additive wound layers with a small two-polarization string and more physical pickup readout.

The goal is not to implement the full guitar model. The goal is to find out whether a more guitar-like physical basis immediately sounds more believable than continuing to tune noise/ring layers on top of the current KS loop.

## Research Motivation

Recent research review points toward these mechanisms as central for electric guitar tone:

- two string polarizations with different damping/dispersion
- pick excitation as a contact/release event, not just an initial noise burst
- pickup position and finite pickup width/aperture
- electrical pickup/circuit resonance
- incidental contact noise only as a small component

Relevant references:

- Parametric Electric Guitar Synthesis: https://research.aalto.fi/en/publications/parametric-electric-guitar-synthesis
- Acoustics and Modeling of Pickups: https://www.researchgate.net/publication/234034228_Acoustics_and_Modeling_of_Pickups
- Player-Instrument Interaction Models for Digital Waveguide Synthesis of Guitar Touch and Collisions: https://liu.diva-portal.org/smash/record.jsf?pid=diva2:316228
- String-fret interaction with two polarization modes: https://www.dafx.de/paper-archive/details/V4Cf7-bGBHWqxAXcEo5eEQ

## Scope

- Add a second delay path per voice for an orthogonal string polarization.
- Excite the two paths differently at note-on.
- Give each polarization slightly different damping and high-frequency loss.
- Add weak cross-coupling between the two paths.
- Read pickup output from velocity at a pickup position, with a small finite-width/aperture approximation.
- Keep output DI-style and mono/stereo duplicated as today.
- Keep the calibration MIDI as six open strings.

## Out of Scope

- Do not add fretting.
- Do not add MPE.
- Do not add UI controls.
- Do not add pickup switching.
- Do not add amp/cab simulation.
- Do not model full magnetic pickup nonlinearity yet.

## Implementation Notes

This should probably become `StringVoice KS-015 DualPickup`.

Small first version:

- keep one fixed-size secondary delay line inside `StringVoice`
- use the existing pitch delay length for both polarizations
- make the second polarization slightly shorter or read with a small offset only if stable
- excite vertical and horizontal paths with different pluck shapes and a tiny phase offset
- replace the wound-motion layer with polarization/coupling behavior
- use pickup velocity from two nearby read positions to approximate finite aperture
- add a simple resonant lowpass/highpass pickup circuit after per-voice readout or in `ElectricGuitarTone`

## Verification

The human should be able to verify:

- [ ] The plugin UI shows the new model label.
- [ ] The plugin still loads in Bitwig.
- [ ] The calibration MIDI renders successfully.
- [ ] Open strings sound less like layered plucked synthesis and less dependent on hiss/noise.
- [ ] Lower strings have a more metallic, physical ring even if spectral metrics are not immediately better.

