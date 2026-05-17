# Studio Handoff: PC Build and Deployment Engineer for Plan 0082

- Timestamp: 2026-05-17T10:32:08-04:00
- Status: ready-for-windows-build-and-bitwig-audition
- Branch: `feature/0082-fretboard-drop-tuning-register-affinity`
- Plan 0082 implementation commit: `b4630a3`
- From: `Codex`
- From machine: `macbook-pro-m5`
- To: `Codex on the human's Windows PC`
- To role: `PC build and deployment engineer`
- To machine: human Windows PC; canonical machine handle not yet formalized
- Subject: Build and deploy the Plan 0082 Windows VST3, then help the human audition it in the active PC project

## Role Definition

You are not acting as the architect/reviewer for this turn. Your job is practical build and deployment work on Windows:

- get the branch onto the PC;
- configure the JUCE/CMake/Visual Studio build;
- build the Windows VST3 and offline renderer;
- install/copy the VST3 where the DAW can scan it;
- help the human confirm Bitwig loads the correct build;
- only make code changes if the Windows build itself fails and the fix is narrow.

If you do need to patch for MSVC, keep it minimal, build again, document exactly what changed, commit, and push.

## Context

Plan 0082 improves fretboard heuristics rather than DSP synthesis:

- auto drop tuning: sub-E2 notes lower string 0 to the exact requested MIDI note until reset;
- register-affinity scoring: low notes prefer lower strings and high notes prefer higher strings in close calls;
- non-strum block chords: same-sample chords use chord-aware string assignment even when `Strum Speed = 0%`;
- partial same-sample chords smaller than six notes now use a fixed-size group search so dyads avoid greedy fret spreads.

Mac Codex already built and installed this branch on macOS. It passed mapper probes and offline renders. The PC task is to prove the Windows build/deployment path and let the human audition inside the current PC project.

## First Steps on the PC

Start by getting the exact branch:

```powershell
git clone https://github.com/ArneGleason/guitar-ag.git
cd guitar-ag
git fetch origin --prune
git switch feature/0082-fretboard-drop-tuning-register-affinity
git pull --ff-only
git status --short --branch
git log --oneline -3
```

Expected implementation commit in the recent history:

```text
b4630a3 Update session for fretboard handoff
```

The branch may also contain a later handoff-only commit above `b4630a3`.

If the repo already exists on the PC, do not delete local files. Fetch, inspect `git status --short --branch`, and only pull/switch if the worktree is clean or the human confirms what to do with local changes.

## Prerequisites

Use:

- Visual Studio 2022 or Visual Studio Build Tools with MSVC C++ x64 tools;
- CMake 3.22 or newer;
- Git;
- a local JUCE checkout.

If JUCE is not already present, a normal location is:

```powershell
mkdir C:\code
git clone https://github.com/juce-framework/JUCE.git C:\code\JUCE
```

If the PC uses a different JUCE path, substitute it in the `-DJUCE_PATH=...` configure command.

## Build Commands

Configure for Visual Studio x64:

```powershell
cmake -S . -B build -DJUCE_PATH=C:\code\JUCE -A x64
```

Build the VST3:

```powershell
cmake --build build --config Release --target GuitarAG_VST3
```

Build the offline renderer too:

```powershell
cmake --build build --config Release --target GuitarAGOfflineRender
```

Expected VST3 output:

```text
build\GuitarAG_artefacts\Release\VST3\Guitar AG.vst3
```

Expected offline renderer output:

```text
build\GuitarAGOfflineRender_artefacts\Release\GuitarAGOfflineRender.exe
```

## Quick Offline Smoke

Run one short render to confirm the executable and shared audio engine work on Windows:

```powershell
mkdir build\diagnostics
build\GuitarAGOfflineRender_artefacts\Release\GuitarAGOfflineRender.exe `
  --midi tests\midi\single-note-calibration.mid `
  --output build\diagnostics\pc-0082-single-note-calibration.wav `
  --sample-rate 48000 `
  --block-size 512 `
  --tail-seconds 2.0
```

This smoke does not replace DAW testing. It only confirms the CLI renderer runs and writes audio.

## Deploy the VST3

Typical Windows VST3 location:

```text
C:\Program Files\Common Files\VST3\
```

From an elevated PowerShell, copy the built bundle:

```powershell
Copy-Item -Recurse -Force `
  "build\GuitarAG_artefacts\Release\VST3\Guitar AG.vst3" `
  "C:\Program Files\Common Files\VST3\"
```

If admin permissions are not available, copy it to a custom VST3 scan folder configured in Bitwig, then rescan that folder.

## Bitwig Audition Targets

Confirm Bitwig loads the Windows build, then test in the active PC project.

First confirm the plugin editor identity line shows:

```text
v0.3.0 / StringVoice EG-081 ArticGroups / b4630a3
```

Bitwig can cache duplicate plugin entries. If more than one `Guitar AG` appears, use the editor identity line to pick the correct one.

Focused Plan 0082 audition:

- D2 alone: should sound as actual D2, not E2 transposed by assignment.
- D2 + A2 same-sample/block chord with `Strum Speed = 0%`: should feel like an open Drop D power chord, no generated strum delay.
- D2 + A2 with nonzero `Strum Speed`: should still use the open Drop D grip but fan across strings.
- G4 + A4 after a low-position phrase: should sound like a compact upper-string dyad, not an awkward wide fret spread.
- Ab3 + Bb3 after low-position context: same compact-dyad check in the mid register.
- Existing project material: listen for any regression in Auto Strum timing, Player Feel, MPE bends, pressure, and CC74.

Suggested plugin settings for first pass:

- `MPE Mode`: match the project; enable it for Bitwig note-expression tests.
- `MPE Bend Range`: `48.0 st` if using Bitwig default note-expression pitch range.
- `Strum Speed`: compare `0%`, default `10%`, and a clear value around `40-70%`.
- `Player Feel`: default `50%` unless the project already has a saved setting.

## What to Report Back

Create a short handoff or final status with:

- Windows machine/toolchain summary: Visual Studio/MSVC version if easy to get, CMake version, JUCE path.
- Build result for `GuitarAG_VST3`.
- Build result for `GuitarAGOfflineRender`.
- Offline smoke result and WAV path if run.
- VST3 deployed path.
- Whether Bitwig loaded the correct `b4630a3` build.
- Human listening notes for the focused Plan 0082 audition.
- Any Windows-only warnings, build failures, DAW scan issues, or plugin-cache weirdness.

If everything passes, no architecture review is needed. The next natural step would be either a PR/merge decision for Plan 0082 or packaging a Windows VST3 asset for a release/pre-release.
