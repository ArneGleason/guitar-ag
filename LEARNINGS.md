# Learnings

Use this file as the running memory for the project. Add concise notes when testing, debugging, researching, or making discoveries.

## Initial Learnings

- MPE is the required control model, not MPC.
- The first success milestone is independent per-note bending, not perfect guitar realism.
- The plugin should output a DI-style electric guitar tone and rely on external amp sims for the MVP.
- Avoid building amp/cab simulation in the first pass.
- Start with a simple string model and improve realism later.
- Keep MPE behavior easy to debug with visible voice/channel state if possible.

## Testing Notes

Add notes here when something sounds good, bad, unstable, too bright, too dull, too synthetic, too clicky, or too CPU-heavy.

## DAW Compatibility Notes

Add notes here for behavior in specific hosts such as Bitwig, Reaper, Ableton Live, Logic, or others.

## 2026-04-25 — Bitwig VST3 cache behavior

- After replacing the VST3 bundle, Bitwig showed two `Guitar AG` entries with the same name.
- One entry appeared to instantiate the older sine-test build while another loaded the newer plucked-string build.
- The plugin editor should show build identity so stale-plugin/cache confusion is visible during testing.

## 2026-04-25 — Plugin shell setup environment

- The repository started as documentation plus empty `src` and `tests` placeholders.
- The folder is not currently a Git repository, so local change tracking is unavailable until Git is initialized or the project is placed inside a worktree.
- Apple clang is available from Command Line Tools.
- CMake 4.3.2 is installed and can detect Apple clang.
- JUCE was cloned to `/Users/arnegleason/code/JUCE` and works with this project through `-DJUCE_PATH=/Users/arnegleason/code/JUCE`.
- Full Xcode is not installed; Xcode-generator builds need Xcode, but Make/Ninja builds should only need CMake, JUCE, and the command-line compiler.
- The plugin shell configures and builds successfully with CMake and Command Line Tools.
- The CMake project needs both C and CXX languages enabled because JUCE compiles some C sources.
- First JUCE/VST3 builds may print warnings from JUCE/VST3 SDK internals, but the VST3 bundle can still be produced.

## 2026-04-25 — Realism research direction

- Guitar realism should be treated as both an instrument-model problem and a player-interpretation problem.
- The instrument model needs string excitation, damping, fret/finger/collision behavior, pickup position/width/circuit coloration, and clean DI output.
- The player interpretation layer should eventually convert MIDI/MPE into guitar gestures such as string/fret choice, pick direction, hammer-ons, pull-offs, slides, bends, vibrato, and muting.
- Recent DDSP/neural work is useful research context, but the near-term plugin should remain physically controlled and real-time safe.

## 2026-04-25 — Basic string voice implementation

- The first Karplus-Strong-style voice builds successfully in the JUCE VST3 target.
- A fixed 8192-sample delay line per voice is enough for the current guitar-oriented range at common sample rates.
- The first implementation deliberately favors simplicity over perfect tuning or realism; fractional delay and pickup-position shaping remain future work.

## 2026-04-25 — Pick attack feedback

- The first string voice sounded like old-school plucked synthesis.
- Its attack read more like a hard tap/fretboard tap than a picked string.
- Volume, sustain, and release were acceptable for now, so the next polish should focus on excitation rather than decay or gain.

## 2026-04-25 — String voice listening feedback after pick polish

- `StringVoice KS-002` is more mellow and less harpsichord-like.
- It still needs more string scrape and twang/quack.
- The twang/quack may belong partly in pickup modeling rather than only string excitation.
- Note-off needs a left-hand damping model; short notes should damp quickly and longer held notes should release more gently.

## 2026-04-25 — Guitar-TECHS reference setup

- Guitar-TECHS provides useful DI, amp-mic, ego, exo, and synchronized per-string MIDI references.
- The first local subset is `P1_singlenotes.zip`, stored outside the repo under `/Users/arnegleason/code/reference-audio/Guitar-TECHS/`.
- `P1_singlenotes` includes one long DI WAV, one long amp-mic WAV, stereo MP3 perspective captures, and a MIDI file with 142 note events.
- The initial reference workflow can locate real note regions by MIDI timestamps before deeper spectral comparison tools exist.

## 2026-04-25 — KS-003 render comparison

