# Plan 0016 — Gentle Damping Retune

## Goal

Retune the KS009 harmonic damping experiment before moving to a new modulation property.

KS009 improved the strongest upper-harmonic shift metric, but it over-damped high harmonics. KS010 keeps the same damping architecture and adjusts it to be gentler and more selective.

## Scope

- Keep the KS008 moving resonance layer.
- Keep the KS009 harmonic damping architecture.
- Raise the high feedback target closer to neutral.
- Slow the damping transition.
- Keep the moving resonance contribution outside the damping path.
- Update the visible model label.
- Build and install the VST3 for another calibration render.

## Out of Scope

- Do not add a new modulation feature.
- Do not add phase-settling all-pass behavior.
- Do not change the exciter, pickup readout, or UI.

## Implementation Notes

The retune changes:

- high feedback target from about `0.980`-`0.984` to about `0.992`-`0.9935`
- transition time from about `420`-`620` ms to about `550`-`800` ms
- split lowpass coefficient from `0.18` to `0.30`
- moving resonance is added after harmonic damping instead of being damped by it

## Verification

The human should be able to verify:

- [ ] The plugin UI shows `StringVoice KS-010 GentleDamp`.
- [ ] The plugin still loads in Bitwig.
- [ ] The calibration MIDI renders successfully.
- [ ] Analysis shows whether h8 decay stays closer to KS008/reference while retaining some strongest-harmonic shift improvement from KS009.

## Completion Notes

2026-04-25:

- Retuned harmonic damping to be gentler and more selective.
- Preserved the KS008 moving resonance layer.
- Updated model label to `StringVoice KS-010 GentleDamp`.
- Verified the Release VST3 target builds.
