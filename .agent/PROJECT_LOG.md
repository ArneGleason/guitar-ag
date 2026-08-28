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
- Received Antigravity clearance for Phase 2 with adjustments: host-facing name `Neck Slide`, range `-24..+24 st`, and final aggregate pitch safety clamps.
- Implemented the Phase 2 global `Neck Slide` lane, offline `--neck-slide`, pitch-ratio/modal-step clamps, EG-062 model label, and a slide audition automation bed.
- Received human DAW feedback that EG-062 pitch motion worked but felt too much like a smooth tension/whammy bend and had too much range for a practical slide.
- Implemented `StringVoice EG-063 FretSlide`: reduced `Neck Slide` to `-12..+12 st`, added `Fret Steps`, and added semitone plateau shaping plus subtle fret-crossing contact texture.
- Received human DAW feedback that EG-063 was better but that the audible `Fret Steps` engagement lived almost entirely in the top 10% of the slider.
- Implemented `StringVoice EG-064 FretStepCurve`, remapping `Fret Steps` so 10% reaches the former 90% engagement point and 10% to 100% covers the useful high-engagement zone.
- Received human DAW feedback accepting EG-064's remapped `Fret Steps` control as a more useful range across the slider.
- Received Antigravity verification that EG-064's remap is mathematically sound, neutral at zero, and isolated from MPE/whammy/vibrato/aftertouch behavior; recorded it in `.agent/handoffs/20260509-201000-antigravity-fret-steps-verification.md`.
- Implemented `StringVoice EG-065 SlideTail`, adding gated `Slide Tail` release modes (`Normal`, `Muted`, `Open`, `Slide Off`) for notes ending during recent `Neck Slide` motion.
- Added offline `--slide-tail` and `--neck-slide-at` smoke checks for EG-065, confirming non-slide releases stay byte-identical and moving-slide releases diverge by tail mode.
- Received Antigravity verification approving EG-065 `SlideTail`; recorded it in `.agent/handoffs/20260509-203500-antigravity-slide-tail-verification.md`.
- Routed EG-065 to human DAW audition in `.agent/handoffs/20260509-203700-codex-slide-tail-human-audition.md`.
- Received human Bitwig feedback that EG-065 note-off slide tails were not convincing and could spike CPU at loop boundaries; recorded it in `.agent/handoffs/20260509-213804-human-slide-tail-feedback.md`.
- Implemented `StringVoice EG-066 SlideLift`, replacing the `Slide Tail` choice with a continuous slide-pressure lift envelope during `Neck Slide` motion.
- Routed EG-066 for review in `.agent/handoffs/20260509-214401-codex-slide-lift-implementation.md`.

## 2026-05-10

