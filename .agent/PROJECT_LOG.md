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
- Received Antigravity verification that the EG-058 pitch control-rate optimization should be kept and recorded it in `.agent/handoffs/20260509-140125-antigravity-optimization-verification.md`.
- Implemented the next optimization pass as `StringVoice EG-059 FeedbackWeightCache`, caching modal feedback harmonic/loop weights at an 8-sample control interval.
- Received Antigravity verification that the EG-059 feedback weight cache should be kept and recorded it in `.agent/handoffs/20260509-141800-antigravity-feedback-verification.md`.
- Tested two feedback-loop `tanh` approximation candidates, rejected both because they did not produce a meaningful performance win, and recorded the result in `.agent/handoffs/20260509-142119-codex-feedback-tanh-response.md`.
- Received Antigravity verification of the rejected feedback-loop `tanh` experiment and recorded it in `.agent/handoffs/20260509-142600-antigravity-tanh-verification.md`.
- Implemented `StringVoice EG-060 ContactTrigFast`, using a contact-local sine approximation and explicit burr-power multiplication for transient pick/gesture tones.
- Received Antigravity verification that EG-060 should be kept and recorded it in `.agent/handoffs/20260509-143800-antigravity-contact-verification.md`.
- Implemented `StringVoice EG-061 RenderHelpers`, extracting modal, pick-transient, and contact-layer stages from `renderSample` with byte-identical audio sample output.
- Received Antigravity verification that EG-061 should be kept and that the code-level optimization cycle should end; recorded it in `.agent/handoffs/20260509-162200-antigravity-render-helper-verification.md`.
- Routed the next step to human DAW/VST3 listening in `.agent/handoffs/20260509-162331-codex-human-daw-listening.md`.
- Received the first human DAW/VST3 listening result: the installed EG-061 VST sounds the same, shows the expected UI header, and has visible but more modest DAW performance gains than offline render factors suggested.
- Completed a research/planning pass for guitar slide gestures, including slide/glissando terminology, MPE pitch-curve authoring for single notes, a proposed global `Slide Offset` automation lane for chord-shape slides, and implementation phases in `plans/0064-slide-gesture-notation.md`.
- Started Phase 1 of the slide gesture plan by adding a generated MPE slide audition MIDI workflow for slide-to, slide-in, throw, slide-out, independent chord-note slides, and a duplicated-MPE chord-slide proxy.
