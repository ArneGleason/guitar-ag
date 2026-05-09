# Studio Handoff: Codex Feedback Tanh Experiment Response

- Timestamp: 2026-05-09T14:21:19-04:00
- Status: handoff
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Feedback tanh approximation tested and rejected
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

Antigravity verified `StringVoice EG-059 FeedbackWeightCache` and accepted the 8-sample feedback weight cache as-is. Codex then evaluated the suggested final feedback-loop `std::tanh` approximation target before moving on to contact/pick transient math.

No production code change is being kept from this tanh experiment.

## What Was Tested

- A high-accuracy rational `tanh` approximation for the three global feedback-loop saturation calls in `AudioEngine::updateAmpFeedbackLoop`.
- A cheaper rational `tanh` approximation for the same three calls.
- Both candidates were limited to the global amp-feedback loop and did not touch `StringVoice::softClip` or the contact transient branch.

## Local Measurements

Current EG-059 baseline, before the tanh candidates:

- Player-articulation MIDI, `Amp Feedback` 100%: 30.445x realtime.
- Feature-audition MIDI with MPE enabled and `Amp Feedback` 100%: 19.865x realtime.
- Player-articulation MIDI, `Amp Feedback` 0%: 43.446x realtime.
- Player-articulation MIDI with heavy pick/contact settings and feedback off: 36.268x realtime.

High-accuracy rational candidate:

- Player-articulation MIDI, `Amp Feedback` 100%: 30.172x realtime.
- Feature-audition MIDI with MPE enabled and `Amp Feedback` 100%: 19.906x realtime.

Cheaper rational candidate:

- Player-articulation MIDI, `Amp Feedback` 100%: 30.139x realtime.
- Voice-sample count changed slightly, so it was not behavior-neutral.

## Conclusion

The `std::tanh` replacement is not worth keeping right now. It did not produce a meaningful performance win, and the cheaper candidate changed behavior enough to be a poor tradeoff.

Recommendation:

- Keep the exact global feedback-loop `std::tanh` for now.
- Prefer the next optimization cycle to target either contact/pick transient math, or a maintainability pass around the modal/contact render structure.
- If `tanh` is revisited later, use a real profiler first rather than swapping approximations speculatively.

## Checks Run

- `git diff --check`
- `cmake --build build --config Release --target GuitarAGOfflineRender`
- Offline renderer performance reports for the EG-059 baseline, high-accuracy tanh candidate, and cheap tanh candidate.

## Checks Not Run

- Manual DAW/VST3 listening.
- CPU profiling inside a DAW host.
- Cross-machine build on `mac-mini-pro-m4`.