- Implemented `StringVoice EG-067 FingerSqueak`, revoicing finger and slide-lift contact noise away from broadband hiss toward string/fret-dependent stick-slip squeak.
- Implemented `StringVoice EG-068 SlideSqueakMotion`, making slide squeak follow `Neck Slide` movement speed and current string pitch instead of a fixed carrier.
- Implemented `StringVoice EG-069 SlideSqueakControl`, adding a separate slide-grouped squeak balance control and offline `--slide-squeak`.
- Implemented `StringVoice EG-070 PerformanceUi`, capping `Slide Squeak` at 100%, defaulting it to 20%, and grouping Performance controls under disclosure rows.
- Implemented `StringVoice EG-071 SlideSqueakDirection`, splitting slide squeak balance into upward and downward controls under the `Neck Slide` disclosure.
- Added `plans/0072-preset-model.md` to define the stock/user preset architecture before implementing preset browser/export UI.
- Added `plans/0073-pick-attack-research.md` after human DAW feedback that strummed chords sound convincing but isolated single-note pick attacks still sound synthetic.
- Implemented `StringVoice EG-072 PickStrokePlectrum`, adding `Pick Stroke` Down/Up/Alternate modes, deterministic per-attack variation, and a less tonal normal plastic-pick attack.
- Implemented `StringVoice EG-073 PlasticPickBite`, adding `Pick Bite`, widening deterministic pick angle/depth variation, and coupling `String Age` more strongly into the pick-contact imprint.
- Implemented `StringVoice EG-074 EconomyPickStroke`, making `Pick Stroke = Alternate` alternate on repeated same-string picked notes while using downstroke travel from low strings to high strings and upstroke travel from high strings to low strings.
- Added `tests/midi/guitar-ag-pick-stroke-audition.mid` and its generator to audition EG-074 with repeated picking, string crossings, string skips, strums, crosspicking, pedal riffs, and scale fragments.
- Updated the pick-stroke audition strum MIDI to preserve held left-hand chord shapes until each string is re-articulated, and added a player-feel architecture plan around cognitive load, dexterity load, and endurance.
- Implemented `StringVoice EG-075 PlayerFeel`, adding deterministic picked-note timing/energy variation from cognitive load, dexterity load, endurance, recovery time, and a manual reset action.
- Implemented `StringVoice EG-076 PlayerFeelCalibration`, recalibrating `Player Feel` for natural midpoint plus sloppy overshoot, extending/defaulting `Feel Recovery`, adding live load meters, and adding a copyable settings JSON export for audition handoffs.
- Implemented `StringVoice EG-077 PlayerFeelSlop`, slowing load-meter accumulation, widening the top-half sloppy timing/energy range, moving settings export to the global header, and storing the first human-exported audition settings snapshot.
- Implemented `StringVoice EG-078 StrumAwareFeel`, reducing Player Feel load for very short cross-string strum continuations and adding the Auto Strum interpreter plan.
- Implemented `StringVoice EG-079 AutoStrum`, adding Bot/Pro/Loose Player Feel landmarks, `Strum Speed`, exact same-sample block-chord strum scheduling, offline `--strum-speed`, and an Auto Strum audition MIDI.
- Implemented `StringVoice EG-080 StrumBalance`, adding a bipolar Auto Strum direction-balance control that reduces generated upstroke or downstroke velocity from a neutral center.
- Antigravity completed a broad retrospective review through commit `585049a` and cleared EG-066 through EG-079 with no required fixes; EG-080 remains the current unreviewed follow-up.
- Implemented `StringVoice EG-081 ArticGroups`, reorganizing Articulation into parent/subordinate control groups and setting audition-ready defaults for Pick Texture, Strum Speed, Strum Balance, and Player Feel.
- Antigravity verified EG-081 at commit `6ed57ff`, approving the articulation grouping, release-candidate defaults, session/export behavior, and `Strum Balance` string conversion.
- Human Bitwig audition accepted the EG-081 release-candidate build.
- Began the formal `v0.3.0` release pass: version bump, README refresh, user-facing release notes, macOS VST3 packaging, and GitHub release publication.
- Published `v0.3.0` on GitHub as `Guitar AG v0.3.0 - Slide, Strum, and Player Feel` with `GuitarAG-v0.3.0-macOS-vst3.zip`.
- Added PolyForm Noncommercial 1.0.0 licensing with required notices, commercial-use guidance, and contribution notes.

## 2026-05-17

- Received Claude reviewer/architect handoff for `feature/0082-fretboard-drop-tuning-register-affinity`, including a draft implementation of auto drop tuning, register affinity, and non-strum chord-aware assignment.
- Codex re-review found the draft still failed the G4+A4 low-position dyad case, because the modest register-affinity weight could not override greedy first-note assignment without damaging single-note position memory.
- Implemented a fixed-size `FretboardMapper::assignNoteGroup` path for same-sample partial chords smaller than six notes, leaving full six-note block chords on the existing sequential preview path.
- Built Release `GuitarAG_VST3` and `GuitarAGOfflineRender`, ran mapper probes and offline scenario renders, and re-rendered the feature, player-articulation, auto-strum, and pick-stroke audition MIDI clips.
- Created a follow-up handoff for Codex on the human's Windows PC to act as PC build and deployment engineer: build the Windows VST3/offline renderer, deploy the plugin to Bitwig, and audition the Plan 0082 drop-tuning/partial-chord behavior in the active PC project.
- Windows Codex diagnosed a Bitwig assignment log showing mixed same-sample MPE reset messages bypassing chord-aware assignment, implemented `StringVoice EG-088 MixedMpeChordAssignment`, built/deployed the Windows VST3, and human Bitwig audition confirmed the song section now behaves.
- Merged `feature/0082-fretboard-drop-tuning-register-affinity` back into `main` and began the `v0.3.1` release pass with a Windows package and Mac build handoff.

## 2026-08-27

