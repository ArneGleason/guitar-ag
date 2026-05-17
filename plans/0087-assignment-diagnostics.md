# Plan 0087 — Assignment Diagnostics

## Goal

Make Bitwig string-assignment failures diagnosable from the VST itself.

## Scope

- Add a bounded rolling diagnostics log for incoming MIDI and assignment decisions.
- Expose a copyable JSON snapshot from the editor.
- Add a six-string header strip that shows mapper occupancy and active voice state.
- Keep diagnostics preallocated and non-blocking for the audio thread.

## Verification

- Release VST3 builds on Windows.
- Offline E2+B2 smoke render still reaches two active voices.
- New instances show `StringVoice EG-087 AssignmentDiagnostics`.
- `Copy Log` produces JSON with `strings` and `events` arrays.