- The first calibration render is stored at `/Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-ks003-e35bf11-single-note-calibration.wav`.
- A rough harmonic comparison against Guitar-TECHS DI notes supports the listening impression that KS-003 is too smooth/nylon-like.
- The render appears to have much less upper-harmonic energy than the real DI notes.
- The next likely sound experiment is a simple pickup-position/tone model rather than a plain brightness boost.

## 2026-04-25 — Pickup model first step

- The first pickup/tone model is fixed and intentionally simple.
- It shapes the mixed string signal after voices, which is easy to test but less physically precise than per-string pickup readouts.
- If the calibration render improves, a future version should move toward a better pickup-position model with controllable pickup position/width and possibly per-voice processing.

## 2026-04-25 — KS-004 render comparison

- `StringVoice KS-004 Pickup-001` moved harmonic-balance metrics in the right direction, but it became quieter/thinner than KS-003.
- The fixed post-mix comb is a useful prototype but not a good long-term pickup model because it ignores each note's string length.
- The next smaller experiment is per-voice pickup readout plus lighter post-mix conditioning, before changing the string/exciter again.

## 2026-04-25 — KS-005 render comparison

- `StringVoice KS-005 Pickup-Voice` restored body and level compared with KS004.
- The upper-harmonic measurement mostly fell back near KS003, so per-voice pickup readout alone did not create the missing steel/electric brightness.
- The next likely improvement should target the string/exciter model: pick contact/scrape, stiffness/dispersion, or a small real-time-safe nonlinear component that produces additional partials before pickup/tone shaping.

## 2026-04-25 — KS-006 steel exciter implementation

- The next testable sound hypothesis is now implemented as `StringVoice KS-006 SteelExcite`.
- The change deliberately targets excitation and feedback partial content, not pickup filtering.
- The main listening risk is harshness/fizz from the pick-contact burst or soft-clipped slope term at high velocity.

## 2026-04-25 — KS-006 harmonic evolution analysis

- KS006 improved the mean upper-to-low harmonic metric versus KS005, but not enough to approach the Guitar-TECHS DI references.
- The real DI references show time-varying behavior that the current KS model mostly lacks: larger harmonic phase drift, moving upper-harmonic emphasis, and harmonic-dependent decay.
- A useful next hypothesis is a time-varying attack/string model: pitch/tension settling, dispersion or inharmonicity, moving contact damping, and/or a short bridge/body/contact resonance layer that changes during the first few hundred milliseconds.

## 2026-04-25 — KS-007 phase-settling implementation

- The next isolated sound experiment targets phase/dispersion only.
- `StringVoice KS-007 PhaseSettle` adds a time-varying all-pass stage in the string feedback loop.
- The KS006 exciter, pickup readout, and tone stage were intentionally left unchanged so the next render can test phase settling without mixing in another new property.

## 2026-04-25 — KS-007 render comparison

- The isolated phase-settling experiment moved the phase metrics in the intended direction.
- KS007 raised phase-drift standard deviation across harmonics 2-8 from about 1.0 cents to about 3.5 cents, with the DI reference around 12.4 cents.
- KS007 also reduced upper-harmonic energy and made the strongest upper-harmonic emphasis less mobile, so all-pass phase movement alone is not enough.
- The next isolated property should probably be time-varying amplitude/resonance behavior, such as harmonic-dependent damping or a short moving contact/resonance layer, while preserving the useful phase-settling idea for later recombination.

## 2026-04-25 — KS-008 moving resonance implementation

- The next isolated sound experiment targets time-varying amplitude/resonance behavior.
- `StringVoice KS-008 MovingRes` removes the KS007 all-pass stage and adds moving resonator weights across the 11th, 7th, and 5th harmonics.
- This should test the moving upper-harmonic emphasis metric separately from the phase-settling result.

## 2026-04-25 — KS-008 render comparison

- KS008 preserved KS006's overall upper-harmonic balance while improving the high-air band.
- The h4-h12 relative spread landed almost exactly on the current DI reference average, which suggests the moving resonance layer is useful.
- Strongest upper-harmonic movement did not improve beyond KS006, so the current moving-resonance implementation shapes spectrum more than it creates moving emphasis.
- Phase drift stayed low, as expected, because KS008 intentionally left out the KS007 all-pass stage.
- A later combined model should probably test KS008-style resonance plus a gentler/reworked phase-settling stage.

## 2026-04-25 — KS-009 harmonic damping implementation

