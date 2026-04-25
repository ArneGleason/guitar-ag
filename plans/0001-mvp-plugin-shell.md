# Plan 0001 — MVP Plugin Shell

## Goal

Create the smallest useful plugin shell that builds and loads as a VST3.

The goal is not yet to sound like a guitar. The goal is to establish the plugin framework and confirm that the project can build and run in a DAW.

## Scope

- Create the initial plugin project.
- Configure the build system.
- Produce a VST3 build artifact.
- Add a very simple audio output for verification, such as a low-level test tone triggered by MIDI note-on.
- Add a minimal UI or default plugin editor.
- Document build commands in `docs/build-notes.md`.

## Out of Scope

- Do not implement the physical guitar model yet.
- Do not implement full MPE handling yet.
- Do not build an amp/cab/effects section.
- Do not add a complex UI.
- Do not refactor into speculative abstractions before the shell works.

## Implementation Notes

Prefer a simple, standard plugin layout.

Keep the first build as boring and reliable as possible.

If using JUCE, set up the project so future code can separate:

- plugin processor
- audio engine
- MIDI/MPE router
- string voice
- UI

But do not overbuild those modules yet.

## Verification

The human should be able to verify:

- [ ] The project builds without errors.
- [ ] A VST3 plugin file is produced.
- [ ] The plugin loads in at least one DAW.
- [ ] The plugin UI opens.
- [ ] MIDI note-on can trigger a simple test sound.
- [ ] MIDI note-off stops or releases the test sound.
- [ ] Build instructions are recorded in `docs/build-notes.md`.

## Completion Notes

2026-04-25:

- Added a CMake/JUCE VST3 plugin shell named `Guitar AG`.
- Added `src/plugin/` for the JUCE processor and minimal editor.
- Added `src/dsp/` for a placeholder audio engine and fixed-capacity MIDI-triggered sine test voices.
- The shell intentionally does not include the guitar string model, MPE routing, a full UI, or amp/cab simulation.
- Cloned JUCE to `/Users/arnegleason/code/JUCE`.
- Verified CMake configure and Release build with `-DJUCE_PATH=/Users/arnegleason/code/JUCE`.
- The VST3 bundle was produced at `build/GuitarAG_artefacts/Release/VST3/Guitar AG.vst3`.
- Build prerequisites and commands are documented in `docs/build-notes.md`.
