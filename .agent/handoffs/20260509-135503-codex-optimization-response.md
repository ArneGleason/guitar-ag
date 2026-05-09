# Studio Handoff: Codex Pitch Control-Rate Optimization

- Timestamp: 2026-05-09T13:55:03-04:00
- Status: handoff
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: First performance optimization pass ready for human-routed review
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

Antigravity verified the immediate baseline review fixes and cleared Codex to proceed into the performance/maintainability optimization pass. Codex implemented the first narrow optimization against the deferred per-sample pitch-modulation finding.

## What Changed

- Added a 4-sample pitch control interval in `StringVoice`.
- Kept aftertouch and MPE pitch smoothing per-sample.
- Recomputed vibrato, whammy, aftertouch-bend, and MPE pitch ratios every 4 samples instead of every sample.
- Cached adjusted modal phase-step sine/cosine values every 4 samples when pitch modulation is active.
- Left neutral-pitch rendering on the existing precomputed modal phase-step path.
- Updated the model label to `StringVoice EG-058 PitchControlRate`.
- Updated `docs/dsp-notes.md`, `plans/0059-cpu-performance-pass.md`, `README.md`, `docs/build-notes.md`, and `CHANGELOG.md`.

## Local Measurements

Baseline was taken from `main` before this optimization in the same local session.

- Player-articulation MIDI, feedback 0%:
  - before: 37.636x realtime
  - after: 43.545x realtime
  - WAV comparison: byte-identical
- Player-articulation MIDI, feedback 100%:
  - before: 17.726x realtime
  - after: 17.702x realtime
  - WAV comparison: byte-identical
- Feature-audition MIDI with MPE enabled:
  - before: 31.692x realtime
  - after: 37.858x realtime
  - WAV comparison: differs as expected because pitch modulation is now control-rate
  - measured difference: about 0.74% relative RMS against the previous per-sample render

## Checks Run

- `git diff --check`
- `cmake --build build --config Release --target GuitarAG_VST3`
- `cmake --build build --config Release --target GuitarAGOfflineRender`
- Offline renderer performance reports for player-articulation feedback 0%, player-articulation feedback 100%, and feature-audition MPE.
- Byte comparison for neutral player-articulation renders at feedback 0% and 100%.
- Numeric 24-bit WAV diff for the MPE feature render.

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

- The expensive pitch ratio and modal phase-step recalculations are no longer performed every sample.
- Neutral pitch/no-modulation renders should remain byte-identical.
- The 4-sample control interval is a reasonable tradeoff for MPE bends, whammy, vibrato, and aftertouch bend.
- The remaining feedback-loop `tanh` and scalar math is correctly left as follow-up rather than mixed into this change.

## Open Questions

- Should the 4-sample pitch control interval be accepted, or should it be reverted to per-sample for maximum MPE precision?
- Should the next optimization pass target feedback-loop math, contact/pick transient math, or maintainability cleanup around modal rendering?
