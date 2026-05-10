# Studio Handoff: Codex Slide Squeak Control

- Timestamp: 2026-05-10T07:54:28-04:00
- Status: implementation
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review EG-069 Slide Squeak amount control
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

The human auditioned EG-068 and said the motion-coupled slide squeak is not perfectly realistic but is about ten times more realistic than the previous white-noise-like layer. The next need is balance: the slide finger noise needs an independent volume control because the older note approach/release `Finger Noise` layer is still not useful enough to share the same slider.

## Changes Made

- Added `Slide Squeak` as a host-facing Performance-page slider.
- Range is 0% to 200%; default is 100% to preserve EG-068 behavior.
- Added `AudioEngine::setSlideSqueak` and a smoothed `slideSqueak` value.
- Passed the amount to `StringVoice::renderSample` and scaled only the slide contact squeak/scrape output.
- Left `Neck Slide`, `Fret Steps`, `Slide Lift` damping, and old `Finger Noise` approach/release behavior independent.
- Added `GuitarAGOfflineRender --slide-squeak`.
- Updated `CHANGELOG.md`, `DECISIONS.md`, `LEARNINGS.md`, `docs/dsp-notes.md`, `docs/test-plan.md`, `docs/audition-midi.md`, `docs/build-notes.md`, and added `plans/0069-slide-squeak-control.md`.
- Changed the model label to `StringVoice EG-069 SlideSqueakControl`.

## Review Focus

- Confirm parameter/state wiring is complete across APVTS, editor, `AudioEngine`, offline renderer, and `StringVoice`.
- Confirm `Slide Squeak` only scales slide contact noise and does not affect pitch, fret stepping, lift damping, or old `Finger Noise`.
- Confirm the 0% to 200% range and 100% default are safe and sensible.
- Confirm no audio-thread allocation/blocking was introduced.

## Suggested Manual Checks

- Pull latest `main`.
- Build/install the VST3.
- Confirm the UI header shows `v0.2.6 / StringVoice EG-069 SlideSqueakControl`.
- On the Performance page, confirm `Slide Squeak` appears with the slide controls.
- During a moving `Neck Slide`, compare `Slide Squeak` at 0%, 100%, and 200%.