- The next isolated sound experiment targets harmonic-dependent decay.
- `StringVoice KS-009 HarmDamp` keeps KS008 moving resonance and adds a dynamic feedback damping tilt.
- The high-frequency feedback component starts nearly neutral and settles lower over the first few hundred milliseconds.
- This should test partial-dependent decay without adding phase-settling, new excitation, or a new pickup model.

## 2026-04-25 — KS-009 render comparison

- KS009 made the note decay more dynamically and moved the strongest upper-harmonic shift metric closer to the DI reference.
- The current damping tilt is too strong for high harmonics: h8 decay overshot the reference average, while KS008 was already close on that metric.
- KS009 slightly reduced upper-harmonic and air-band energy compared with KS008.
- The damping concept is useful, but a future pass should make it gentler or more selective rather than applying broad high-frequency loss.

## 2026-04-25 — KS-010 gentle damping retune

- `StringVoice KS-010 GentleDamp` retunes the KS009 damping idea rather than adding a new feature.
- The high feedback target is closer to neutral, the transition is slower, and the moving resonance layer is no longer damped by the harmonic tilt.
- The goal is to keep some KS009 moving-emphasis benefit while restoring KS008-like h8 decay and air-band energy.

## 2026-04-25 — KS-010 render comparison

- KS010 restored most of KS008's level, upper-harmonic energy, and air-band behavior while preserving a small amount of KS009's stronger moving-emphasis behavior.
- The user's listening feedback that higher strings sound more convincing than lower strings is supported by grouped analysis.
- Treating E2/A2/D3 as lower or wound-side notes, the reference has much stronger early/mid upper and air energy plus more phase drift than the render.
- A likely next experiment is string-class-aware behavior: lower notes should receive a wound-string interaction model with more scrape/air, stronger evolving upper content, and possibly more phase irregularity than plain-string notes.

## 2026-04-25 — KS-011 wound open-string implementation

- The calibration MIDI is now treated as open strings for the next sound experiment.
- `StringVoice KS-011 WoundOpen` applies a wound-string interaction layer only to E2, A2, and D3.
- G3, B3, and E4 are intended to remain close to KS010.
- The goal is to improve lower-string scrape/air and evolving roughness without a global tone change.

## 2026-04-25 — CMake build label cache issue

- The visible plugin model label should not be stored as a CMake cache variable.
- CMake preserved an older `GUITAR_AG_MODEL_LABEL` value in `build/CMakeCache.txt`, causing the Bitwig UI label to disagree with source-controlled `CMakeLists.txt`.
- The label is now a normal CMake variable so source changes update `BuildInfo.h` reliably.

## 2026-04-25 — KS-011 render comparison

- `StringVoice KS-011 WoundOpen` successfully isolated its change to the lower open-string notes E2, A2, and D3.
- The wound group gained about 17% upper-to-low energy and about 30% early air-to-low energy versus KS010 without shifting the plain-string notes.
- The wound group strongest upper-harmonic shift moved closer to the DI reference, which suggests the added interaction layer is directionally useful.
- The wound group is still far below the DI reference for air/upper energy, and phase drift stayed almost unchanged.
- The next wound-string experiment should probably add motion-coupled phase/dispersion or roughness that changes the string state, instead of only mixing more noise into the pickup readout.

## 2026-04-25 — KS-012 wound phase implementation

- The next isolated lower-string experiment is `StringVoice KS-012 WoundPhase`.
- It keeps KS011's wound interaction layer and adds a gentler wound-only all-pass phase stage in the feedback loop.
- This specifically targets the missing lower-string phase irregularity shown by the Guitar-TECHS DI comparison.
- The main risk is dulling or blurring the wound notes, as the earlier global KS007 phase-settling experiment improved phase metrics but hurt brightness.

## 2026-04-25 — KS-012 render comparison

- KS012 confirmed that putting even a gentle all-pass phase stage directly in the wound-string feedback path is risky.
- The change increased lower/wound mean absolute phase drift, but it did not create the large phase-drift standard deviation seen in the real DI references.
- It also reduced wound-string upper/air energy and collapsed the strongest upper-harmonic movement metric.
- This suggests the missing wound-string behavior is not just a simple phase-settling problem; it likely needs motion-coupled roughness, moving partial excitation, or selective dispersion that does not blur the whole feedback signal.
- The next sound pass should probably use KS011 as the base, not KS012.

