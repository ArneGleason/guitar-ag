# Learnings

Use this file as the running memory for the project. Add concise notes when testing, debugging, researching, or making discoveries.

## 2026-08-27 — Current `StringVoice` is modal, not a running waveguide

- The current `delayLine` and `secondaryDelayLine` buffers are filled during `start()` but are not advanced or read by the render path.
- Audible output comes from the modal bank plus explicit pick/contact layers. The contact layer includes tonal rings, chirp modes, impact sinusoids, and periodic scrape components that directly explain the synthetic chirp/specular-noise complaint.
- Commit `3457200` had a functioning two-polarization delay loop and finite-aperture pickup readout. It is a useful project-owned basis for an experimental stateful engine, but it should not be restored as a blind revert because the modal pivot was preferred in the earlier listening context.

## 2026-08-27 — Articulation realism requires persistent string state

- `AudioEngine` can infer a same-string legato source, but it releases that source and calls `StringVoice::start()`, which resets vibration and contact state.
- A convincing hammer-on, pull-off, slide, or repick should change the boundary/contact/excitation of one continuing string rather than start an unrelated note plus an auxiliary noise event.
- MIDI note lifetime and physical finger/string lifetime are different. A repeated fretted note may keep the left-hand finger down while only the right hand acts again.

## 2026-08-27 — Windows baseline versus Guitar-TECHS DI

- The current six-note calibration render showed roughly 4–8 times the zero-crossing rate of matched Guitar-TECHS P1 DI examples while also being substantially more periodic/harmonic and less spectrally evolving.
- That combination is consistent with a narrow chirpy high-frequency layer over an overly clean/static modal core.
- Whole-note analyzer scores remain supporting evidence only. Earlier Guitar AG experiments proved that metric improvement does not reliably predict human listening preference.

## 2026-08-27 — Recent picking research changes the control mapping

- Robotic measurements at 192-micrometre pick-depth increments found nonlinear threshold/shelf changes in loudness, harmonic balance, noisiness, centroid, and flux.
- Pick depth should govern a contact/release event and initial string state. It should not be a linear alias for more brightness or more noise.
- Magnetic pickup response should shape the resulting string transient; raw contact foley should not bypass the string/pickup path.

## 2026-08-27 — Force-wave pickup readout avoids double differentiation

- In the Plan 0090 prototype, the delay state is excited directly by a compliant contact force. Treating that state as displacement and differentiating again at the pickup produced an extremely bright attack, weak body, and rapid apparent decay.
- Treating the traveling-wave state as force/velocity, then applying finite pickup aperture, temporal low-pass response, and DC blocking, produced more plausible attack-to-sustain and zero-crossing behaviour.
- This choice must remain explicit as the model evolves; mixing displacement, velocity, and force interpretations without an impedance-aware conversion can create convincing-looking code with the wrong spectral slope.

## 2026-08-27 — Residual-state repicks can be isolated deterministically

- The Plan 0090 offline `--stateful-repick` switch creates a controlled experiment: the preserving and reset renders are bit-identical before the repeated-note section and diverge only when the same physical string is restruck.
- The focused stateful render is byte-identical across repeated runs. Persistent state does not require nondeterminism.
- Peak-matched stateful audio is currently about 9 dB less dense in whole-file mean level than legacy. Human listening should decide whether this reads as healthier dynamics or an underpowered note body.

## 2026-08-27 — Experimental DSP should not tax the released plugin

- The first integration instantiated experimental delay buffers in the VST3 even though the engine was not selectable. That would have increased plugin memory and reset work without user benefit.
- A target-specific compile definition now includes the stateful engine only in `GuitarAGOfflineRender`; the VST3 retains the original `AudioEngine` storage and behavior.

## Initial Learnings

- MPE is the required control model, not MPC.
- The first success milestone is independent per-note bending, not perfect guitar realism.
- The plugin should output a DI-style electric guitar tone and rely on external amp sims for the MVP.
- Avoid building amp/cab simulation in the first pass.
- Start with a simple string model and improve realism later.
- Keep MPE behavior easy to debug with visible voice/channel state if possible.

## Testing Notes

Add notes here when something sounds good, bad, unstable, too bright, too dull, too synthetic, too clicky, or too CPU-heavy.

## 2026-05-17 — DAW octave names versus MIDI note numbers

- Bitwig audition exposed an octave-name mismatch: the note the human sees as E1 can be MIDI note 40, which is physically/scientifically E2 and the guitar low E. If the human authors visible E2 in that convention, the plugin receives MIDI note 52 unless an input-octave correction is applied.
- This correction must happen before guitar interpretation, not as an audio pitch shift, because fretboard assignment, note-off matching, key/poly aftertouch, and strum grouping all depend on the same note number.

