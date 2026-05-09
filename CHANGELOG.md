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

### Fixed

- Fixed lookahead timing so pitch wheel, mod wheel/CC1, channel pressure, CC74, and key/poly aftertouch are scheduled with delayed note events instead of applying immediately.
- Fixed MPE mode changes so per-channel pitch bend, pressure, and CC74 state is cleared when toggling modes.
- Fixed normal MIDI channel pressure and CC74 fallback so they apply globally to all active voices when MPE mode is disabled.

### Deferred

- Deferred the reviewer-identified per-sample `std::pow`/`std::sin`/`std::tanh` performance concern to the upcoming optimization pass.
