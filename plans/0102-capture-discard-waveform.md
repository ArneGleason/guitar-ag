# Plan 0102 — Capture Discard and Waveform Review

## Goal

Make repeated human capture faster by letting obvious duff performances be
discarded immediately and by showing the selected recording at an honest
absolute amplitude scale.

## Scope

- Delete or Backspace during recording stops and discards the partial WAV.
- Delete or Backspace on a selected rejected take permanently erases its WAV
  and removes it from `session.json` and the take list.
- Candidate and approved takes cannot be deleted until explicitly rejected.
- Notes editing, Audio Settings, file choosers, and background applications keep
  normal Delete/Backspace behavior and never trigger capture deletion.
- The selected take shows its full-duration waveform without peak normalization,
  plus peak, RMS, duration, a -12 dBFS guide, and hot/possible-clip labeling.

## Safety Boundary

- Permanent deletion is limited to a `.wav` whose direct parent is the current
  session directory.
- A failed filesystem deletion leaves the take in the manifest and list.
- One physical key press performs at most one deletion even if Windows repeats
  the key while it is held.
- Aborted recordings never enter the take list or manifest.

## Verification

- [x] Windows Release capture app builds.
- [x] Disposable rejected takes are deleted by Delete and Backspace, including
      their files and manifest entries.
- [x] Candidate and approved takes survive Delete/Backspace with a clear status.
- [x] A held key does not cascade through multiple rejected takes.
- [ ] Notes and Audio Settings retain normal Delete/Backspace behavior.
- [x] A selected valid WAV renders at absolute scale with correct measurements.
- [ ] Human confirms Delete/Backspace aborts a live ASIO recording without
      leaving a WAV or manifest entry.