## 2026-04-25 — KS-013 wound motion implementation

- `StringVoice KS-013 WoundMotion` removes the KS012 wound all-pass stage and returns to the KS011-style feedback path.
- It adds a separate wound-only moving partial/roughness layer so lower-string motion can gain evolving upper content without broadly blurring the feedback loop.
- The layer is mixed mostly into pickup readout, with only light coupling into the moving resonance and loop.
- The main listening risk is metallic or fizzy wound-string noise if the added upper partials are too prominent.

## 2026-04-25 — KS-013 render comparison

- KS013 improved the lower/wound moving upper-harmonic metric substantially and restored air compared with KS012.
- The user's listening feedback identified the failure mode clearly: the lower-string layer reads as a short synthetic white-noise tail rather than metallic wound-string ring.
- The current analysis metrics can reward extra air-band energy even when the perceptual material is wrong, so subjective listening should override the metric here.
- The next retune should keep the moving-partial idea but reduce stochastic noise drive, lengthen the resonator behavior, and lower the direct pickup mix.

## 2026-04-25 — KS-014 wound ring retune

- `StringVoice KS-014 WoundRing` retunes KS013 rather than adding a new architecture.
- The high-passed noise and slow random texture contributions are reduced sharply.
- The wound-motion resonators are longer-lived, lower in level, and driven more by string motion than noise.
- The goal is metallic/string-coupled ring on lower strings instead of a short hiss envelope.

## 2026-04-26 — Harmonic touch research

- Natural/artificial harmonic gestures fit the current modal-cluster voice well because the physical action is mostly modal selection: touching a node damps modes that move at that point while allowing modes with a node there to ring.
- The first three useful harmonic families are 1/2, 1/3, and 1/4 of the speaking string length, corresponding to 12th, 7th/19th, and 5th/24th fret regions.
- Higher-division harmonics become weaker and more precision-sensitive, so the first implementation should make 1/4 quieter than 1/3, and 1/3 quieter than the midpoint octave.
- The note-start implementation is a good first pass for the described “touch, strike, lift” gesture. A later pass could add a true time-varying touch damper if harmonics need to be faded in while a note is already ringing.
- Research references used for this pass: Aaron Shearer Foundation guitar harmonic node chart, UNSW strings/harmonics notes, Woodhouse's guitar pluck synthesis paper, and the Columbia waveguide guitar synthesis notes.
- Listening feedback on KS035 suggests harmonic articulations should act as a self-contained gesture, not as a layer on top of the current pick material settings. This makes MIDI/automation authoring easier because a harmonic can be added without also automating the pick controls away and back.

## 2026-04-26 — Model label naming

- The `KS` model prefix no longer describes the current voice well. Starting with `EG-037`, visible model labels use `EG` for electric guitar while preserving the existing numeric sequence.
- String age is better modeled at the string source than as a final tone filter: older strings should reduce upper-partial excitation and high-mode sustain while keeping the basic clean DI character.

## 2026-04-26 — Pickup position research

- Pickup position is not just a brightness knob. It acts like a harmonic comb because each mode has different displacement at each string position.
- Useful sources for the first model were J. Donald Tillman's pickup position/width and pickup mixing notes, plus the Physics of Shred pickup placement explainer.
- A humbucker's wider sensing aperture should naturally smooth high partials. Modeling it as two nearby readout points also creates spacing-dependent notches.
- Pickup-position UI markers are best treated as harmonic landmarks, not as promised sweet spots for every note, because fretting moves the effective speaking length and node layout.

## 2026-04-26 — Bridge intonation model

- Bridge intonation error should be tied to the fretboard assignment, not applied as a global pitch drift.
- A simple saddle-offset formula gives the right shape for this milestone: open strings stay tuned, low frets are subtle, and higher frets reveal more error.
- The editor is now grouped into collapsible sections so future controls can remain discoverable without making the plugin window permanently taller.

## 2026-04-25 — KS-014 render comparison and research pivot

- KS014 reduced the KS013 air/noise boost but did not create a more realistic open-string sound by ear.
- The analysis metrics still moved within the same narrow feature space, which supports the user's concern that optimizing these dimensions can miss the perceptual target.
- The additive wound-layer branch should be paused.
- Research on electric guitar synthesis and pickup modeling suggests a bigger next experiment: dual-polarization string motion plus a more physical pickup-position/width readout.
- Noise should become incidental pick/contact texture, not the primary source of wound-string realism.

