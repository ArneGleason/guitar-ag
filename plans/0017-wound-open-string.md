# Plan 0017 — Wound Open String

## Goal

Add open-string class behavior for wound strings.

For this calibration pass, the MIDI notes are treated as the six open guitar strings. E2, A2, and D3 are treated as wound strings; G3, B3, and E4 are treated as plain strings. The goal is to improve lower-string realism without changing the higher/plain-string path that already sounds more convincing.

## Scope

- Keep KS010 as the base sound path.
- Detect open wound-string MIDI notes: E2, A2, D3.
- Leave G3, B3, and E4 essentially unchanged.
- Add a small wound-only interaction layer with extra scrape/air and motion-coupled roughness.
- Slightly increase wound-string excitation partial/noise energy.
- Update the visible model label.
- Build and install the VST3 for another calibration render.

## Out of Scope

- Do not add fretting.
- Do not model fret noise or changing fretted string length.
- Do not add MPE.
- Do not add UI controls.
- Do not add pickup switching or amp/cab simulation.

## Implementation Notes

The wound interaction layer is deliberately simple and real-time safe:

- it only activates for MIDI notes 40, 45, and 50
- it adds a short decaying high-passed noise/roughness component
- it is lightly coupled to string motion and pick contact
- it feeds both the string loop and pickup readout

Initial values:

- wound excitation scale: 1.28
- wound interaction envelope: 0.006 to 0.028, scaled by velocity
- wound interaction duration: about 420 to 600 ms
- wound loop injection: 0.22
- wound pickup readout mix: 0.88

## Verification

The human should be able to verify:

- [ ] The plugin UI shows `StringVoice KS-011 WoundOpen`.
- [ ] The plugin still loads in Bitwig.
- [ ] The calibration MIDI renders successfully.
- [ ] Analysis shows whether E2/A2/D3 gain air/upper movement while G3/B3/E4 stay close to KS010.

## Completion Notes

2026-04-25:

- Added wound-open-string detection for E2/A2/D3.
- Added wound-only interaction state and processing.
- Left plain-string notes on the KS010 path.
- Updated model label to `StringVoice KS-011 WoundOpen`.
- Verified the Release VST3 target builds.