## 2026-05-17 — Legato articulation can masquerade as string allocation failure

- A DAW-label E2+B2 block chord correctly maps to native guitar E2+B2 after the input-octave correction, but high `Legato Articulation` can still collapse it if the second note treats the first note as a same-string hammer-on/tap source.
- Offline rendering confirmed the symptom: the E2+B2 fixture reached max 2 active string voices at default settings, but collapsed to max 1 active voice with `Legato Articulation = 100%` before the guard.

## 2026-05-17 — Stop/rewind can leave stale string occupancy without panic handling

- Bitwig `Neck Slide` auditioning exposed a deeper reset-path issue: if a host stops or rewinds without ordinary note-offs, stale fretboard occupancy can survive and bias later string assignment.
- MIDI CC120 All Sound Off and CC123-127 All Notes Off should be treated as panic/cleanup messages, not ignored as unrelated controllers.

## 2026-05-17 — Debug controls need to exist as plugin parameters

- Bitwig can expose host automation lanes for plugin parameters more conveniently than authoring raw MIDI CC panic messages inside an instrument clip.
- A dedicated `Panic Reset` parameter gives the human a reliable way to draw a reset pulse exactly at the point where assignment state goes bad.

## 2026-05-17 — String-assignment bugs need mapper and voice telemetry

- Recovering with `Panic Reset` proves cleanup works, but does not identify which prior chord or controller state poisoned assignment.
- A useful handoff log needs both host-visible note numbers and engine note numbers after input-octave correction.
- The live UI should show mapper occupancy separately from active voices; a string can be "unavailable" to the mapper even when no audio voice is visibly ringing.

## 2026-05-17 — Windows build identity probe

- On the Windows PC, CMake's Git build-identity probe can return `unknown` if Git rejects the checkout as a dubious-ownership repository. Invoking Git with an explicit `safe.directory` for the source directory keeps the editor identity line tied to the actual commit.
- The build identity dirty check should ignore untracked files so local build folders do not make normal PC deployment builds appear dirty.

## 2026-05-09 — Fret Steps control feel

- EG-063 proved that fret engagement during `Neck Slide` is musically useful, but the audible effect was concentrated near the top of the `Fret Steps` slider.
- EG-064's remap makes the useful fret-engagement range available across the slider breadth and is accepted as the better control feel.

## 2026-05-10 — Block chords versus right-hand articulation

- MIDI clips that stagger chord notes are effectively hand-authoring the right-hand stroke, while block chords leave that interpretation to the instrument.
- The useful Auto Strum first slice is exact-same-sample chord grouping: it covers normal DAW block chords without adding latency or a collection window.
- Near-simultaneous note jitter from hosts may still need a future tolerance window, but that choice should be explicit because it can trade live immediacy for interpretation accuracy.
- Player Feel should see internally generated strum delays as the actual picked event times, otherwise same-sample block chords look like zero-time unrelated picking decisions.

## 2026-05-10 — Strum direction energy balance

- EG-079 Auto Strum landed well in DAW audition, but downstrokes and upstrokes need a simple directional energy balance.
- Scaling down one stroke direction is more predictable than boosting the other because authored MIDI velocities may already be near 127.
- A balanced center is the safest default for continuity, while a small positive `Strum Balance` can create the common stronger-downstroke feel by reducing generated upstroke velocities.

## 2026-05-10 — Release-candidate articulation defaults

- A light Auto Strum default is better for first audition than making users discover `Strum Speed` before block chords feel guitar-like.
- `Player Feel` at 50% has become the practical "pro" default rather than an effect to opt into.
- Grouping Articulation controls by parent action helps the page read as a playable instrument surface instead of a flat parameter list.

## 2026-05-10 — Windows release build lambda captures

- The v0.3.0 Windows VST3 build found an MSVC-only compile failure in editor layout lambdas that referenced local marker-width constants without capturing them.
- Capture local layout constants explicitly in lambdas instead of relying on compiler-tolerated surrounding context.
- The v0.3.0 Windows release asset was built with this small source-compatible fix applied before packaging.

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

## 2026-04-26 — Whammy bend should not be perfectly parallel by default

The relevant tremolo-arm physics are string-tension changes, not a clean digital pitch shifter. A PLOS One paper by Jonathan Kemp on Fender-style tremolo systems supports the important modeling intuition: different strings can produce different pitch intervals for the same bridge motion, especially across wound and unwound string construction. For this pass, a single `Whammy String Spread` control is the smallest useful approximation.

