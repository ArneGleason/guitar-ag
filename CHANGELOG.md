# Changelog

All notable project changes should be recorded here.

This changelog starts on 2026-05-09. Earlier project history is preserved across `README.md`, `DECISIONS.md`, `LEARNINGS.md`, `docs/`, and `plans/`.

## Unreleased

### Added

- Added guarded Delete/Backspace capture controls: either key discards an active
  recording without saving, or permanently deletes a selected rejected take
  while protecting candidate and approved takes.
- Added an absolute-scale selected-take waveform with peak/RMS/duration readout,
  a -12 dBFS guide, and hot/possible-clipping cues.
- Enabled JUCE ASIO support in the Windows reference-capture executable so the
  installed `Focusrite USB ASIO` driver can be selected instead of the shared
  or exclusive Windows Audio backends.
- Added a foreground-only Space shortcut that toggles reference-capture record
  and stop while preserving normal spaces in take notes and avoiding accidental
  triggers during Audio Settings interaction.
- Added Plan 0100, a 30-item phased reference-capture inventory with a seven-item
  setup/minimum-baseline stopping point, stable request generation, automatic
  app discovery, first-incomplete selection, and approval-driven progress.
- Added a human-readable inventory covering noise floor, core pick-contact
  damping comparisons, direction/dynamics/depth, flexible/stiff picks, and
  deferred finger/surface gestures.
- Added Plan 0099 and a JUCE `GuitarAGReferenceCapture` desktop companion for
  Focusrite clean-DI recording, multi-take audition, human approval, live
  headroom metering, resumable external session manifests, and 24-bit mono WAV
  capture.
- Added dependency-free capture-request generation and session-validation
  scripts plus a documented agent-request/human-approval workflow for pick,
  finger, and contact-noise references.
- Added Plan 0098 and an offline `--legacy-tone-recipe previous` regression path that reproduces the former EG-088 production tone while leaving the VST3 on the new default.
- Added Plan 0097, a stitched current-versus-candidate production-tone promotion gate spanning ordinary and deep open-string sweeps plus a compact phrase, with an exactly reconstructing signed difference.
- Added a compact promotion phrase MIDI generator and fixture covering a wound-string riff, cross-string arpeggio, upper-register melody, and held chord.
- Added Plan 0096, a no-code 0x/2x/4x/6x calibration of the accepted metal-restoration direction with an isolated 6x contribution and exact reconstruction checks.
- Added Plan 0095 with offline-only independent register-decay anchoring and fixed-Hz, plain-string-weighted inharmonic side-mode restoration controls, plus a reconstruction-checked 2x2 listening set.
- Added Plan 0094, an offline absolute-frequency register/formant envelope anchor with audition-only level compensation and progressive 35/65/100% A/B candidates.
- Added a compact E2-to-E4 open-string register audition MIDI generator and fixture.
- Added Plan 0093 with offline modal/direct pick blending and texture-event density controls, plus a five-track hybrid A/B that separates mix amount from density and isolates the added direct contribution.
- Added a clean-DI damped-guitar pick-contact capture protocol for fitting synthesized excitation statistics without sample playback.
- Added Plan 0092 and an offline-only modal-coupled pick excitation that applies deterministic plectrum force to modal quadrature state instead of mixing the rejected picked transient/contact layers directly into output.
- Added offline `--legacy-pick-excitation additive|modal` and `--legacy-modal-pick-force` research controls plus a reconstruction-checked deep-pick A/B set.
- Added Plan 0091 and offline-only legacy layer switches for separately ablating short chirp modes, the pick transient, and the per-voice contact layer without changing the VST3 or default renderer output.
- Added aligned attack/finger-noise diagnostic renders and subtraction residuals for human A/B attribution of the legacy spectral-chirp, glassiness, and overlay-like contact noises.
- Added Plan 0089, a research-backed staged path from the current reset-on-note modal/contact engine to a stateful two-polarization string with compliant pick contact, persistent left-hand state, and measured-inspired electric-instrument response.
- Added a Windows Visual Studio 2022 x64 build recipe and local Guitar-TECHS reference-audio workflow to the Studio Pattern development notes.
- Added Plan 0090 and an offline-only `StatefulStringVoice` prototype with two persistent polarization loops, fractional-delay pitch control, compliant plectrum force, finite pickup aperture/filtering, and state-preserving repicks.
- Added offline `--string-engine legacy|stateful` and `--stateful-repick 0|1` research controls.
- Added a focused stateful-waveguide audition MIDI generator and fixture covering isolated wound/plain notes, repeated same-string repicks, and a preview legato transition.

### Changed

