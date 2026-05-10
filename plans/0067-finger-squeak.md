# 0067 — Finger Squeak Revoice

## Context

Human DAW listening after EG-066 found the slide-lift gesture more useful than note-off tails, but the associated finger/string noises still read too much like white or pink hiss. The desired direction is a more guitar-like friction squeak: finger against metal string, with different behavior on wound versus plain strings and less static high-frequency noise.

## Research Notes

- Stick-slip friction produces burst-like acoustic emission, and its amplitude/energy tracks the friction/slip event rather than behaving as a steady noise bed. See Babici, Tudor, and Romeu, “Stick-Slip Phenomena and Acoustic Emission in the Hertzian Linear Contact” (2022): https://doi.org/10.3390/app12199527.
- Guitar strings have measurable friction differences between wound and unwound strings. Groves and Kemp measured lower coefficients for unwound strings than wound strings and noted that wound-string surface unevenness can increase metal-to-metal friction: https://doi.org/10.24425/aoa.2019.129261.
- Squeak can become more tonal when a surface structure regularizes slip pulses; irregular contact stays more broadband. Djellouli et al., “Squeaking at soft-rigid frictional interfaces” (2026): https://www.nature.com/articles/s41586-026-10132-3.

## Plan

1. Revoice global `Finger Noise` around short stick-slip impulses that excite a string/fret-dependent squeak carrier.
2. Keep a smaller filtered scrape/body component so the layer still has contact texture without sounding like static hiss.
3. Make wound strings more ridged and assertive; keep plain strings smoother and less noisy.
4. Apply the same principle to slide-fret scrape so `Slide Lift` can sound like pressure easing against strings rather than a generic noise wash.
5. Keep the existing host-facing `Finger Noise` parameter and `Slide Lift` controls unchanged.

## Implementation

- Added stick-slip pulse state to `AudioEngine::FingerNoiseVoice`.
- Replaced the previous high-passed random-noise-heavy finger layer with filtered friction, short impulse bursts, and a string/fret-dependent squeak carrier.
- Revoiced `StringVoice` slide-fret scrape with filtered friction state and small impulse-driven ridge squeak.
- Updated the `Finger Noise` info note to describe stick-slip squeak/body bursts and wound/plain string behavior.
- Updated the model label to `StringVoice EG-067 FingerSqueak`.

## Verification Targets

- Build the VST3 and install it to the normal user VST3 folder.
- Confirm new plugin instances show `v0.2.6 / StringVoice EG-067 FingerSqueak`.
- With `Finger Noise` at 0%, note playback should remain effectively unchanged.
- With `Finger Noise` raised and `Lookahead` enabled, approach/release noises should sound less like hiss and more like small friction chirps.
- During `Neck Slide` with `Slide Lift`, lifted scrape should be less airy and more string-contact-like, especially on wound-string assignments.