## 2026-04-26 — Key aftertouch is useful before MPE

JUCE key/poly aftertouch includes note number and channel, so it can be routed directly to a matching active voice. That gives the instrument a useful pressure-to-bend gesture now without solving MPE channel allocation yet. Channel pressure is different and should stay unmapped until the MPE/member-channel behavior is designed.

## 2026-04-26 — MPE bend range needs to be visible

MPE pitch expression is only intuitive when the DAW and instrument agree on bend range. Bitwig's default of 48 semitones is a practical plugin default for this project, because it makes DAW-side note-expression pitch curves line up without trial-and-error. Smaller guitar-like ranges can still be selected when desired.

## 2026-04-27 — Accordion sections do not scale for plugin UI

Collapsible sections kept early controls organized, but the full editor still became too tall once setup, pickup, performance, vibrato, MPE, whammy, and articulation controls all existed. A tabbed page model is a better fit for DAW plugin ergonomics because it keeps the window compact while preserving discoverability.

## 2026-05-02 — Separate pitch pressure from expression pressure

Key/poly aftertouch is useful as a pressure-to-pitch-bend gesture, but MPE channel pressure should not automatically reuse that same bend mapping. For the first MPE expression pass, channel pressure works better as a tone/sustain/intensity lane, while key/poly aftertouch keeps the explicit `Aftertouch Bend` behavior.

## 2026-05-02 — Player interpretation belongs above voice synthesis

Hammer-ons, pull-offs, and tapping should not be bolted onto raw MIDI note-on handling as random alternate attacks. They need a phrase-aware layer that tracks likely string/fret/hand positions, then decides whether a transition is picked, left-hand legato, or right-hand tapping before the voice is excited.

## 2026-05-02 — Audition MIDI should be scripted

A feature-audition MIDI file is useful for quick DAW checks, but it should be generated from a script rather than hand-edited as an opaque binary. Plain MIDI can exercise notes, velocity, channels, pitch bend, pressure, CC1, and CC74, but it cannot directly set arbitrary VST parameters such as pickup model, palm mute, or harmonic touch without host-specific automation or MIDI mapping.

## 2026-05-02 — Legato energy should remain below picked energy

The first player-articulation pass makes hammer-ons, pull-offs, and right-hand taps separate excitation profiles rather than full picked notes with different names. Keeping their DI energy below picked attacks is important: high-gain amp chains can compress the difference later, but the clean instrument should still expose the weaker left-hand energy and the grabbier pull-off shape.

## 2026-05-02 — Pickup changes can be excitation changes

The pickup-position path did not change in the player-articulation pass. The perceived pickup difference came from feeding the same pickup model different harmonic and attack content. This is a useful reminder that pickup auditioning should compare both fixed excitation and realistic playing gestures.

## 2026-05-02 — Feedback needs explicit headroom checks

The first `Amp Feedback` implementation made the broad arpeggio audition safe, but a longer single-note render at 100% exposed too much high-end sustain energy. Long held notes are the right stress test for this feature; the top of the slider should be tuned against peak/RMS diagnostics as well as listening.

## 2026-05-02 — Feedback needs a winner

Listening feedback on EG-049 showed that per-voice harmonic sustain sounds like even excitation, not amp feedback. A more convincing effect needs a separate loop with memory, band competition, and a dominant frequency that pushes back into matching string modes. The first EG-050 diagnostic renders show the long-note tail concentrating around upper feedback bands instead of merely raising the whole modal bank.

## 2026-05-03 — Pickup microphonics should not count as strings

The six-string cap is about physical string voices. If Guitar AG later models pickup microphonics, body taps, or handling thumps, those should be a separate auxiliary pickup/body source rather than extra string polyphony. A simple seventh non-string voice or bus could carry pickup/body resonance and mix into the DI path without breaking the six-string allocation rule.

## 2026-05-03 — Neutral-path math dominated the modal voice cost

The passive EG-052 optimization kept renders byte-identical while making common offline renders much faster. The biggest wins came from rendering only configured modal slots, reusing precomputed neutral pitch steps, and skipping vibrato/whammy/MPE/feedback math when those features are exactly inactive. This suggests the next risky quality modes should be optional; there was still plenty of free performance available without changing sound.

## 2026-05-03 — Out-of-phase pickup spacing matters

