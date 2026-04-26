# Plan 0038: Coin Texture Extension

## Goal

Extend the top of `Pick Texture` with a Brian May style coin-pick region without changing the useful plastic-pick range below it.

## Changes

- Label this pass `StringVoice KS-030 CoinTexture`.
- Remap the KS-029 texture range so its prior maximum is reached at about 80%.
- Add a dedicated 80% to 100% coin-edge layer.
- Model the coin layer as fast irregular contact impulses plus a ridged metallic phase function.

## Verification

- Build offline renderer and VST3.
- Render the same MIDI at 80% and 100% texture.
- Install the VST3 for manual Bitwig audition.
