# Plan 0040: Friction Drag Texture

## Goal

Make the final 95% to 100% texture range add harsher coin grind without rolling off the useful 95% texture.

## Changes

- Label this pass `StringVoice KS-032 FrictionDrag`.
- Preserve most of the 95% coin layer through the final 5%.
- Reduce the heavy-contact choke from KS-031.
- Replace the top-end heavy layer with a dense ridged-drag friction signal instead of random impulse bursts.

## Verification

- Build the offline renderer and VST3.
- Render the same MIDI at 95% and 100% texture.
- Install the VST3 for manual Bitwig audition.