Subtracting two very nearby readouts behaves more like a differential humbucker coil trick than the familiar two-pickup out-of-phase guitar sound. For a neck/middle-style nasal tone, the phase cancellation should come from two single-coil read points separated by a meaningful distance along the string, with the pickup-position control moving that pair together.

## 2026-05-03 — MPE still needs a global performance lane

Turning on MPE should not make the instrument lose guitar-wide gestures. Member-channel pitch wheel belongs to per-note bends, but lower-zone channel 1 can remain a master/global pitch-wheel lane for whammy. This keeps independent note expression and tremolo-arm behavior from fighting over the same MIDI messages.

## 2026-05-03 — Info copy should have two layers

The in-plugin help is more useful when the first sentence says what a control does in plain language, then a short technical note explains the model behavior. That keeps the interface friendly while rewarding users who want to understand the physical-model assumptions.

## 2026-05-03 — Feedback needs a string winner too

A dominant feedback band is not enough by itself if every active string with a compatible harmonic gets reinforced. The loop needs a physical-string focus stage so the high-feedback behavior feels like one string/harmonic taking over, with possible handoffs, rather than a global sustain boost across the whole guitar.

## 2026-05-03 — Distorting only the feedback return preserves clean DI scope

A clipped amp signal can be useful as the excitation source for feedback, but the plugin still should not become an amp simulator. Clipping only the internal feedback return adds harmonic material to the loop while leaving the main output available for external amp/cab sims.

## 2026-05-03 — Feedback should bloom after the guitar attack

The feedback loop sounded more realistic when the user manually kept `Amp Feedback` near zero during the note attack and raised it during the sustain. A convincing feedback model needs onset behavior, not just a steady-state resonance. Letting the picked attack win first gives the loop a cleaner musical target and avoids the "all strings get excited immediately" impression.

## 2026-05-03 — Clean feedback return exposes band-hunt chirp

The clean feedback return can make the loop's early movement across resonant bands too audible as a chirp. The clipped return sounds more natural by feeding the resonator bank a denser, more amp-like signal while leaving the main DI path clean.

## 2026-05-09 — Guitar slide authoring needs two lanes

Single-note slides can use existing MPE per-note pitch bend, but chord slides are awkward if the human must draw identical per-note curves. A shared automatable slide-offset lane would let a held chord move as one guitar shape, while MPE pitch bend remains available for independent note detail.

## 2026-05-09 — MPE Slide is not automatically guitar slide

Some DAWs label the MPE Y dimension as Slide, but Guitar AG currently maps CC74/timbre as timbre. The planned guitar-slide feature should avoid overloading CC74 by default and treat fret slides as pitch-motion plus physical contact/noise behavior.

## 2026-05-09 — Slide release should not depend on note-off

Human Bitwig audition of EG-065 showed that note-off-driven slide tails are a weak fit for DAW loop playback and do not model the physical gesture well. Sliding off should be an explicit pressure-lift envelope during slide motion, not an inferred release color at the note boundary.

## 2026-05-09 — Finger noise needs pulse and surface structure

Finger/string movement stops sounding guitar-like when it is mostly filtered random noise. A better first approximation is intermittent friction pulses exciting a small resonant squeak, with wound strings getting stronger ridged texture and plain strings staying smoother. The broad scrape component should support the squeak, not dominate it.

## 2026-05-10 — Slide squeak must follow motion

A fixed-pitch squeak carrier under a random scrape floor still reads as hiss in a spectrum and by ear. Slide noise needs an explicit movement envelope: faster `Neck Slide` changes should raise pulse density and amplitude, while stopped movement should decay quickly toward silence.

The perceptual balance of slide squeak can differ by direction. Upward throws and downward returns may need separate gains even when they share the same underlying motion-coupled contact model.

For presets, host state recall and in-plugin preset browsing are related but not the same thing. The current JUCE/APVTS state chunk is enough for DAW sessions; a useful Guitar AG preset system should add named files around that parameter state so stock and user sounds can be curated without changing DSP code.

## 2026-05-10 — Single-note pick attack still exposes synthetic contact

Chord/strum examples can sound convincing because staggered note starts and different note/string seeds smear the attack into a plausible gesture. Single-note lines expose the current contact model more nakedly: the attack can sound like a tonal digital chirp rather than a plectrum contacting and releasing a string.

The current note-start random seed uses only MIDI note and channel, so repeated same-note attacks can reuse the same transient fingerprint. A better pick pass should include deterministic per-attack variation, pick direction, and a more physical contact/release envelope before adding many pick material choices.

## 2026-05-10 — Slide noise and note finger noise need separate balances