- Promoted the human-approved Plan 0097 tone to `StringVoice EG-089 ModalFormantTone`: 1.75x modal pick force, 12% direct texture at 2.5x density, 35% amplitude/formant anchoring, harmonic-number decay, and 2x fixed-Hz/plain-string side-mode restoration. No new VST parameters or preset fields were added.
- Recorded the Plan 0097 promotion verdict: the complete accepted candidate works across ordinary picking, the deep-pick failure case, and the compact phrase, clearing it for a separate production implementation milestone.
- Recorded the Plan 0096 listening calibration: 2x side-mode restoration is good and replaces the deliberately exaggerated 6x diagnostic in the accepted offline recipe.
- Recorded the Plan 0095 listening result: the combined harmonic-number decay and 6x side-mode restoration candidate is "pretty good" and becomes the provisional offline foundation pending restoration-amount calibration.
- Recorded the Plan 0094 listening result: 35% register anchoring substantially improves high-register scale, but its coupled decay/envelope change progressively removes metal ring and brightness.
- Recorded the Plan 0093 listening calibration: 12% dense direct texture is a good medium setting and 22% is a good maximum over the 1.75x modal-force foundation.
- Recorded the Plan 0092 listening direction: retain the 1.75x modal-force attack as the foundation, then test a quieter and denser form of the current additive texture in a separate hybrid A/B.
- Recorded Plan 0091 human A/B findings: short chirp modes are negligible even in the deep/flexible/textured failure regime, the explicit attack extras become a woody digital rattle, high E sounds like a pitch-shifted low-register model, and the current comb-like finger-noise generator is rejected.
- Updated the realism vision and project learnings with the 2026-08-27 architecture diagnosis, local DI-reference measurements, recent real-time nonlinear-string/pick/pickup research, and an explicit A/B-first implementation boundary.
- Refreshed Studio Pattern session and review memory after confirming `main` is synchronized with GitHub, `v0.3.1` is published, and both Windows Release targets build successfully.
- Kept the VST3 target on the legacy engine and excluded the experimental delay buffers at compile time; the released/default offline path remains byte-identical.
- Recorded the failed Plan 0090 human A/B gate: both stateful variants sounded like synths, so the prototype remains offline and must be refined before any VST3 or left-hand-state promotion.

## v0.3.1 - 2026-05-17

### Added

- Auto drop tuning in `FretboardMapper`: when a note arrives below the current open string 0 pitch, string 0 is automatically tuned down to that exact MIDI note and stays there until `reset()`. Enables Drop D, Drop C, and any arbitrary low string pitch without manual configuration.
- `FretboardMapper::getDropSemitones()` accessor returns how many semitones string 0 is currently below standard E2 tuning.
- Register-affinity scoring term in `FretboardMapper::scoreCandidate`: low notes prefer lower strings and high notes prefer higher strings, reducing spread voicings for partial chords on outer strings. Weight (2.5) is moderate — it nudges preferences without overriding position-memory choices.
- Chord-aware string assignment for non-strum block chords: simultaneous note groups (2+ note-ons at the same sample position) now run through pitch-sorted previewFretboard assignment even when `Strum Speed` is 0%, giving natural register-ordered string placement without an audible strum delay. Previously the non-strum path dispatched each note independently.
- Partial same-sample chord assignment now uses a fixed-size group search for chords smaller than six notes, so close dyads/triads can choose a better combined string/fret layout instead of a greedy first-note assignment that leaves the next note far up the neighboring string.
- Added the PolyForm Noncommercial License 1.0.0, required notices, commercial-use guidance, and contribution notes.

### Changed

- Same-sample MPE expression resets no longer disable chord-aware assignment; mixed groups now process note-offs/expression at the sample and still assign the note-on subset as one guitar chord.
- Bass-register string selection now resists drifting to high-fret low-string duplicates when a natural low-fret string is available, keeping E2+B2-style parts stable after higher-position passages.
- Incoming MIDI diagnostics now mirror occupancy before/after fields for the pre-processing snapshot instead of leaving the after fields at zero.
- Bumped the project version to `0.3.1` for the fretboard assignment, diagnostics, and Windows package release.

## v0.3.0 - 2026-05-10

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
- Changed the model label to `StringVoice EG-080 StrumBalance`.
- Added the `Strum Balance` host parameter and Articulation-page slider to reduce generated Auto Strum downstroke or upstroke velocities from a balanced center.
- Added offline-renderer `--strum-balance` support for directional strum balance checks.
- Changed the model label to `StringVoice EG-081 ArticGroups`.
- Reorganized the Articulation page into parent/subordinate groups: `Pick Bite` owns `Pick Stiffness` and `Pick Texture`, `Pick Stroke` owns `Strum Speed` and `Strum Balance`, and `Player Feel` owns recovery/reset/meters.
- Updated release-candidate defaults for new instances and offline renders: `Pick Texture` 25%, `Strum Speed` 10%, `Strum Balance` -13%, and `Player Feel` 50%.
- Bumped the project version to `0.3.0` for the slide, strum, and player-feel release.

### Fixed

- Fixed lookahead timing so pitch wheel, mod wheel/CC1, channel pressure, CC74, and key/poly aftertouch are scheduled with delayed note events instead of applying immediately.
- Fixed MPE mode changes so per-channel pitch bend, pressure, and CC74 state is cleared when toggling modes.
- Fixed normal MIDI channel pressure and CC74 fallback so they apply globally to all active voices when MPE mode is disabled.

### Deferred

- Deferred feedback-loop `std::tanh` approximation after local candidates failed to produce a meaningful speedup.
- Deferred contact transient `std::tanh` replacement to later profiler-backed work.
