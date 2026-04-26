# Plan 0037: Stick-Slip Texture

## Goal

Keep the audible and useful 50% pick texture while replacing the excessive high-texture snap with a rougher contact regime.

## Changes

- Label this pass `StringVoice KS-029 StickSlipTexture`.
- Preserve the 0% to 50% smooth pick-contact behavior.
- Above 50%, reduce the smooth snap contribution so it does not become a louder white-noise click.
- Add irregular stick-slip impulses with a short metallic resonance to approximate gritty pick/string drag.

## Verification

- Build the offline renderer and VST3.
- Render the same MIDI at 50% and 100% texture.
- Install the VST3 for manual Bitwig audition.
