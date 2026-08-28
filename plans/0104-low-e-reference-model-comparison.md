# Plan 0104 — Low-E Reference/Model Comparison

## Goal

Use the complete Plan 0103 human low-E exercise to measure realistic attack,
direction, repick, damping, decay, and performance variation, then compare those
distributions with the current EG-089 production model before changing DSP.

## Evidence Set

- two approved 44.1 kHz batches for each of the six low-E exercise requests;
- two approved connected-input noise-floor takes;
- deterministic EG-089 renders matched to the accepted batches' observed six-
  stroke and 12-stroke performance patterns at 44.1 kHz;
- both normal ringing and the closest current palm-muted/short-note model for
  the fretting-hand-damped reference condition.

## Analysis Boundary

- Treat human inconsistency as a distribution to measure, not an error to erase.
- Onset-align individual strokes before comparing attack windows.
- Compare level-independent spectral balance as well as peak/RMS variation.
- Compare down/up and odd/even alternate events separately.
- Use the ringing-versus-damped contrast as a diagnostic residual, never as a
  perfectly isolated pick sample.
- Explicitly test the known production limitation that repeated legacy-engine
  note-ons restart the string instead of re-exciting preserved vibration.
- Preserve the useful observed performance: the independent ringing batches
  contain six clear strokes rather than the requested four, while the damped
  alternate batches contain six strong alternating events rather than 12.
- Do not promote a DSP change until the measurements identify a narrow cause
  and a human-listenable A/B can be generated.

## Verification

- [x] Validate headers, clipping, zero gaps, and observed stroke counts for all
      approved reference batches.
- [x] Generate and render equivalent EG-089 MIDI exercises at 44.1 kHz.
- [x] Produce event-level and aggregate comparison evidence.
- [x] Document the smallest justified next model experiment and its listening
      gate.
