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
