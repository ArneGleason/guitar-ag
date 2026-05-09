# Changelog

All notable project changes should be recorded here.

This changelog starts on 2026-05-09. Earlier project history is preserved across `README.md`, `DECISIONS.md`, `LEARNINGS.md`, `docs/`, and `plans/`.

## Unreleased

### Added

- Adopted the Studio Pattern memory surfaces for lead/reviewer handoffs:
  - `LOCAL_DEV_NOTES.md`
  - `.agent/PROJECT_LOG.md`
  - `.agent/REVIEW_QUEUE.md`
  - `.agent/session.json`
  - `.agent/handoffs/`
  - `.agent/workflows/`
- Added human-routed handoff notes for the first Antigravity baseline code review and Codex review-fix response.

### Changed

- Updated project agent instructions to include Studio Pattern first reads and handoff rules.
- Updated MPE and test documentation to describe normal MIDI pressure/CC74 fallback and lookahead expression sync checks.
- Changed the model label to `StringVoice EG-058 PitchControlRate`.
- Moved pitch-modulation ratio and adjusted modal phase-step recalculation to a 4-sample control interval while keeping expression smoothing per-sample.
- Changed the model label to `StringVoice EG-059 FeedbackWeightCache`.
- Cached high-feedback per-mode harmonic/loop weights at an 8-sample control interval while preserving per-sample feedback gates and injection.
- Documented Antigravity acceptance of the EG-059 feedback weight cache and Codex's rejected feedback-loop `tanh` approximation experiment.
- Changed the model label to `StringVoice EG-060 ContactTrigFast`.
- Replaced contact/gesture transient sine calls with a contact-local fast sine approximation and replaced the contact burr `pow(abs(sin), 7)` with explicit multiplication.
- Changed the model label to `StringVoice EG-061 RenderHelpers`.
- Extracted `StringVoice::renderSample` modal-bank, pick-transient, and contact-layer stages into helper functions without changing rendered audio sample data.

### Fixed

- Fixed lookahead timing so pitch wheel, mod wheel/CC1, channel pressure, CC74, and key/poly aftertouch are scheduled with delayed note events instead of applying immediately.
- Fixed MPE mode changes so per-channel pitch bend, pressure, and CC74 state is cleared when toggling modes.
- Fixed normal MIDI channel pressure and CC74 fallback so they apply globally to all active voices when MPE mode is disabled.

### Deferred

- Deferred feedback-loop `std::tanh` approximation after local candidates failed to produce a meaningful speedup.
- Deferred contact transient `std::tanh` replacement to later profiler-backed work.
