# Plan 0041: Palm Mute Parameter

## Goal

Add a simple global palm mute model that can be left on for muted playing or automated/modulated to damp strings that are already ringing.

## Changes

- Label this pass `StringVoice KS-033 PalmMute`.
- Add a `Palm Mute` VST parameter from 0% to 100%, default 0%.
- Add the control to the basic plugin UI.
- Smooth the parameter in `AudioEngine` and pass it to every active voice on every sample.
- Apply palm mute as extra modal damping plus a small output reduction at high mute amounts.
- Keep pick/contact output comparatively intact so full mute is mostly pick sound with a quickly damped string resonance.
- Add `--palm-mute` to the offline renderer for quick diagnostic renders.

## Verification

- Build the offline renderer and VST3.
- Render the same MIDI at 0%, 50%, and 100% palm mute.
- Install the VST3 for manual Bitwig audition.
