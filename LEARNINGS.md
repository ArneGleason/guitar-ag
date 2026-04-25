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
