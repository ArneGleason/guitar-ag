# Plan 0022 — Offline Render Harness

## Goal

Add a command-line render harness so Guitar AG DSP experiments can be rendered from MIDI without opening Bitwig for every iteration.

Human listening in Bitwig remains the important gate for meaningful milestones, but unattended offline renders make it practical to try more radical DSP variations and run analysis quickly.

## Scope

- Add a CMake console target.
- Reuse the shared `AudioEngine`.
- Read a MIDI file.
- Render deterministic stereo WAV output.
- Support sample rate, block size, tail length, and optional output gain.
- Document the workflow.

## Out of Scope

- Do not build a full VST host.
- Do not test plugin scanning or UI behavior.
- Do not replace manual DAW verification.

## Verification

- [x] `GuitarAGOfflineRender` builds.
- [x] The tool renders `tests/midi/single-note-calibration.mid` to WAV.
- [x] The offline KS015 render closely matches the Bitwig KS015 render in waveform shape.
- [x] The workflow is documented.

## Completion Notes

2026-04-25:

- Added `tools/render-calibration.cpp`.
- Added the `GuitarAGOfflineRender` CMake target.
- Rendered `/Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-offline-ks015-dualpickup-3457200-single-note-calibration.wav`.
- Offline and Bitwig KS015 renders had about 0.999 waveform correlation over their overlapping duration.
- Offline output was about 10 dB louder, so the tool supports `--gain` for DAW-level matching.

