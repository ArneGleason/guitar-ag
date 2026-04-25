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

## 2026-04-25 — CMake build label cache issue

- The visible plugin model label should not be stored as a CMake cache variable.
- CMake preserved an older `GUITAR_AG_MODEL_LABEL` value in `build/CMakeCache.txt`, causing the Bitwig UI label to disagree with source-controlled `CMakeLists.txt`.
- The label is now a normal CMake variable so source changes update `BuildInfo.h` reliably.
