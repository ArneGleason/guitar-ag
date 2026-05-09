# Project Log

Use this file for durable Studio Pattern project events and meaningful workflow decisions. Keep entries short and chronological.

## 2026-05-09

- Adopted the Studio Pattern from `the-studio-pattern`.
- Source version/date/commit: 2026-05-09 initial canonical scaffold, `5624928215232b7b82702929a7b9f8f11f19fd5d`.
- Local adaptation: preserve Guitar AG's existing `AGENTS.md` engineering rules and add Studio Pattern memory surfaces for lead/reviewer cycles, human-routed handoffs, and multi-machine state.
- Initial review focus: see `.agent/REVIEW_QUEUE.md`.
- Assigned Studio Pattern machine handles: Codex lead on `macbook-pro-m5`, Antigravity reviewer on `mac-mini-pro-m4`.
- Created first reviewer bootstrap handoff with GitHub clone/pull instructions before code review.
- Received baseline static review findings from Antigravity on `mac-mini-pro-m4` and recorded them in `.agent/handoffs/20260509-133525-antigravity-review-findings.md`.
- Implemented the immediate review fixes in `AudioEngine`: lookahead now schedules continuous expression events with delayed notes, MPE mode changes clear per-channel expression state, and normal MIDI channel pressure/CC74 apply globally.
- Deferred the per-sample `std::pow`/`std::sin`/`std::tanh` performance finding to the upcoming optimization pass.
- Added `CHANGELOG.md` and updated `AGENTS.md` so future code, behavior, workflow, and notable documentation changes are recorded there.
- Received Antigravity verification that the immediate baseline review fixes were correct and recorded it in `.agent/handoffs/20260509-134600-antigravity-verification.md`.
- Implemented the first optimization pass as `StringVoice EG-058 PitchControlRate`, caching pitch ratios and adjusted modal phase steps at a 4-sample control interval.
