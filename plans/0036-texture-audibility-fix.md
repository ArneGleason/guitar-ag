# Plan 0036: Texture Audibility Fix

## Goal

Make `Pick Texture` unmistakably audible before moving to any new synthesis feature.

## Changes

- Label this pass `StringVoice KS-028 TextureDebug`.
- Use target values for note-start pick controls.
- Move the rendered texture contact outside the attack ramp so it is not faded away with the modal body.
- Add a short metallic ring component to the contact texture so high settings read more like a rough pick or coin.
- Add offline renderer flags for `--sustain`, `--pick-stiffness`, and `--pick-texture` so texture extremes can be rendered without a DAW.

## Verification

- Build the VST3 and offline renderer.
- Render a 0% texture file and a 100% texture file from the same MIDI.
- Install the VST3 and manually test texture extremes in Bitwig.