## 2026-04-25 — KS-015 dual pickup implementation

- `StringVoice KS-015 DualPickup` is a basin-jump experiment, not a retune of KS014.
- The voice now has two simplified string polarizations with weak coupling and separate high-frequency damping behavior.
- Pickup output now comes mainly from velocity at a finite-width pickup aperture, rather than additive wound air/ring layers.
- Wound open strings still get slightly different coupling/damping, but no longer receive a dedicated sustained noise or ring layer.
- This version should be judged by listening first, because the old air/upper metrics were shown to reward perceptually wrong changes.

## 2026-04-25 — Offline render harness

- Added `GuitarAGOfflineRender`, a CMake console target that renders MIDI through the shared `AudioEngine` and writes WAV output.
- The harness enables faster unattended DSP experiments without opening Bitwig for every candidate.
- It does not replace DAW verification because it bypasses plugin hosting, scanning, UI, and any DAW gain/routing choices.
- The first KS015 offline render had very high waveform correlation with the Bitwig render, but was about 10 dB louder, likely due to DAW project gain or export routing.
- Rendering with `--gain 0.316` closely matches the Bitwig KS015 render level while preserving about 0.999 waveform correlation.

## 2026-04-25 — Perceptual analysis upgrade

- Added `scripts/analyze-perceptual-distance.py` to compare gain-normalized render notes against same-pitch DI reference candidates.
- The report includes log-spectral distance, attack/early/late distance, spectral flatness, harmonic-band fraction, and spectral flux.
- Initial results line up better with listening feedback: KS013 is closer on some wound-string spectral distances but too flat/noise-like, KS014 reduces that noise but still does not become realistic, and KS015 is too spectrally sparse/static.
- This supports the hypothesis that the missing property is not just "more high frequency"; the model needs coherent electric-guitar string/pickup behavior with the right amount of broadband complexity.

## 2026-04-25 — KS-016 radical experiment loop

- A pure explicit-modal DI string bank was a useful falsification: it moved away from the desired sound analytically, becoming too clean and too harmonically stable.
- Adding sparse inharmonic side modes as independent layers did not solve the lower-string realism problem; those modes likely need to be coupled into the string/contact dynamics rather than sprinkled on top.
- A hybrid waveguide plus quiet modal residual preserved the KS015 behavior but was not a large enough jump on its own.
- A simple pickup/cable resonance in the DI tone stage was the first candidate in this loop to improve the current perceptual distance versus KS015, moving mean full log-spectral distance from about 30.16 dB to about 29.34 dB.
- The broader hypothesis is that physically modeled electric guitar may be hard because DI pickup output exposes raw string/pickup/circuit interactions that acoustic bodies, amp chains, or sample layers often mask.

## 2026-04-25 — KS-017 modal cluster listening pivot

- User listening feedback identified `guitar-ag-offline-ks016-modalcluster-dirty-single-note-calibration.wav` as much more realistic than the installed hybrid/pickup version.
- Recreated the modal-cluster branch as `StringVoice KS-017 ModalCluster`; the new offline render is byte-for-byte identical to the earlier liked dirty render.
- The current analyzer penalizes the modal cluster, but several lower-level metrics hint why it can still sound better: strong discrete partials, more metal-like high harmonic structure, and E2 h8 decay close to the DI reference.
- The current perceptual analyzer should be treated as a diagnostic tool, not the objective. It likely over-weights exact spectral-bin matching and under-weights perceptual string identity.
- Next analysis work should search for features that correlate with the user's realism judgment rather than forcing the model back toward lower full-spectrum distance.

## 2026-04-25 — Guitar-likeness critic corpus seed

- Added a small labeled corpus manifest for guitar-likeness work before introducing any ML dependency.
- The first corpus has one positive real DI reference, one preferred synth render, and four rejected synth renders.
- Running the corpus summary confirms the mismatch we need to solve: `KS017 ModalCluster` is the preferred render even though current full log-spectral distance is worse than several rejected examples.
- This gives future embedding/classifier experiments a concrete local target: agree with reference DI and human listening labels, not generic internet `electric guitar` labels.

## 2026-04-25 — KS-018 pick regime implementation

