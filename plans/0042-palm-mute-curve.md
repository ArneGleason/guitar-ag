# Plan 0042: Palm Mute Curve

## Goal

Retune the `Palm Mute` parameter response without changing the underlying mute sound.

## Change

- Label this pass `StringVoice KS-034 PalmMuteCurve`.
- Remap the visible 0% to 50% slider range onto the previous 0% to 25% mute range.
- Remap the visible 50% to 100% slider range onto the previous 25% to 100% mute range.
- Keep the existing modal damping and pick/contact balance intact.

## Verification

- Build the offline renderer and VST3.
- Render diagnostic palm mute values at 0%, 50%, and 100%.
- Install the VST3 for manual Bitwig audition.
