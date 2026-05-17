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
- Offline render target: `GuitarAGOfflineRender`.
- Source layout:
  - `src/plugin/` contains the JUCE processor and editor.
  - `src/dsp/` contains the audio engine, string voice, tone stage, and first fretboard mapper.
  - `tools/` contains command-line development tools.
- Current audio behavior: MIDI note-on is assigned to a plausible standard-tuned string/fret location, then triggers a modeled clean-DI electric-string voice; note-off damps/releases it.
- Current player behavior: the phrase layer can interpret eligible transitions as picked notes, hammer-ons, pull-offs, or right-hand taps.
- Current expression behavior: MPE pitch bend, channel pressure, and CC74 are routed by MIDI channel to matching voices when the DAW sends separate member channels. In lower-zone MPE, channel 1 pitch wheel can still drive global whammy.
- Current feedback behavior: `Amp Feedback` uses a note-on bloom, a dominant resonant band, and a dominant physical-string focus. `Distorted Return` defaults on for new instances so the feedback source behaves more like a clipped amp while the main output remains clean DI-style.
- Current non-goals: no full MPE zone/master-channel negotiation, no amp/cab simulation, and no sample playback.

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

Build the offline render tool:

```bash
cmake --build build --config Release --target GuitarAGOfflineRender
```

Render the calibration MIDI through the shared audio engine without opening a DAW:

```bash
build/GuitarAGOfflineRender_artefacts/Release/GuitarAGOfflineRender \
  --midi tests/midi/single-note-calibration.mid \
  --output /Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-offline-<model>-<commit>-single-note-calibration.wav \
  --sample-rate 48000 \
  --block-size 512 \
  --tail-seconds 2.0
```

The offline renderer uses the same `AudioEngine` as the VST3 processor, so it is useful for rapid DSP iteration. It does not test DAW/plugin-host behavior, plugin scanning, or UI behavior.

Optional arguments include `--gain`, `--sustain`, `--pick-stiffness`, `--pick-texture`, `--pick-bite`, `--pick-stroke`, `--strum-speed`, `--strum-balance`, `--player-feel`, `--player-feel-recovery`, `--player-feel-reset-at`, `--palm-mute`, `--harmonic-touch`, `--string-age`, `--bridge-intonation`, `--aftertouch`, `--channel-pressure`, `--cc74`, `--mpe-mode`, `--mpe-bend-range`, `--mpe-pressure-amount`, `--mpe-cc74-amount`, `--pickup-position`, `--pickup-model`, `--legato-articulation`, `--amp-feedback`, `--feedback-return-distorted`, `--slide-squeak-up`, and `--slide-squeak-down`. `--slide-squeak` remains an alias for upward squeak. `--gain` can align the offline WAV level with a DAW export if the DAW project has track/output gain applied.

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

Manual checks for the current VST3 build:

- The DAW sees `Guitar AG` as an instrument plugin.
- The plugin opens without crashing.
- The tabbed editor opens and the header identity line is readable.
- No sound is produced when no MIDI note is held.
- MIDI note-on produces the modeled clean-DI guitar tone.
- MIDI note-off releases the tone.
- New plugin instances should open as `v0.3.0 / StringVoice EG-082 FretboardHeuristics`.
- `Distorted Return` should be enabled by default on new instances.
- High `Amp Feedback` should bloom after picked attacks rather than grabbing the start of the note immediately.
- With MPE enabled in the DAW and plugin, per-note pitch bend, channel pressure, and CC74 should affect only the matching member-channel voice.
- The editor identity line should show the current model label, for example `StringVoice EG-082 FretboardHeuristics`.

## Release Packaging

For a macOS VST3 release asset, build Release first, then package the bundle from:

```text
build/GuitarAG_artefacts/Release/VST3/Guitar AG.vst3
```

Recommended asset name:

```text
GuitarAG-v<version>-macOS-vst3.zip
```

Example:

```bash
mkdir -p dist
ditto -c -k --sequesterRsrc --keepParent \
  "build/GuitarAG_artefacts/Release/VST3/Guitar AG.vst3" \
  "dist/GuitarAG-v0.3.0-macOS-vst3.zip"
```

The GitHub release tag should match the CMake project version, for example `v0.3.0`.

For a Windows VST3 release asset, configure with Visual Studio and the local JUCE checkout, then build the VST3 target:

```powershell
cmake -S . -B build -DJUCE_PATH=C:\code\JUCE -A x64
cmake --build build --config Release --target GuitarAG_VST3
```

Package the bundle from:

```text
build\GuitarAG_artefacts\Release\VST3\Guitar AG.vst3
```

Recommended asset name:

```text
GuitarAG-v<version>-Windows-vst3.zip
```

Windows/MSVC is stricter than the macOS clang release build about lambda captures in editor layout code. Layout helpers that use local constants such as marker widths should capture those constants explicitly.

## Bitwig Notes

On 2026-04-25, Bitwig showed two `Guitar AG` entries after replacing the VST3 bundle, and one appeared to load the older sine-test build while the other loaded the newer string build.

The plugin editor now shows version/build identity in the form:

```text
v<plugin-version> / <model-label> / <git-commit>
```

Use that line to confirm which binary Bitwig actually instantiated after rescanning or restarting.
