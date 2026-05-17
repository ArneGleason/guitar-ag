# Handoff - Mac Codex v0.3.1 Release Build

Date: 2026-05-17
From: Windows Codex
To: MacBook Codex (`macbook-pro-m5`)

## Goal

Build the macOS VST3 asset for Guitar AG `v0.3.1` and attach it to the GitHub release.

## Starting State

- Branch: `main`
- Release tag: `v0.3.1` at commit `13c93f3`
- Release title: `Guitar AG v0.3.1 - Fretboard Assignment and Diagnostics`
- Release notes: `dist/RELEASE_NOTES-v0.3.1.md`
- Draft GitHub release exists for tag `v0.3.1`.
- Windows asset is uploaded as `GuitarAG-v0.3.1-Windows-vst3.zip`.
- GitHub reports the draft URL as `https://github.com/ArneGleason/guitar-ag/releases/tag/untagged-a5711c18bac7d45567dc` until publication.
- Build the macOS asset from tag `v0.3.1`, not from any later handoff-only commits on `main`.

## Mac Build Steps

1. Pull `main` and tags from GitHub.
2. Confirm `CMakeLists.txt` says `project(GuitarAG VERSION 0.3.1 LANGUAGES C CXX)`.
3. Check out tag `v0.3.1` for the build.
4. Build the macOS VST3 release target.
5. Package with:

```sh
mkdir -p dist
ditto -c -k --sequesterRsrc --keepParent \
  "build/GuitarAG_artefacts/Release/VST3/Guitar AG.vst3" \
  "dist/GuitarAG-v0.3.1-macOS-vst3.zip"
```

6. Attach `dist/GuitarAG-v0.3.1-macOS-vst3.zip` to the draft `v0.3.1` GitHub release.
7. Publish the draft release once both Windows and macOS assets are present.

## Verification

- Fresh macOS plugin instance shows `v0.3.1 / StringVoice EG-088 MixedMpeChordAssignment`.
- The macOS zip contains `Guitar AG.vst3` as the top-level bundle.
- The GitHub release has both Windows and macOS VST3 zip assets before being considered complete.