- Added `tests/midi/velocity-ladder-open-strings.mid` to make velocity expression easier to test by ear.
- `StringVoice KS-018 PickRegimes` keeps the KS017 modal-cluster core but maps velocity into pluck position, contact width, harmonic tilt, side-mode strength, and short-lived attack modes.
- The first velocity-ladder analysis confirms the render changes more than loudness: upper-to-low harmonic balance increases roughly 5x to 7x from velocity 25 to 127 across the open strings.
- The old DI spectral-distance analyzer dislikes KS018 more than KS017, which is expected because this experiment intentionally changes excitation regime rather than matching the previous velocity-100 spectrum.

## 2026-04-25 — KS-019 hard pick retune

- Listening feedback said KS018 improved dynamics but compressed the whole range into gentle picking; even the loudest notes felt like medium-soft strokes.
- `StringVoice KS-019 HardPickRegimes` shifts the velocity curve upward, adds a high-velocity hard-strike region, raises output, and boosts high modal/side-cluster excitation.
- Velocity-ladder analysis confirms a much stronger regime spread: upper-to-low harmonic balance rises roughly 11x to 13x from velocity 25 to 127.
- KS019 may overshoot on wound-string air-band content, but it should answer the immediate listening question: can the model produce genuinely aggressive picking at the top of the velocity range?

## 2026-04-25 — First fretboard mapping pass

- Guitar string/fret assignment is commonly treated as an optimization problem over valid fretboard locations, with costs for position and movement. Full-phrase systems often use dynamic programming; this project now starts with a smaller real-time heuristic.
- The mapper should live above `StringVoice`: the voice should synthesize the assigned string, not infer playable guitar position from pitch alone.
- User listening feedback exposed a useful test case: notes fretted on the low E, A, or D strings should keep wound-string behavior even when their MIDI note number is not one of the open-string pitches.
- The first implementation favors first/second position, avoids occupied strings for chords where possible, and lets the remembered hand position drop slowly after high notes.

## 2026-04-25 — KS-021 sustain and velocity ceiling

- Fretboard mapping passed the first listening check, so the next change should leave mapper behavior untouched.
- Clean DI electric guitar needs more held-note sustain than KS020 provided; the modal bank can decay more slowly without adding compressor behavior yet.
- The most useful high-velocity strike character appears to be around the previous 67-68% velocity region, so KS021 caps strike timbre there while still allowing full incoming velocity to affect loudness.

## 2026-04-26 — KS-022 wound taper and pick attack

- A single wound/plain boolean is too coarse once the fretboard mapper assigns real strings. The low E, A, and D strings need graduated wound behavior.
- The intended first taper is low E 1.00, A 0.67, D 0.33.
- Capping the top of the velocity curve is not the same as scaling the whole curve. KS022 remaps the full MIDI range into the previous 0-68% strike-character range.
- The hammer-like attack likely comes from too much instantaneous transient/chirp energy, so this pass lowers the transient/chirp layers and adds a very short onset ramp.

## 2026-04-26 — KS-023 clear DI pass

- User feedback described KS022 as improved but veiled, like the sound was passing through a warm paper speaker or cone resonance.
- The likely short-term suspects are the post-mix body/tone stage and pickup aperture smoothing, not the fretboard mapper.
- The first corrective pass should reduce low-passed body coloration, narrow the modeled pickup aperture, and preserve more dry/presence detail before inventing another string layer.
- The requested velocity curve can be represented by three main points: 0.10 input -> 0.30 internal, 0.90 input -> 0.80 internal, 1.00 input -> 1.00 internal.

## 2026-04-26 — KS-024 tail sustain

- KS023 passed the listening check for removing the warm/papery output coloration.
- The next issue is held-note decay: notes become too quiet around five seconds even though the attack and early tone are good.
- A compressor could mask this, but the model should first try physically plausible lower late-stage damping.
- KS024 keeps normal early modal decay, then relaxes damping after the first half-second so the remaining core modes hang on longer.
- An 8-second open-string render now shows 6-second RMS around 12-16% of the 0.5-second RMS, rather than collapsing as quickly.

## 2026-04-26 — KS-025 sustain parameter

- The long KS024 sustain is musically useful but should be adjustable by patch or automation.
- `Sustain` is the first real plugin parameter and uses JUCE `AudioProcessorValueTreeState`.
- Defaulting to 100% preserves the last liked sound, while lower values move back toward the natural/shorter tail.
- Parameter smoothing belongs in `AudioEngine` so host automation does not step directly into voice damping.
- Plugin state now stores parameter state instead of only a placeholder XML version node.

