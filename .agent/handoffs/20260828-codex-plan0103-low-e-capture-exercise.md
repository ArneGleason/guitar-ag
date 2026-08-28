# Plan 0103 Low-E Capture Exercise Handoff

## Outcome

Replaced the speculative 30-item active capture inventory with the smallest
complete low-E set needed for a current-model comparison.

- Reuse the approved input noise floor while guitar, Focusrite input, gain, and
  sample rate remain unchanged.
- Capture ringing independent down, ringing independent up, ringing alternate,
  hand-damped down, hand-damped up, and hand-damped alternate.
- The stroke counts are 4, 4, 12, 6, 6, and 12 respectively.
- One take is one complete WAV batch. Only one approved batch is required; the
  second available take is for a retry or deliberately useful variation.
- Timing is natural, no metronome is required, and extra silence or hand travel
  is accepted because analysis will locate the events.
- Foam damping, other strings, dynamics, pick depth, and pick materials remain
  out of scope until the low-E set proves useful.
- The complete exercise is fixed at 44.1 kHz to match the approved noise floor.

Implementation commit: `1075fdd`.

## Verification Completed

- The three Python reference-capture workflow tests pass.
- Both inventory/request generators compile with `py_compile`.
- Windows Release `GuitarAGReferenceCapture` builds successfully with ASIO.
- The generator emits seven items across two phases with exact Phase 1 stroke
  counts of 4, 4, 12, 6, 6, and 12.
- Every Phase 1 request says not to use a metronome, requires one approval, and
  offers no more than two WAV batches.
- The active requests contain no foam requirement and prefer only 44.1 kHz.
- Forced regeneration of the real inventory changed none of the 22 existing
  session files.
- The app is open on `low-e-eval-ringing-down` using Focusrite USB ASIO.

## Human Gate

Approve one representative batch for each of the six Phase 1 items, advancing
through the app's inventory in order. Record a second batch only to replace a
duff or preserve genuinely useful variation. Keep guitar, pickup, controls,
pick, picking location, interface gain, and 44.1 kHz rate fixed.

After all six items are approved, stop capturing and return for the full model
comparison. The first analysis should onset-align individual strokes and compare
down/up attack distributions, alternate re-excitation, long decay, and the
ringing-versus-hand-damped diagnostic residual against the current model.
