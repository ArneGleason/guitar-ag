# Build Notes

## Purpose

Use this file to record build tooling, platform requirements, and setup notes once implementation begins.

## Initial Direction

C++ with JUCE and CMake is the chosen initial build approach for the MVP plugin shell.

The repository expects JUCE to be supplied externally rather than vendored into this project. CMake supports either:

- `-DJUCE_PATH=/path/to/JUCE` for a local JUCE source checkout.
- `find_package(JUCE CONFIG REQUIRED)` through `CMAKE_PREFIX_PATH` or another CMake package location.

## Current Environment Check

Checked on 2026-04-25:

- Operating system: macOS.
- Compiler/toolchain: Apple clang 21.0.0 is available from Command Line Tools.
- CMake: 4.3.2 is installed and working.
- Git: Apple Git 2.50.1 is installed.
- Active developer directory: `/Library/Developer/CommandLineTools`.
- Full Xcode: not installed. The CMake setup should avoid requiring `xcodebuild`; use the default Unix Makefiles generator or Ninja if installed.
- JUCE: cloned locally at `/Users/arnegleason/code/JUCE`.
- Homebrew: installed at `/opt/homebrew/bin/brew`, but JUCE is not present in `/opt/homebrew/Cellar`.

The local CMake configure and Release build both complete successfully when configured with `-DJUCE_PATH=/Users/arnegleason/code/JUCE`.

The first JUCE build emits a deprecated `std::wstring_convert` warning from JUCE/VST3 SDK code and a couple of empty-object `ranlib` warnings. The VST3 bundle is still produced successfully.

## Project Build Layout

- Build system: CMake.
- Plugin framework: JUCE.
- Plugin format enabled: VST3.
- Product name: `Guitar AG`.
- Target name: `GuitarAG`.
- Source layout:
  - `src/plugin/` contains the JUCE processor and editor.
  - `src/dsp/` contains the placeholder audio engine and test-tone voice.
- Current audio behavior: MIDI note-on triggers a basic plucked string model; note-off damps/releases it.
- Current non-goals: no string model, no MPE routing, no full UI, no amp/cab simulation.

## Prerequisites

Install:

- CMake 3.22 or newer.
- JUCE, either as a local source checkout or as an installed CMake package.
- A macOS compiler toolchain. Apple Command Line Tools are enough for this CMake setup; install full Xcode only if you specifically want the Xcode generator.

Example local JUCE checkout:

```bash
git clone https://github.com/juce-framework/JUCE.git ~/code/JUCE
```

## Build Commands

Using a local JUCE checkout:

```bash
cmake -S . -B build -DJUCE_PATH=/Users/arnegleason/code/JUCE -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Build and install to the user VST3 folder in one step:

```bash
scripts/install-vst3.sh --build
```

Install the already-built VST3 bundle without rebuilding:

```bash
scripts/install-vst3.sh
```

The script copies:

```text
build/GuitarAG_artefacts/Release/VST3/Guitar AG.vst3
```

to:

```text
~/Library/Audio/Plug-Ins/VST3/Guitar AG.vst3
```

Using an installed JUCE CMake package:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/juce/install -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Current local verification command run:

```bash
cmake -S . -B build -DJUCE_PATH=/Users/arnegleason/code/JUCE -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Result:

```text
Build succeeded.
```

Expected VST3 output:

```text
build/GuitarAG_artefacts/Release/VST3/Guitar AG.vst3
```

For multi-config generators such as Xcode, the configuration directory may vary by the selected build configuration.

## DAW Verification

After building, copy or symlink the VST3 bundle into one of the macOS VST3 scan locations:

```text
~/Library/Audio/Plug-Ins/VST3/
/Library/Audio/Plug-Ins/VST3/
```

Then rescan plugins in a DAW such as Reaper, Bitwig, Ableton Live, or another VST3 host.

Manual checks for this milestone:

- The DAW sees `Guitar AG` as an instrument plugin.
- The plugin opens without crashing.
- The small placeholder editor opens.
- No sound is produced when no MIDI note is held.
- MIDI note-on produces a basic plucked-string tone.
- MIDI note-off releases the tone.

## Bitwig Notes

On 2026-04-25, Bitwig showed two `Guitar AG` entries after replacing the VST3 bundle, and one appeared to load the older sine-test build while the other loaded the newer string build.

The plugin editor now shows version/build identity in the form:

```text
v<plugin-version> / <model-label> / <git-commit>
```

Use that line to confirm which binary Bitwig actually instantiated after rescanning or restarting.
