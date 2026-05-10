# Studio Handoff: Codex Finger Squeak Revoice

- Timestamp: 2026-05-09T22:06:05-04:00
- Status: implementation
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review EG-067 finger/string squeak revoice
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

Human DAW audition accepted the `Slide Lift` direction but reported that the finger/string noises sounded too much like white or pink hiss. The requested direction was more finger-on-metal-string squeak, with less broadband high-frequency noise and different texture for wound versus plain strings.

## Research Basis

- Stick-slip friction produces burst-like acoustic emission rather than a steady noise bed.
- Guitar string friction differs between wound and unwound strings, and wound-string surface unevenness can increase friction.
- Structured surfaces can turn irregular friction noise into more coherent squeak pulse trains.

## Changes Made

- Added `plans/0067-finger-squeak.md`.
- Changed the model label to `StringVoice EG-067 FingerSqueak`.
- Updated `AudioEngine::FingerNoiseVoice` with filtered scrape, stick-slip impulse, and squeak amount state.
- Revoiced global `Finger Noise` approach/release events away from high-passed random noise and toward string/fret-dependent squeak carriers plus smaller filtered scrape/body texture.
- Revoiced `StringVoice` slide-fret/slide-lift scrape toward filtered friction plus impulse-driven ridged squeak.
- Updated the `Finger Noise` UI info note.
- Updated `CHANGELOG.md`, `DECISIONS.md`, `LEARNINGS.md`, `docs/dsp-notes.md`, `docs/test-plan.md`, and `docs/build-notes.md`.

## Review Focus

- Confirm that `Finger Noise` at 0% remains silent/effectively neutral.
- Confirm that the new AudioEngine finger-noise state is bounded and does not create audio-thread allocation or blocking.
- Confirm that the slide scrape changes are gated to existing slide/fret/lift activity and do not alter ordinary static notes.
- Review whether the wound/plain weighting and impulse clamps look safe for dense chords.

## Suggested Manual Checks

- Pull latest `main`.
- Build/install the VST3.
- In Bitwig/Live, confirm the UI header shows `v0.2.6 / StringVoice EG-067 FingerSqueak`.
- With `Lookahead` enabled and `Finger Noise` raised, audition approach/release events on low wound strings and high plain strings.
- During a moving `Neck Slide`, raise `Slide Lift` and listen for more string-contact squeak and less hiss.
