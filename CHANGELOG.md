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
- Added `plans/0064-slide-gesture-notation.md` to define the slide/glissando research, DAW authoring model, and phased implementation path for single-note, chord-shape, throw, and slide-out gestures.
- Added `scripts/create-slide-gesture-midi.py` and `tests/midi/guitar-ag-slide-gesture-audition.mid` for phase-1 slide gesture auditioning with current MPE pitch-bend behavior.
- Added the `Neck Slide` automatable VST parameter and offline-renderer `--neck-slide` flag for global chord-shape slide pitch motion.
- Added the `Fret Steps` automatable VST parameter and offline-renderer `--slide-fret-steps` flag to make `Neck Slide` behave more like a fretted glissando.

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
- Documented Antigravity acceptance of EG-061, closed the code-level optimization cycle, and added a human DAW/VST3 listening handoff.
- Documented the first human DAW/VST3 listening result: EG-061 sounds the same, shows the expected UI header, and has visible but modest DAW performance improvement.
- Documented how the slide plan should coexist with existing MPE pitch-bend and CC74/timbre behavior.
- Updated the slide plan and audition docs to separate the current duplicated-MPE chord-slide proxy from the future global `Slide Offset` automation lane.
- Changed the model label to `StringVoice EG-062 NeckSlide`.
- Updated the slide audition MIDI with a clean held-chord automation bed for drawing host `Neck Slide` curves.
- Clamped the aggregate pitch ratio and adjusted modal phase steps so stacked `Neck Slide`, MPE bend, whammy, vibrato, and aftertouch bend remain bounded.
- Changed the model label to `StringVoice EG-063 FretSlide`.
- Reduced `Neck Slide` to an octave range (`-12.0 st` to `+12.0 st`) after DAW listening showed the two-octave lane was broader than a practical slide gesture.
- Added fret-stepped slide pitch shaping and small fret-crossing contact ticks/scrape controlled by `Fret Steps`.
- Changed the model label to `StringVoice EG-064 FretStepCurve`.
- Remapped `Fret Steps` so 10% reaches the former 90% engagement point, spreading the audible fret-engagement range across the rest of the control.
- Documented human DAW/VST3 acceptance that the EG-064 `Fret Steps` remap makes the useful engagement range available across the slider.
- Documented Antigravity approval of the EG-064 `Fret Steps` remap and clearance to proceed to slide endings.
- Added the `Slide Tail` mode parameter with `Normal`, `Muted`, `Open`, and `Slide Off` release colors for notes ending during recent `Neck Slide` motion.
- Added offline-renderer `--slide-tail` and `--neck-slide-at` support for slide-ending smoke checks.
- Changed the model label to `StringVoice EG-065 SlideTail`.

### Fixed

- Fixed lookahead timing so pitch wheel, mod wheel/CC1, channel pressure, CC74, and key/poly aftertouch are scheduled with delayed note events instead of applying immediately.
- Fixed MPE mode changes so per-channel pitch bend, pressure, and CC74 state is cleared when toggling modes.
- Fixed normal MIDI channel pressure and CC74 fallback so they apply globally to all active voices when MPE mode is disabled.

### Deferred

- Deferred feedback-loop `std::tanh` approximation after local candidates failed to produce a meaningful speedup.
- Deferred contact transient `std::tanh` replacement to later profiler-backed work.