The motion-coupled slide squeak is now musically useful, but the older note approach/release `Finger Noise` layer is still not at the same realism level. Keeping them on separate controls lets slide gestures be balanced immediately without overcommitting to the older noise model.

## 2026-05-10 — Performance controls need primary/character hierarchy

As the instrument gains expressive controls, a flat Performance page hides important controls by making every tweak look equally primary. The usable taxonomy is gesture first, then character: keep the thing a player automates most visible, and tuck shaping details beneath it without moving the slider column.

## 2026-05-10 — Pick attack variation must be deterministic but not cloned

Repeated same-note attacks need small contact/release differences, but offline rendering and reviewer checks still need repeatability. Seeding the voice from note, channel, assigned string/fret, render sample time, stroke direction, gesture, and a resettable attack counter gives varied attacks inside a phrase while rendering the same MIDI and parameters byte-identically.

## 2026-05-10 — The pickup hears string motion, not a pick-noise bus

When an attack sounds like a brittle object, the tempting fix is a "pick noise volume" slider, but the physical model should first ask which string motions caused that sound. EG-073 treats the control as `Pick Bite`: how much edge contact, scrape, coherent contact ring, and chirp are imprinted into the string at release. This keeps the mental model aligned with the pickup capturing string motion rather than mixing an independent foley layer.

String age should influence picked attacks because the pick is exciting the same physical string. It should not simply lower every attack component; aged strings can have more surface friction, but the high-Q bright contact ring and chirp should dull faster than the body of the note.

## 2026-05-10 — Alternate picking is a right-hand path, not only a toggle

The first pick-stroke implementation treated alternate as a simple down/up flip. Human audition clarified the player-interpretation layer: repeated notes on the same string should alternate, but cross-string motion should follow the efficient pick path. Since Guitar AG already has a fretboard string assignment, the right hand can use that physical string index to infer sweep direction for adjacent and skipped strings.

## 2026-05-10 — Humanization needs a physical cause

Random timing and velocity offsets can make MIDI less grid-perfect, but they do not create the sense of a person solving a musical task. A better first model is load-based: cognitive demand, dexterity demand, and endurance rise when the part is difficult and recover during easier passages or rests. Deterministic noise is still useful, but only as a texture scaled by those causes.

## 2026-05-10 — Player Feel needs headroom and visibility

Human DAW audition found EG-075 `Player Feel = 100%` musically natural rather than excessive, with recovery closer to 2 to 2.5 seconds. That means the first scale underused the control range. EG-076 makes the previous natural maximum land around 50%, keeps the top half available for sloppier playing, and adds visible cognitive/dexterity/endurance meters so the load model can be tuned by watching it breathe while listening.

Audition settings should be easy to pass back through the Studio Pattern. A copyable JSON snapshot from the plugin is more reliable than describing slider positions by memory, especially as the UI grows.

## 2026-05-10 — Load meters need headroom too

EG-076 proved the cognitive/dexterity/endurance gauges are useful, but meters that hit 100% too quickly stop teaching the human what the model is doing. For EG-077, the load impulses are intentionally smaller while the `Player Feel` top half gets a larger timing/energy scale. That separates "how hard is the passage" from "how much should we exaggerate the player's imprecision."

The first exported audition setting worth keeping is stored at `docs/audition-settings/20260510-eg076-pick-tone-reference.json`. It has high `String Age`, high `Pick Bite`, bridge-side `Pickup Position`, `Pickup Model = Humbucker`, and moderate `Player Feel`; use it as a reference context for future tone/preset discussion, not as a default.

## 2026-05-10 — Strums are one gesture, not many decisions

Already-staggered MIDI strums can fool a note-by-note feel model: each string arrival looks like another fast event, so cognitive and dexterity meters climb even though the player may simply be sweeping through a held chord shape. A better mental model separates left hand and right hand: the left hand holds or changes shape, while the right hand performs a stroke across strings. EG-078 starts with load reduction for short cross-string strum continuations; a future Auto Strum layer should generate the inter-string timing itself from simultaneous chord input.

## 2026-05-17 — Drop tuning is a session-level retuning, not a per-note event

Auto drop tuning works best as a persistent state change: the first sub-E2 note lowers string 0 to exactly that pitch, and the mapper stays there for the session. Resetting between notes would cause audible pitch inconsistency mid-phrase. The pattern mirrors how a real guitarist retunes and then plays — not how a pitch shifter works.