- Confirmed `main` is clean and exactly synchronized with `origin/main` at `d6fad1a`; confirmed the `v0.3.1` GitHub release is published with Windows and macOS VST3 assets.
- Established `C:\code\github\guitar-ag` as the canonical Windows checkout. The clean legacy `C:\code\guitar-ag` checkout remains temporarily because the active Codex workspace prevented Windows from moving it safely.
- Configured Visual Studio 2022 x64 with JUCE at `C:\code\JUCE` and built Release `GuitarAG_VST3` and `GuitarAGOfflineRender` successfully.
- Downloaded the Guitar-TECHS P1 single-note subset outside the repository and compared the current deterministic calibration render with real direct-input notes.
- Diagnosed that the current audible engine is a modal bank plus explicit contact layers; its two delay-line buffers are not advanced or read, and all note starts reset physical state.
- Traced commit `3457200` as the last project revision with a running two-polarization delay loop and finite pickup-aperture reads.
- Added Plan 0089 to stage an A/B-safe stateful string/contact redesign before fret-dependent termination and optional nonlinear-core experiments.
- Verified the redundant `C:\code\guitar-ag` checkout is clean and older than the canonical checkout, but the active-workspace filesystem guard rejected recursive deletion. Cleanup remains for a fresh task opened from `C:\code\github\guitar-ag`.
- Created `codex/0090-stateful-waveguide-ab` and implemented an offline-only two-polarization stateful string prototype with compliant plectrum force and residual-state repicks.
- Protected the production boundary with `GUITAR_AG_ENABLE_STATEFUL_ENGINE`: the offline target includes the experiment; the VST3 does not compile or instantiate it.
- Confirmed the legacy calibration render remains byte-identical, the stateful render is deterministic, and repick-preserve/reset renders diverge only after the repeated-note section begins.
- Built both Windows Release targets, rendered at 44.1/48 kHz, and rendered a six-voice Auto Strum stress fixture at 33.0x real time with a -2.4 dBFS peak.
- Routed Plan 0090 to human listening before any VST3 promotion or left-hand-state work.
- Human completed a loudness-matched three-way A/B: legacy was usable but had a spectral-chirp attack and glassy body, while both stateful preserve/reset variants sounded like synths.
- Rejected promotion of the current Plan 0090 stateful voice. Kept the offline harness and VST3 isolation; the next DSP step is isolated-note string/pickup/body refinement before repick or left-hand-state expansion.
- Implemented Plan 0091 offline-only legacy attack/contact ablations on `codex/0091-legacy-layer-ablation`; both Release targets build and default legacy output remains byte-identical.
- Human A/B found default-setting chirp modes negligible, explicit overlays necessary for pick identity, high E suggestive of a pitch-shifted low-register modal recipe, and isolated finger noise unacceptable as a plastic-comb-like overlay.
- Accepted a motion-driven finger-noise target and routed the next attack test to the actual failure regime: deep pick, flexible stiffness, and added texture.
- Completed the targeted deep/flexible/textured A/B. The full attack became a sparse woody rattle, short chirp-mode removal remained minor, and the isolated pick/contact extras sounded like crude digital synthesis; route the next prototype through modal-string excitation instead of another overlay retune.
- Created Plan 0092 and implemented an offline-only modal-coupled pick experiment. The direct picked extras are suppressed while a deterministic stiffness/bite/texture-shaped force updates modal quadrature state; the VST3/default render remains unchanged.
- Built both Windows Release targets, confirmed the canonical default hash and repeated modal determinism, and prepared a five-track deep-pick A/B with an exactly reconstructing isolated modal contribution.
- Human selected the 1.75x modal-force direction as the next attack foundation and requested a subordinate, denser version of the old additive texture. The high-register failure was clarified as a formant-envelope problem rather than simple brightness.
- Created Plan 0093, added offline direct-texture mix and compensated event-density controls, preserved the default and Plan 0092 zero-mix hashes, and prepared an exact-residual five-track hybrid A/B.
- Added a damped-guitar clean-DI capture protocol so real pick contact can calibrate synthesized force/noise statistics without becoming a playback sample.
- Human loudness-matched A/B accepted the 2.5x-density hybrid range: 12% direct texture is a good medium setting and 22% is a good maximum. Route register/formant work through the 12% baseline.
- Created Plan 0094 and an offline absolute-frequency modal-envelope anchor, preserving pitch-relative modes/pickup geometry while progressively fixing material/contact/decay scale to low E2.
- Added audition-only register level compensation after the literal anchor made high E up to about 14 dB quieter; final per-note levels stay within about 1 dB, low E is effectively unchanged, and default/anchor-zero hashes remain exact.
- Generated a compact six-open-string register fixture and a reconstruction-checked five-track 35/65/100% anchor A/B.
- Human loudness-matched listening selected the 35% register anchor as much better than current, while identifying progressive loss of metal ring/brightness; preserve the envelope correction and separate decay from fixed-Hz metallic side-mode retention next.
- Created Plan 0095 and split the accepted 35% amplitude anchor from modal-decay anchoring, then added an offline fixed-Hz/plain-string-weighted restoration of only the lost inharmonic side-mode energy.
- Literal 1x side-mode restoration was too quiet for a useful review slot, so prepared a labelled 6x diagnostic in a 2x2 decay/restoration set; default and Plan 0094 hashes remain exact, the candidate is deterministic, and the signed contribution reconstructs exactly.
- Human loudness-matched listening called the combined harmonic-number-decay plus 6x side-mode candidate "pretty good." Keep it as the provisional offline foundation, but calibrate 0x/2x/4x/6x next because 6x was an audibility probe and the single-axis candidates received no explicit verdict.
- Created no-code Plan 0096 and prepared a 0x/2x/4x/6x restoration calibration at fixed harmonic-number decay, with the selected 6x file first and its isolated metal contribution included.
- Verified the 6x reference exactly reproduces Plan 0095, 2x/4x repeat byte-identically, all full mixes stay within 0.1 dB mean, and 0x plus the isolated contribution reconstructs 6x exactly.
- Human loudness-matched calibration selected 2x side-mode restoration as good. Replace the 6x audibility probe with 2x in the consolidated offline recipe and require a broader end-to-end gate before VST3/default promotion.
- Created Plan 0097 and a compact promotion phrase, then stitched current-versus-candidate ordinary sweep, deep-pick sweep, and phrase sections into a 26.9-second production-tone gate.
- Verified deterministic sections, consistent candidate level offset of about +1 dB across all contexts, equal 1,291,200-sample files, and exact reconstruction from the signed candidate-minus-current difference.
- Human loudness-matched listening confirmed that the complete candidate works across the ordinary, deep-pick, and compact-phrase Plan 0097 gate. Proceed with a separate production implementation while preserving the previous tone offline for regression.
- Implemented Plan 0098 as `StringVoice EG-089 ModalFormantTone`, promoting the exact accepted recipe into both VST3 and default offline builds without new parameters or preset fields.
- Added `--legacy-tone-recipe previous`; verified the new default and previous recipe byte-for-byte against all six Plan 0097 section references, repeated the new single-note hash exactly, and reproduced the canonical EG-088 single-note hash.
- Built Windows Release `GuitarAG_VST3` and `GuitarAGOfflineRender`; installed-plugin identity/tone verification remains the human gate.
- Created `codex/0099-reference-capture-lab` and added a standalone JUCE
  companion for Focusrite clean-DI capture, live headroom metering, 24-bit mono
  WAV recording, multi-take audition, human approval, and resumable external
  session manifests.
