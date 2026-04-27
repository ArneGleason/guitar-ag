# Guitar AG

Guitar AG is an experimental **physical-model electric guitar VST3 instrument** built with **C++**, **JUCE**, and **CMake**.

It is not sample based. It synthesizes a clean DI-style electric guitar voice from modeled string behavior, pickup readout, and performance controls. The goal is a lightweight virtual instrument that can sit before a normal amp/cab simulator and respond more like a playable guitar-style synth than a static sample library.

The project is also a practical example of AI-assisted audio plugin development: a human musician/developer gave listening feedback in Bitwig, while Codex iterated on DSP, UI, build tooling, documentation, analysis scripts, and versioned VST3 builds in small testable steps.

## Current Status

The current build is a working macOS VST3 instrument.

Implemented so far:

- Polyphonic modeled string voices.
- Clean DI-style electric guitar tone.
- Standard MIDI note on/off and velocity.
- Fretboard/string assignment heuristic.
- Pickup type and pickup position controls.
- Pick stiffness and pick texture controls.
- Palm mute, harmonic touch, string age, sustain, bridge intonation, and fret pressure controls.
- Lookahead finger-noise mode for rendered playback.
- Finger vibrato controls with optional mod-wheel routing.
- Global pitch-wheel whammy mode.
- Per-note key/poly aftertouch bend.
- First MPE pitch-bend milestone: one held note can bend independently while other notes remain stable, provided the DAW sends notes on separate MPE member channels.

Current model label:

```text
StringVoice EG-046 MPEBend
```

## Demo

![Guitar AG running as a VST3 instrument in Bitwig](assets/demo/guitar-ag-bitwig-screenshot.png)

Short MP3 render from the current modeled guitar voice:

<audio controls src="assets/demo/guitar-ag-demo-2026-04-27.mp3"></audio>

[Download or play the MP3 demo](assets/demo/guitar-ag-demo-2026-04-27.mp3)

## Why This Exists

Many virtual guitar instruments rely on large sample libraries or sound convincing only inside a narrow articulation set. Guitar AG explores a different direction: a small physical model with continuous controls for the in-between behaviors that are hard to cover with samples.

The first major success condition was:

> Hold a chord and bend one note independently with MPE while the other notes stay put.

That milestone is now implemented.

Perfect guitar realism is not claimed. This is a playable research instrument, a useful DI tone source, and a living example of how an AI-assisted development loop can move from idea to working VST.

## Quick Start

### macOS

Prerequisites:

- CMake 3.22 or newer.
- Apple Command Line Tools or Xcode.
- Git.
- A local JUCE checkout.

Example setup:

```bash
git clone https://github.com/juce-framework/JUCE.git ~/code/JUCE
git clone https://github.com/ArneGleason/guitar-ag.git
cd guitar-ag
cmake -S . -B build -DJUCE_PATH=~/code/JUCE -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target GuitarAG_VST3
```

Install to the user VST3 folder:

```bash
scripts/install-vst3.sh --build
```

Installed location:

```text
~/Library/Audio/Plug-Ins/VST3/Guitar AG.vst3
```

### Windows

Windows builds should be made on Windows.

Prerequisites:

- Visual Studio 2022 or Visual Studio Build Tools with MSVC C++ tools.
- CMake.
- Git.
- A local JUCE checkout.

Example PowerShell flow:

```powershell
git clone https://github.com/juce-framework/JUCE.git C:\code\JUCE
git clone https://github.com/ArneGleason/guitar-ag.git
cd guitar-ag
cmake -S . -B build -DJUCE_PATH=C:\code\JUCE
cmake --build build --config Release --target GuitarAG_VST3
```

Typical Windows VST3 install location:

```text
C:\Program Files\Common Files\VST3\
```

Cross-compiling a Windows VST3 from macOS is not the recommended path. JUCE/CMake keeps the source portable, but VST3 binaries are platform-specific and are easiest to build and test on the target OS.

## Releases And Downloads

Compiled plugin binaries should be distributed through **GitHub Releases**, not committed directly into the repository.

Suggested release assets:

- `Guitar-AG-macOS-vX.Y.Z.zip`
- `Guitar-AG-Windows-vX.Y.Z.zip`

At the moment, the repository is primarily source-first. Release packaging can be added next with GitHub Actions so macOS and Windows builds are attached automatically to tagged releases.

