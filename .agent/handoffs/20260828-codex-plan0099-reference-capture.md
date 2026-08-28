# Plan 0099 Reference Capture Handoff

## Outcome

Implemented a separate Windows JUCE companion for interactive guitar-reference
capture. It loads a focused request, records the first enabled interface input
as mono 24-bit WAV, meters headroom, keeps multiple takes, auditions them, and
stores candidate/approved/rejected states plus notes and measurements in a
resumable external `session.json`.

The request generator and session validator complete the agent/human loop. The
validator can emit only approved absolute WAV paths for the next analysis pass.
Captured audio remains external evidence and never becomes VST3 sample playback.

## Verification Completed

- Windows Release `GuitarAGReferenceCapture` build passes.
- Existing Windows Release `GuitarAG_VST3` and `GuitarAGOfflineRender` builds
  still pass after the new CMake target.
- The app launches against a generated request and reports the connected
  Focusrite device in `session.json`.
- A seeded mono 24-bit/48 kHz approved take survives app restart with its status
  and notes intact.
- The session validator emits that approved take's absolute path.
- Python request/session contract tests pass.
- `git diff --check` reports no whitespace errors (only Windows line-ending
  conversion warnings).

## Human Gate

1. Launch `build-vs2022-x64\GuitarAGReferenceCapture_artefacts\Release\Guitar AG Reference Capture.exe`.
2. Select the Focusrite and enable only the instrument input carrying the
   guitar; set 48 or 96 kHz.
3. Load a generated request and record at least two short takes while keeping
   the hardest peaks near or below -12 dBFS.
4. Play both takes, approve one, reject one, add notes, then reopen the same
   request and confirm all states return.
5. Validate the resulting session with
   `scripts/summarize-reference-capture-session.py`.

The first modeling set should use three focused requests with the same
comparison group: normal ringing, non-harmonic fretting-hand damping, and soft
foam/cloth damping near the fretboard. Compare aligned distributions; do not
call their direct difference a pure pick signal.

## Review Focus

- Audio callback/threaded-writer lifetime during record, stop, device changes,
  and shutdown.
- Session-manifest durability and safe resumption when WAVs are missing or the
  request is reopened.
- Real Focusrite mono 24-bit recording and playback behavior.
- Clarity of the first-enabled-input limitation and external-data boundary.