Each FretboardMapper instance (main, fingerNoise, playerFeel, previewFretboard copy) tracks its own drop tuning state. The previewFretboard copy in handleAutoStrumGroup inherits the current drop state from `fretboard` at chord-assignment time, which is correct.

## 2026-05-17 — Chord-aware assignment and strum delay are orthogonal

Before this change, `handleAutoStrumGroup` did two things at once: assign strings in pitch-sorted register order AND apply inter-string strum delay. When strumSpeed = 0, the early return prevented the register-ordered assignment from running at all, so non-strum block chords fell through to per-note dispatch.

Separating the concerns (by removing the early return and letting perStringSeconds collapse to 0 at strumAmount=0) gives register-aware string placement to all simultaneous note groups, not only strummed ones. Velocity balance scaling stays guarded by strumAmount > 0 to avoid affecting non-strum chord velocities.

## 2026-05-17 — Register-affinity scoring weight calibration

The register-affinity term (weight 2.5) in scoreCandidate adds at most ~0.63 penalty for maximum mismatch (note at one extreme, string at the other). A typical 4-fret deviation from positionFret contributes 16.0 to score, so the affinity term is subordinate to position memory. This is intentional: it nudges close-call assignments without overriding the dominant position-memory guidance for single-line playing. If audition finds it too weak or strong, adjust the weight from 2.5.

## 2026-05-17 — Partial chords need a group cost, not a huge single-note bias

The first register-affinity pass still mapped a low-context G4+A4 block to high-E fret 3 plus B-string fret 10. Raising the single-note register weight enough to prevent that would overpower normal melodic position memory. A better boundary is to keep the single-note score modest, then run a fixed-size group search for same-sample partial chords so the combined assignment can choose G4 on string 4 fret 8 and A4 on string 5 fret 5.

## 2026-05-17 — Mixed MPE resets can hide chord intent

Bitwig can send pitch wheel, CC74, and channel-pressure resets in the same sample as chord note-ons. Treating mixed same-sample groups as "not a chord" bypassed the chord-aware assignment path, so later E2+B2 dyads could collapse onto low E after position memory drift. The better routing is to process same-sample note-offs and expression messages at that sample, then assign the remaining note-ons as one guitar chord.

On Windows, Bitwig scanned both the system VST3 folder and a per-user VST3 file under `%LOCALAPPDATA%\Programs\Common\VST3`. Keeping duplicate `Guitar AG.vst3` copies makes DAW identity checks ambiguous; use the system-wide VST3 folder as the canonical PC install.

## 2026-08-27 — Stateful storage is not sufficient for guitar identity

Loudness-matched human A/B listening rejected both Plan 0090 stateful variants:
the preserved-state and reset-on-note renders both "just sound like a synth."
The legacy engine remains imperfect—the attack has a spectral chirp and the
note body is somewhat glassy—but it still reads closer to the intended source.

This separates two questions that metrics and state-continuity tests cannot
collapse into one. Persistent delay-line state can be deterministic, stable,
fast, and measurably different on repicks while the underlying excitation,
dispersion/loss, pickup readout, or body response still lacks electric-guitar
identity. Do not build left-hand/fret mechanics on the present stateful voice
or expose it in the VST3. Keep the offline A/B harness and first improve the
isolated picked-note body enough to pass an ear-based guitar-DI gate.

## 2026-08-27 — Diagnose layers at the parameter regime where they fail

At ordinary pick settings, disabling the legacy short chirp-mode bank was barely
audible. The human clarified that objectionable chirp appears when pick depth is
high, stiffness is low, and texture is raised. An ablation at defaults cannot
clear or condemn a nonlinear/extreme control regime; reproduce the reported
gesture first, then remove one component at a time.

The high E4 sounded like a low-register guitar model digitally pitch-shifted
upward. This is different from excessive treble and will not be solved by
`String Age` roll-off. A model that scales nearly the same modal recipe with
fundamental frequency can retain mathematically clean harmonic relationships
while failing to acquire register-specific string identity.

The current global finger noise confirmed the overlay concern. Periodic
stick impulses driving a multi-sine ridge carrier sound like a plastic comb when
isolated. Finger movement needs motion as its cause: speed-driven friction hiss,
less-periodic transverse/bowing energy, and only a restrained depth-dependent
squeak coupled to string/harmonic position.

Residual preparation must be reconstruction-tested. FFmpeg `amix` did not honor
a negative weight as subtraction in this workflow; it summed the inputs and made
the supposed residual 6 dB louder. Explicit channel subtraction produced the
correct residual, and source plus residual reconstructed the original with
infinite measured PSNR.

