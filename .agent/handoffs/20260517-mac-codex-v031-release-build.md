# Handoff - Mac Codex v0.3.1 Release Build

Date: 2026-05-17
From: Windows Codex
To: MacBook Codex (`macbook-pro-m5`)

## Goal

Build the macOS VST3 asset for Guitar AG `v0.3.1` and attach it to the GitHub release.

## Starting State

- Branch: `main`
- Intended tag: `v0.3.1`
- Release title: `Guitar AG v0.3.1 - Fretboard Assignment and Diagnostics`
- Release notes: `dist/RELEASE_NOTES-v0.3.1.md`
- Windows asset produced on the PC as `dist/GuitarAG-v0.3.1-Windows-vst3.zip`
- GitHub CLI auth on the Windows PC was invalid, so Mac Codex may need to create/finalize the GitHub release if it has working auth.

## Mac Build Steps

1. Pull `main` and tags from GitHub.
2. Confirm `CMakeLists.txt` says `project(GuitarAG VERSION 0.3.1 LANGUAGES C CXX)`.
3. Build the macOS VST3 release target.
4. Package with:

```sh
mkdir -p dist
ditto -c -k --sequesterRsrc --keepParent \
  "build/GuitarAG_artefacts/Release/VST3/Guitar AG.vst3" \
  "dist/GuitarAG-v0.3.1-macOS-vst3.zip"
```

5. Attach `dist/GuitarAG-v0.3.1-macOS-vst3.zip` to the `v0.3.1` GitHub release.

## Verification

- Fresh macOS plugin instance shows `v0.3.1 / StringVoice EG-088 MixedMpeChordAssignment`.
- The macOS zip contains `Guitar AG.vst3` as the top-level bundle.
- The GitHub release has both Windows and macOS VST3 zip assets before being considered complete.
