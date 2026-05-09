# Studio Handoff: Codex Feedback Weight Cache Optimization

- Timestamp: 2026-05-09T14:10:06-04:00
- Status: handoff
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Feedback modal-weight optimization ready for human-routed review
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

Antigravity verified the EG-058 pitch control-rate pass and cleared Codex to target feedback-loop math. Codex first tested a global feedback scalar cache, rejected it because it did not improve local performance, and then implemented the narrower high-value optimization inside the per-voice modal feedback branch.

## What Changed

- Added an 8-sample feedback control interval inside `StringVoice`.
- Cached per-mode feedback harmonic weights and loop-lock weights when feedback is active.
- Moved expensive `round`, `exp`, and `log2` work out of the per-mode/per-sample feedback inner loop.
- Kept per-sample energy gates, feedback rise, release scaling, loop signal scaling, modal decay, and feedback injection.
- Left exact global feedback-loop `std::tanh` saturation unchanged.
- Updated the model label to `StringVoice EG-059 FeedbackWeightCache`.
- Updated `docs/dsp-notes.md`, `plans/0059-cpu-performance-pass.md`, `README.md`, `docs/build-notes.md`, and `CHANGELOG.md`.

## Local Measurements

Baseline was taken from EG-058 in the same local session.

- Player-articulation MIDI, `Amp Feedback` 100%:
  - before: 17.768x realtime
  - after: 29.543x realtime
  - WAV difference: about 0.0015% relative RMS
- Feature-audition MIDI with MPE enabled and `Amp Feedback` 100%:
  - before: 9.315x realtime
  - after: 19.648x realtime
  - WAV difference: about 0.0199% relative RMS
- Player-articulation MIDI, `Amp Feedback` 0%:
  - audio sample data stayed identical against the EG-058 no-feedback render

## Checks Run

- `git diff --check`
- `cmake --build build --config Release --target GuitarAG_VST3`
- `cmake --build build --config Release --target GuitarAGOfflineRender`
- Offline renderer performance reports for player-articulation feedback 100%, feature-audition feedback 100% with MPE, and player-articulation feedback 0%.
- Numeric 24-bit WAV diffs for high-feedback player-articulation and feature-audition renders.
- Numeric sample-data comparison for no-feedback player-articulation render.

## Checks Not Run

- Manual DAW/VST3 listening in Bitwig.
- Cross-machine build on `mac-mini-pro-m4`, since Antigravity reported JUCE is not installed there.
- CPU profiling inside a DAW host.

## Suggested Reviewer Verification

Please pull latest `main` and inspect:

- `src/dsp/StringVoice.cpp`
- `src/dsp/StringVoice.h`
- `plans/0059-cpu-performance-pass.md`
- `docs/dsp-notes.md`
- `CHANGELOG.md`

Please verify:

- The modal feedback harmonic/loop lock calculations are no longer performed every sample.
- The per-sample feedback gates and injection behavior remain intact.
- The no-feedback path remains behavior-neutral.
- The tiny high-feedback render differences are acceptable for the performance gain, or should be routed for human listening before acceptance.
- The exact `std::tanh` feedback saturation is correctly deferred rather than mixed into this pass.

## Open Questions

- Should the 8-sample feedback weight cache be accepted as-is, tuned to 4 samples, or deferred until after DAW listening?
- Should the next optimization target be contact/pick transient math, feedback `tanh` approximation, or a maintainability cleanup around modal rendering?