## MPE Setup Notes

In the plugin UI:

- Enable `MPE Mode`.
- Leave `MPE Bend Range` at `48.0 st` for Bitwig's default MPE pitch-expression range.

In the DAW:

- Enable MPE/note expression for the instrument track.
- Match the DAW note-expression pitch range to the plugin's `MPE Bend Range`.

If the DAW and plugin ranges disagree, a drawn two-semitone bend will not sound like two semitones. This is why the bend range is visible in the UI.

## Offline Render Tool

The repo includes a command-line renderer that uses the same `AudioEngine` as the plugin. This made it possible to run quick experiments without opening a DAW.

Example:

```bash
cmake --build build --config Release --target GuitarAGOfflineRender

build/GuitarAGOfflineRender_artefacts/Release/GuitarAGOfflineRender \
  --midi tests/midi/single-note-calibration.mid \
  --output build/diagnostics/guitar-ag-test.wav \
  --sample-rate 48000 \
  --block-size 512 \
  --tail-seconds 2.0
```

The offline renderer is useful for DSP comparison and regression checks. It does not replace testing the VST3 in a real host such as Bitwig, Reaper, Ableton Live, or another DAW.

## Development Process

We started with a rough idea: a lightweight physical-model electric guitar VST that could act as a clean DI instrument without samples, then built it in small auditionable steps over about a day of focused iteration. First we made the JUCE/CMake VST3 shell and a simple plucked string, then repeatedly listened, measured, and adjusted the model through pickup behavior, sustain, wound/plain string character, pick stiffness and texture, palm muting, harmonics, string age, intonation, fret pressure, finger noise, vibrato, whammy behavior, aftertouch bend, and finally MPE per-note pitch bend. The process was very "warmer/colder": Codex made a narrow hypothesis and built it, the human tested in Bitwig and gave musical feedback, and the project kept or redirected each experiment based on what actually sounded convincing. By the end, the repo had gone from documentation and an empty shell to a buildable, installed, versioned VST3 with real-time modeled guitar-like sound, useful performance controls, GitHub backup, offline render tooling, and the original core goal working: independent pitch control for notes via MPE.

## By The Numbers

Measured at commit `df112e2`:

- Elapsed time from first commit to MPE pitch-bend milestone: about 27 hours 51 minutes.
- Git commits: 64.
- Tracked files: 99.
- Total tracked text lines: 12,344.
- Code/tooling lines: 6,370.
- Docs/plans lines: 5,884.
- Core plugin source lines: 3,544.
- DSP source lines: 2,093.
- Plugin/UI source lines: 1,451.
- `StringVoice.cpp`: 938 lines.
- VST parameters: 24.
- Collapsible UI sections: 7.
- Info popovers/buttons: about 21.
- Plan files: 53.
- Docs files: 10.
- Analysis/helper scripts: 10.
- Offline render/diagnostic WAVs present during development: 31.
- Named model checkpoints found in docs/build labels: 46.
- Sound/control-related commits by rough keyword search: 49.
- Retune/fix/polish/pivot-style commits by rough keyword search: 23.

## Repo Guide

- `src/dsp/` — audio engine, string voice, pickup/tone stage, fretboard mapper.
- `src/plugin/` — JUCE processor and editor.
- `tools/` — offline render tool.
- `scripts/` — analysis, MIDI generation, reference-data, and install helpers.
- `docs/requirements.md` — product requirements.
- `docs/architecture.md` — component boundaries.
- `docs/mpe-behavior.md` — MPE behavior and routing notes.
- `docs/dsp-notes.md` — DSP experiments and listening notes.
- `docs/realism-vision.md` — longer-term research direction.
- `docs/build-notes.md` — build and install details.
- `plans/` — milestone-by-milestone implementation plans.
- `DECISIONS.md` — accepted decisions.
- `LEARNINGS.md` — accumulated project memory.

## Non-Goals

For now, Guitar AG does not aim to be:

- A sample library.
- A full amp/cab/effects suite.
- A perfect named guitar emulation.
- A finished commercial-grade release.
- A complete MPE implementation for every possible zone/master-channel edge case.

The current aim is simpler and more fun: a compact modeled electric guitar instrument that is playable, inspectable, hackable, and useful enough to keep improving.

## License

No explicit license has been added yet. Treat the code as private/all-rights-reserved until a license file is added.
