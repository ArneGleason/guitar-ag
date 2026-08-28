# Plan 0102 Capture Discard and Waveform Handoff

## Outcome

Added fast, guarded duff-take cleanup and visual level review to the standalone
capture app.

- Delete or Backspace while recording closes the writer and deletes the partial
  WAV without adding a take or manifest entry.
- Delete or Backspace while idle permanently deletes only the selected rejected
  take. Candidate and approved takes report an error and remain intact.
- Saved-take deletion is limited to a `.wav` directly inside the active session
  folder. A filesystem failure leaves the take in the list and manifest.
- Notes, Audio Settings, file choosers, and background applications suppress all
  capture Delete/Backspace behavior.
- The selected take displays its entire waveform at absolute digital full scale,
  with peak, RMS, duration, -12 dBFS guides, and hot/possible-clip coloring.

Implementation commit: `e849c13`.

## Verification Completed

- Windows Release `GuitarAGReferenceCapture` builds successfully with ASIO.
- The three Python capture workflow tests pass.
- Disposable rejected WAV copies were removed independently by Delete and
  Backspace; each action removed exactly one file and manifest row.
- A key held for 180 ms removed only the selected rejected take, not the next
  row selected after deletion.
- Candidate and approved disposable takes survived Delete/Backspace.
- Delete and Backspace each aborted a disposable live ASIO recording after its
  partial WAV appeared; afterward the partial file was absent and the manifest
  take count was unchanged.
- The rebuilt app was rendered and visually inspected at its default size. The
  waveform, absolute-scale guides, measurements, take list, and notes remained
  legible without overlap.
- The final rebuilt executable repeated the reject-then-Delete file/manifest
  smoke test successfully.
- No real user capture WAV was modified or deleted during testing.

## Human Gate

With the real Focusrite ASIO input:

1. Start recording a deliberate duff and press Delete; confirm recording stops,
   no take appears, and no WAV remains.
2. Repeat with Backspace.
3. Save a take normally with Space, reject it, select it, then press Delete or
   Backspace and confirm it disappears.
4. Confirm Delete/Backspace cannot erase candidate or approved takes.
5. Type and erase text in selected-take notes, and use Audio Settings, confirming
   the shortcuts never fire in those contexts.
6. Compare a quiet, target-level, and deliberately hot take against the waveform
   and audible playback. The display should not inflate the quiet take.

Continue only Phase 0 and Phase 1 of the inventory after this interaction check,
then return the approved sessions for analysis.