The targeted deep-pick test resolved the attack attribution. At 100% bite, 10%
stiffness, and 75% texture, the full attack became a sparse woody rattle and the
isolated explicit extras sounded unmistakably like crude digital synthesis.
Removing the short chirp modes still made little difference. Do not spend the
next pass retuning that mode bank or low-passing the residual; replace the
additive pick/contact path with excitation that enters and decays through the
modal string itself.

A state-coupled experiment still needs an audible-difference check before human
review. The first normalized modal impulse was deterministic and structurally
correct but its isolated contribution measured about -76 dB mean, roughly 29 dB
below a useful comparison. Scaling the integrated impulse into the modal body's
amplitude range produced -50 dB at 1.00x and -45 dB at 1.75x without changing
the default output. A physically motivated topology can still be an invalid A/B
if its effect is effectively inaudible.

Human listening preferred the stronger 1.75x modal-force direction but still
wanted a little of the old attack for texture. The important constraint is
hierarchy: modal response carries the note entry; direct texture is quieter and
denser so it does not return as the sparse woody rattle that Plan 0091 rejected.

The high-note "pitch shift without formant correction" analogy is technically
useful. Scaling a fixed envelope over harmonic index moves that envelope in Hz
with the fundamental, just as naive resampling moves vocal formants. Guitar
partials must move with pitch, but pickup electronics, material/contact bands,
and much of the perceived body envelope should remain fixed or move much less.
An absolute-frequency spectral-envelope experiment is a better next test than
simply darkening high notes.

When raising stochastic event density, scale individual impulses down by the
square root of the density ratio before judging timbre. Otherwise "denser" also
means trivially louder and the comparison cannot identify whether continuity or
level helped. Keep a same-mix sparse control because the shared nonlinear output
stage can still alter apparent level after this first-order compensation.

The hybrid listening pass turned an open-ended blend into useful control bounds:
12% dense direct texture reads as medium and 22% as maximum over the 1.75x modal
force. Preserve both endpoints. A single chosen blend would throw away the
human's more valuable calibration of the musical range.

A fixed-Hz spectral-envelope experiment must be level-controlled per register.
The first literal Plan 0094 anchor reduced E4 by roughly 4–14 dB across the three
anchor strengths, even though it preserved low E2. Global A/B loudness matching
cannot correct each note independently and would make the unchanged low note
louder. A smooth audition-only register compensation brought per-note levels to
within about 1 dB, making "less Munchkin-like" separable from "much quieter."

Human listening validated the register/formant premise but exposed why the
first control cannot be the production control. A 35% absolute-frequency anchor
was much better than the current harmonic-number envelope, yet the notes
progressively flattened and lost metal ring/brightness upward. Scale correction
and metal retention are therefore partly independent. In particular, anchoring
decay curvature can shorten upper fixed-Hz partial lifetimes, and attenuating the
same amplitude envelope for main and inharmonic side modes can remove a useful
metal cue. Test those axes separately instead of backing away from the accepted
envelope correction or adding a broadband treble boost.

An ablation axis must be calibrated to audible scale before it consumes a slot.
In Plan 0095, literal 1x restoration of side-mode energy lost to the 35% anchor
measured only -60.6 dB mean during E4. The useful diagnostic is therefore an
explicitly labelled 6x extrapolation, still limited to the existing detuned side
modes by a 650–2500 Hz smooth gate and plain-string weighting. If that character
is useful, listening should calibrate downward; if it sounds artificial, the
result rejects the side-mode hypothesis without altering the accepted main body.

Exact audition commands belong in durable documentation. The first Plan 0094
handoff recorded hashes and parameters but omitted its render command, which
made a later byte-regression check appear to fail when extra `player-feel` and
`finger-noise` overrides were guessed. Recovering the original command showed
the new default-follow path was byte-identical. Preserve commands alongside
hashes so a regression failure distinguishes changed DSP from changed inputs.

A positive factorial corner does not establish that both axes are independently
necessary. Plan 0095 listening called the harmonic-decay plus 6x side-mode
corner "pretty good," but supplied no verdict for decay-only or side-mode-only.
Record the combined success without inventing negative results. A follow-up that
holds harmonic decay fixed and steps restoration through zero and lower factors
will determine whether the side-mode addition is necessary and how much is
enough.

When calibrating a quiet additive detail, match the full mixes by construction
and isolate the detail separately. Plan 0096's 0x/2x/4x/6x full mixes remain
within 0.1 dB whole-file mean and 0.2 dB per note, while the isolated 6x metal
term is -47.8 dB mean. The full mixes therefore test timbre rather than level;
the isolated track, boosted by A/B matching, identifies whether the contribution
itself sounds like metal-string behavior or a synthetic layer.