- Added focused request generation, session validation/approved-path output,
  script unit tests, and documentation for ringing/hand-damped/foam-damped
  comparisons without treating subtraction as a pure physical null.
- Built all three Windows Release targets. The capture app found the Focusrite
  driver, loaded a generated request, retained a seeded approved take across a
  restart, and exposed that take through the validator. Real guitar recording
  and playback remain the human gate.
- Created Plan 0100 after human feedback showed that single request files did
  not provide a usable capture queue. Added a 30-item, five-phase inventory but
  made the first stopping point only seven items: input noise floor plus six
  low/high E ringing/hand-damped/foam-damped baselines.
- Added automatic Documents-known-folder inventory discovery, first-incomplete
  selection, `[done]` progress labels, inline research reasons, and immediate
  refresh when approvals change. Inventory sessions use fixed directories so
  progress cannot be detached by changing folders.
- Generated the real local inventory under the OneDrive-redirected Documents
  folder. Unit tests pass, the Windows app builds, default discovery creates the
  correct first session, a seeded approval advances to Phase 1, and inventory
  startup no longer creates stray manual-session folders.
- Added a foreground-only Space transport shortcut after human usability
  feedback: Space toggles record/stop, but is suppressed during take-note entry,
  Audio Settings interaction, file choosers, and whenever another application
  is foreground.
- Diagnosed the first real Windows Audio reference take and confirmed abrupt
  sample discontinuities plus an approximately 8 ms zero-filled gap even though
  the threaded disk writer reported no overflow.
- Implemented Plan 0101 by compiling JUCE ASIO support only into the Windows
  reference-capture target. The Release build contains the ASIO backend, and
  the workstation registers `Focusrite USB ASIO`; a clean human capture remains
  the final gate.
- Implemented Plan 0102 capture triage: Delete or Backspace discards an active
  recording, while the same keys permanently erase only a selected rejected
  take. Candidate/approved takes and non-capture focus contexts are protected.
- Added an absolute-scale selected-take waveform with peak, RMS, duration,
  -12 dBFS guides, and hot/possible-clip cues. Disposable ASIO runtime tests
  verified both abort keys, both rejected-take deletion keys, manifest/file
  removal, status guards, one-action key edges, and the rendered layout.
- Replaced Plan 0100's broad active inventory with Plan 0103's focused low-E
  evaluation gate: approved noise floor plus ringing down/up/alternate and
  hand-damped down/up/alternate batches.
- Defined the exercise as 4/4/12 ringing strokes and 6/6/12 damped strokes,
  natural timing without a metronome, one required approved WAV batch per item,
  and a fixed 44.1 kHz ASIO rate matching the existing noise-floor reference.
- Regenerated the real OneDrive Documents inventory without changing any of 22
  existing session files, rebuilt the Windows Release capture app, and opened
  it on `low-e-eval-ringing-down`. The next action is human capture of all six
  items followed by a full current-model comparison.