## 2026-04-26 — KS-026 pick material controls

- Pick stiffness and pick texture are best treated as note-start excitation controls for now.
- Default 50%/50% should preserve the current pick sound while allowing audition range on either side.
- Stiffness maps to contact sharpness and upper-mode attack energy.
- Texture maps to friction/scrape and tangential/horizontal excitation.
- Since the current voice is modal rather than a full string-contact solver, these controls are an excitation-layer approximation, not yet a physically complete plectrum/string interaction.

## 2026-04-26 — KS-027 pick texture needs an audible contact path

The `Pick Texture` control can feel inactive if it only changes the initial delay-line noise distribution. A short rendered contact/scratch component gives the ear a clearer friction cue, and setting the low end to true zero better matches the idea of a frictionless plectrum. The 50% default remains the anchor point for continuity while the extremes are widened for meaningful auditioning.

## 2026-04-26 — Note-start controls should use target values

Pick stiffness and pick texture are note-start excitation controls, so new voices should use the current parameter target rather than the smoothed current value. Smoothing is still useful for continuous controls, but for excitation parameters it can make the UI feel unresponsive or stale if a note starts before the smoother reaches the target.

## 2026-04-26 — Texture needs contact regimes, not just gain

Manual testing of KS-028 confirmed that `Pick Texture` was audible, but the top end sounded like louder snap or a small snare-like noise burst. For pick-material realism, the useful control shape is likely regime-based: smooth plastic-pick snap around the middle, then a separate stick-slip/grind behavior above the middle rather than scaling the same broadband component.

## 2026-04-26 — Reserve parameter extremes for special materials

KS-029 made the full texture range usable, but the user wanted an additional Brian May/coin-pick extreme. Compressing the normal range to end around 80% leaves room for a deliberately nonstandard material model from 80% to 100% without damaging the regular plastic-pick control range.

## 2026-04-26 — Extreme contact should choke, not only excite

KS-030's coin zone was useful up to about 95%, but the last few percent became chirpy and bursty. A more plausible extreme-contact model should reduce fast release impulses and partially choke the free string while injecting darker friction energy. For rough materials, "more contact" is not necessarily "more bright transient."

## 2026-04-26 — Choking can erase texture

KS-031 proved that too much choke in the final texture range can read as rolling the texture off, even when a darker friction layer is present. For the coin extreme, preserving the useful 95% coin layer and adding a dense ridged-drag component is a better last experiment than replacing excitation with damping.

## 2026-04-26 — Palm mute needs to affect active voices

Palm muting is a performance control, not only a note-start articulation. The first simple model passes the smoothed mute amount into `StringVoice::renderSample`, so automation or slider movement can damp strings that are already ringing. This is a better fit than sampling mute amount only at note start.

## 2026-04-26 — Palm mute needs more light-range resolution

The first palm mute extremes were good, but the useful light range was too compressed near the bottom of the slider. Mapping the visible 0-50% range onto the previous 0-25% mute amount gives more control over barely-touching palm positions while preserving the full mute at 100%.

## 2026-04-26 — Separate setup intonation from performance pressure

Bridge/saddle intonation and fretting overpressure both change pitch, but they should remain separate controls. Bridge intonation is a stable instrument setup error; fret pressure is a player/performance error that only affects fretted notes and should probably become continuous later if we want to model squeezing a note sharp after attack.

## 2026-04-26 — Hover help is too hidden for plugin parameters

Tooltips technically work, but they are a poor fit for controls whose ranges need musical explanation. Clickable info popovers are more discoverable, easier to read, and should scale better as the parameter set grows.

## 2026-04-26 — Anticipatory noises require a playback mode

Finger approach and release noises can happen before the musical note boundary, so they need either explicit pre-note MIDI authoring or internal lookahead. Keeping lookahead defaulted to Off preserves live playability, while 150/250 ms modes can serve rendered playback where host latency compensation can align the delayed note.

## 2026-04-26 — Finger vibrato can be global before MPE

MPE will eventually provide per-note expression, but a global CC1 vibrato layer is still useful now. Treating mod wheel as an additive amount on top of baseline speed/depth keeps the sliders musically meaningful while allowing performance automation to intensify vibrato.