The restoration calibration reduced a diagnostic exaggeration without losing
the positive result: human listening selected 2x as good, down from the 6x probe.
This is exactly why audible ablations and production values should be separate.
Use enough exaggeration to identify a mechanism, then explicitly calibrate back
to the lowest accepted amount before treating it as part of the model.

A promotion gate should test whether calibrated improvements compose outside the
fixtures that selected them. Plan 0097 stitches ordinary and deep versions of
the open-string register sweep with a compact picked phrase, keeping current and
candidate aligned through identical MIDI and performance settings. The candidate
is about 1 dB louder in every section, so whole-file loudness matching remains
interpretable; an inconsistent section-by-section offset would have required
separate comparisons instead of one stitched verdict.

The consolidated tone passed outside its tuning fixtures. Human listening
confirmed the Plan 0097 candidate across ordinary picking, the original deep
failure regime, and a phrase with repeated notes, string crossings, upper melody,
and polyphony. That is sufficient to justify production implementation, while
still keeping the implementation and installed-plugin checks as a separate
milestone. Preserve the previous tone offline so later regressions can separate
changed code from changed listening preference.

Promoting an audition chain should be an identity-preserving transfer, not one
more tuning pass. EG-089 compiled the accepted mechanisms and constants into the
normal voice, then proved the no-flags renderer against each accepted Plan 0097
section byte for byte. A named `previous` recipe simultaneously reproduced every
old reference. That paired proof is stronger than a fresh subjective A/B because
it establishes both that the approved sound was not altered during promotion and
that future investigations still have an exact before state.

## 2026-08-28 — Capture approval is part of the reference data

A folder of unlabeled takes is not yet a useful modeling reference. The human's
choice of which repetitions are representative, which are flawed, and what was
heard must travel with the WAVs. A resumable manifest makes that judgment
machine-readable without deleting rejected evidence or relying on chat history.

The first-enabled-input rule is a useful small MVP boundary for interface
capture, provided the UI and documentation explicitly tell the human to enable
only the intended Focusrite channel. The file records the device sample rate
without resampling and keeps each take mono/24-bit, making later alignment and
analysis predictable.

Muted and ringing pick takes are complementary interventions, not two terms in
a perfect linear null. Different damping changes string boundaries and player
interaction. Use paired repetitions to estimate distributions and transfer
traits; treat subtraction only as a diagnostic listening residual.

## 2026-08-28 — A recorder needs a stopping rule, not only request files

Individual structured requests make captures analyzable but do not tell the
player what to do next or how much work is enough. A useful human-facing capture
system needs an ordered inventory, visible progress, a short reason for each
condition, and an explicit point to stop for analysis.

The first inventory therefore avoids a full factorial. It begins with one input
noise floor and six low/high E damping comparisons under one medium downstroke
setup. Direction, force, depth, material, and finger gestures remain visible but
deferred. This makes the first handoff about 7 decisions rather than 30 tasks or
hundreds of individual strokes with no feedback loop.

## 2026-08-28 — Writer overflow is not an input-dropout detector

The first real capture's `dropped_audio: false` did not mean the incoming audio
was continuous. Sample inspection found several hard discontinuities and about
8 ms of exact zeros in the WAV. The current flag reports only failure to enqueue
audio into JUCE's threaded disk writer; a device, driver, or callback-layer
dropout can arrive as apparently valid samples and remain unreported.

For Windows Focusrite diagnosis, verify the recorded waveform as well as the
writer flag, close competing microphone/ASIO clients, and prefer the native
Focusrite ASIO device. A future diagnostics pass should add callback timing or
sample-discontinuity evidence rather than overloading the disk-writer warning.

## 2026-08-28 — Capture review needs an honest amplitude picture

A waveform preview intended for gain calibration should not normalize every
take to the panel height. Drawing the full file at absolute digital full scale
makes low-level recordings remain small, puts the -12 dBFS target in context,
and makes a near-zero-headroom take visibly approach the edges. Peak, RMS, and
duration beside the waveform remove guesswork that shape alone cannot answer.

Fast deletion is compatible with durable human judgment when Reject and Delete
remain separate. The rejected state is a reversible annotation; a second,
explicit Delete/Backspace action erases only that rejected session child. Edge
detection is also important: polling the physical key state must turn a held key
into one action rather than cascading through the remaining take list.
