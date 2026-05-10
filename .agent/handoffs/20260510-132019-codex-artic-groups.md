# Studio Handoff: Codex Articulation Groups and Release-Candidate Defaults

- Timestamp: 2026-05-10T13:20:19-04:00
- Status: handoff
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review EG-081 ArticGroups release-candidate polish
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Pull Instructions

Please run:

```bash
git fetch origin
git switch main
git pull --ff-only origin main
```

Then review the latest pushed `main` commit for EG-081.

## Human Context

Human DAW audition found the strum/player-feel work good enough to prepare as a release candidate, without bumping the product version yet. The requested final polish is an Articulation-page taxonomy pass plus new audition-ready defaults for fresh plugin instances.

## Implementation Summary

- Changed model label to `StringVoice EG-081 ArticGroups`.
- Reorganized the Articulation page into parent/subordinate groups:
  - `Legato Artic` remains standalone.
  - `Pick Bite` is now the primary pick-character control.
  - `Pick Stiffness` and `Pick Texture` are grouped under `Pick Bite`.
  - `Pick Stroke` is now the primary right-hand direction control.
  - `Strum Speed` and `Strum Balance` are grouped under `Pick Stroke`.
  - `Player Feel` stays as the primary feel control with Bot/Pro/Loose landmarks.
  - `Feel Recovery`, `Reset Feel`, and the cognitive/dexterity/endurance meters are grouped under `Player Feel`.
  - `Palm Mute` and `Harmonic Touch` remain standalone.
- Left the global header `Export Settings` flow unchanged.
- Updated new-instance/APVTS defaults:
  - `Pick Texture`: `0.25`;
  - `Strum Speed`: `0.10`;
  - `Strum Balance`: `-0.13`;
  - `Player Feel`: `0.50`.
- Updated `AudioEngine` fallback defaults and offline renderer defaults to match.
- Updated changelog, decisions, learnings, DSP notes, test plan, audition notes, build notes, review queue, project log, and session state.

## Review Focus

- Confirm the Articulation page hierarchy is readable and no labels/sliders/info/disclosure buttons clip.
- Confirm the disclosure groups behave like the existing Performance-page groups.
- Confirm Export Settings remains in the header and was not moved or changed.
- Confirm the new defaults are consistently wired in APVTS, `AudioEngine`, `processBlock` fallbacks, and `tools/render-calibration.cpp`.
- Confirm existing saved sessions should keep their stored parameter values rather than being forced to the new defaults.
- Confirm `Strum Balance` behavior from EG-080 remains intact.

## Known Limitations

- The product version remains `0.2.6`; no 3.0/version bump was made in this pass.
- Auto Strum still only groups exact same-sample block chords. Near-time chord collection remains deferred.
- Stock/user preset import is still deferred to the preset-management pass.

## Checks Run

- `git diff --check`.
- Built `GuitarAGOfflineRender`.
- Built `GuitarAG_VST3`.
- Rendered Auto Strum audition with implicit offline defaults.
- Rendered the same audition with explicit `--pick-texture 0.25 --strum-speed 0.10 --strum-balance -0.13 --player-feel 0.5`.
- Confirmed the implicit and explicit default renders were byte-identical.
- Installed the VST3 with `scripts/install-vst3.sh --build`.
