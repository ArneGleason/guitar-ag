# Plan 0039: Heavy Coin Grind

## Goal

Keep the usable KS-030 coin texture through about 95%, then make the final 5% darker and grindier instead of chirpy and bursty.

## Changes

- Label this pass `StringVoice KS-031 HeavyCoinGrind`.
- Preserve the 80% to 95% coin-pick behavior.
- Fade down the fast coin impulse layer from 95% to 100%.
- Add a heavy-contact grind layer with slower ridged friction and smoothed pressure motion.
- Slightly choke the free modal output in the extreme zone so the coin feels like it remains in contact with the string.

## Verification

- Build the offline renderer and VST3.
- Render the same MIDI at 95% and 100% texture.
- Install the VST3 for manual Bitwig audition.
