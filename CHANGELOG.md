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
- Documented Antigravity approval of EG-065 and routed the next step to human DAW audition.
- Documented human DAW feedback that EG-065 note-off-based slide tails were not convincing and could misbehave around Bitwig loop boundaries.
- Replaced the `Slide Tail` choice with a continuous `Slide Lift` parameter that reduces fretted pressure during actual `Neck Slide` motion instead of waiting for note-off.
- Added offline-renderer `--slide-lift` support and removed `--slide-tail`.
- Changed the model label to `StringVoice EG-066 SlideLift`.
- Added `plans/0067-finger-squeak.md` to capture the finger/string squeak revoice research and verification targets.
- Revoiced `Finger Noise` from mostly broadband scrape/body noise toward short stick-slip squeak impulses with string/fret-dependent pitch and stronger wound-string texture.
- Revoiced `Slide Lift`/fret-slide scrape toward filtered friction plus ridged squeak instead of mostly high-passed random noise.
- Updated the `Finger Noise` info note and changed the model label to `StringVoice EG-067 FingerSqueak`.
- Added `plans/0068-motion-coupled-slide-squeak.md` after human spectrum/listening feedback showed EG-067 still behaved like a fixed hiss layer.
- Made slide squeak motion-coupled: pulse density, amplitude, decay, and carrier pitch now follow `Neck Slide` movement speed and current string pitch.
- Reduced remaining broadband scrape weighting in the global `Finger Noise` approach/release layer.
- Updated slide/finger-noise info notes and changed the model label to `StringVoice EG-068 SlideSqueakMotion`.
- Added a `Slide Squeak` performance parameter, grouped with the slide controls, to balance motion-coupled slide finger noise independently from the older `Finger Noise` approach/release layer.
- Added offline-renderer `--slide-squeak` support for slide-noise balance checks.
- Changed the model label to `StringVoice EG-069 SlideSqueakControl`.
- Capped `Slide Squeak` at 100% and changed its default to 20% after human DAW audition found that balance more useful.
- Reorganized the Performance page around primary controls with disclosure rows for slide, finger-noise, and feedback character tweaks.
- Changed the model label to `StringVoice EG-070 PerformanceUi`.
- Split slide contact volume into direction-aware `Slide Squeak Up` and `Slide Squeak Down` controls so upward throws and downward returns can be balanced separately.
- Added offline-renderer `--slide-squeak-up` and `--slide-squeak-down` support while preserving `--slide-squeak` as the legacy/upward alias.
- Added `plans/0072-preset-model.md` to define the recommended stock/user preset architecture before implementing preset UI.
- Changed the model label to `StringVoice EG-071 SlideSqueakDirection`.
- Added `plans/0073-pick-attack-research.md` to diagnose the synthetic single-note pick attack and outline a direction-aware plectrum improvement pass.
- Added the `Pick Stroke` host parameter and Articulation-page choice with `Down`, `Up`, and default `Alternate` modes.
- Added offline-renderer `--pick-stroke down|up|alternate` support for pick-stroke calibration renders.
- Added `plans/0074-pick-stroke-plectrum.md` to document the first normal-plastic-plectrum implementation pass.
- Changed the model label to `StringVoice EG-072 PickStrokePlectrum`.
- Revoiced the default picked attack toward a normal plastic plectrum by reducing exposed tonal contact ring/chirp energy, shortening the pick transient, and letting stroke direction shape release polarization.
- Made per-attack variation deterministic by seeding picked voices from note/channel/string/fret, render timeline, stroke direction, gesture, and a resettable attack counter.
- Added the `Pick Bite` host parameter and Articulation-page slider to control how strongly pick contact imprints on the string attack.
- Added offline-renderer `--pick-bite` support.
- Changed the model label to `StringVoice EG-073 PlasticPickBite`.
- Rebalanced the normal pick-contact model away from brittle/glassy coherent ring by lowering default rough-contact onset, broadening the compliant pick contact, and increasing deterministic pick angle/depth variation.
- Coupled `String Age` more directly into picked attacks by damping contact ring/chirp brightness and lowering the contact-scratch high-pass coefficient as strings age.
- Added `plans/0075-plastic-pick-bite.md` to document the physical pick-contact mental model and verification notes.
- Changed the model label to `StringVoice EG-074 EconomyPickStroke`.
- Updated `Pick Stroke = Alternate` so repeated same-string picked notes alternate, while cross-string picked notes use economy direction based on string travel: low-to-high string movement uses downstrokes and high-to-low movement uses upstrokes.
- Updated the `Pick Stroke` info note and added `plans/0076-economy-pick-stroke.md` for the right-hand motion model.
- Added `scripts/create-pick-stroke-audition-midi.py` and `tests/midi/guitar-ag-pick-stroke-audition.mid` for DAW auditioning repeated picking, string crossings, string skips, strums, crosspicking, pedal riffs, and scale fragments.
- Added `plans/0077-pick-stroke-audition-midi.md` and documented the pick-stroke audition workflow in `docs/audition-midi.md` and `docs/test-plan.md`.
- Updated the pick-stroke audition chord strums so fretted notes sustain until the same string is struck again, better matching a held left-hand shape with right-hand re-articulation.
- Added `plans/0078-player-feel-model.md` to frame deterministic player interpretation around cognitive load, dexterity load, endurance, timing, and energy variation.
- Changed the model label to `StringVoice EG-075 PlayerFeel`.
- Added `Player Feel`, `Feel Recovery`, and `Reset Feel` controls for deterministic picked-note timing/energy variation driven by cognitive load, dexterity load, and endurance.
- Added offline-renderer `--player-feel`, `--player-feel-recovery`, and `--player-feel-reset-at` support.
- Added live Player Feel meter readouts for cognitive load, dexterity load, and endurance.
- Added an `Export Settings` UI action that opens a copyable JSON snapshot of current plugin parameters and Player Feel meters for audition handoffs.
- Changed the model label to `StringVoice EG-076 PlayerFeelCalibration`.
- Recalibrated `Player Feel` so 50% is near the previously accepted natural EG-075 maximum and 100% deliberately overshoots into sloppier playing.
- Changed `Feel Recovery` to default to 2.0 seconds and extended its range to 8.0 seconds for longer accumulated-load auditioning.
- Added `docs/audition-settings/20260510-eg076-pick-tone-reference.json` as a human-provided reference setting snapshot for a useful picked tone/feel context.
- Changed the model label to `StringVoice EG-077 PlayerFeelSlop`.
- Reduced Player Feel load impulse rates so cognitive/dexterity/endurance meters do not saturate as quickly.
- Expanded the top half of `Player Feel` with a wider nonlinear timing and velocity/energy variation range for intentionally sloppier playing at 100%.
- Moved `Export Settings` from the Articulation page to the global header.
- Added `plans/0079-auto-strum-interpreter.md` to outline the future Auto Strum interpreter.
- Changed the model label to `StringVoice EG-078 StrumAwareFeel`.
- Made Player Feel strum-aware: very short cross-string note clusters now count as a continuing right-hand stroke with reduced cognitive and dexterity load.
- Increased header vertical spacing so the global `Export Settings` button no longer crowds the heading text.
- Changed the model label to `StringVoice EG-079 AutoStrum`.
- Added Bot/Pro/Loose landmarks under the `Player Feel` slider so the control reads as perfect machine timing, controlled human feel, and deliberately loose playing.
- Added the `Strum Speed` host parameter and Articulation-page slider to fan exact same-sample block chord note-ons across assigned guitar strings.
- Wired Auto Strum into `Pick Stroke` direction, fretboard assignment, delayed note scheduling, and Player Feel timing/load accounting while leaving `Strum Speed = 0%` neutral.
- Added offline-renderer `--strum-speed` support.
- Added `scripts/create-auto-strum-audition-midi.py` and `tests/midi/guitar-ag-auto-strum-audition.mid` for block-chord Auto Strum auditioning.

### Fixed

- Fixed lookahead timing so pitch wheel, mod wheel/CC1, channel pressure, CC74, and key/poly aftertouch are scheduled with delayed note events instead of applying immediately.
- Fixed MPE mode changes so per-channel pitch bend, pressure, and CC74 state is cleared when toggling modes.
- Fixed normal MIDI channel pressure and CC74 fallback so they apply globally to all active voices when MPE mode is disabled.

### Deferred

- Deferred feedback-loop `std::tanh` approximation after local candidates failed to produce a meaningful speedup.
- Deferred contact transient `std::tanh` replacement to later profiler-backed work.
