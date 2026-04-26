# Plan 0034 — Pick Material Parameters

## Goal

Expose two controls for pick contact character:

- `Pick Stiffness`
- `Pick Texture`

## Scope

- Add both controls as JUCE/APVTS parameters.
- Default both to 50% so the current pick sound is preserved.
- Smooth values in `AudioEngine`.
- Sample values at note start.
- Use stiffness to shape contact width, attack ramp, transient sharpness, and upper-mode excitation.
- Use texture to shape friction/scrape and horizontal excitation.
- Add minimal UI sliders.

## Non-Goals

- No full plectrum/string contact solver yet.
- No MPE changes.
- No broader UI redesign.
- No preset browser.

## Result

Implemented as `StringVoice KS-026 PickMaterial`.
