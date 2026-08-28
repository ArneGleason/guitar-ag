# Plan 0104 Low-E Reference/Model Comparison Handoff

## Outcome

Completed the first full comparison between the Plan 0103 Focusrite captures
and production EG-089.

- Two approved batches exist for all six low-E exercise requests.
- 84 usable reference attacks were detected and visually checked.
- All exercise WAVs are mono 24-bit/44.1 kHz with zero clips, no writer-overflow
  flags, and no exact-zero run longer than two samples.
- The player performed six independent ringing strokes per batch instead of
  four and six strong damped alternate events per batch instead of 12. These are
  useful deviations and do not require rerecording.
- Ringing pitch medians remain between 83.127 and 83.168 Hz, about 15 cents
  sharp of standard E2 and extremely stable across the natural performances.

Implementation/report commit: `3b945b5`.

## Main Diagnosis

- Reference ringing attack centroid: 220-262 Hz; EG-089: 1,593-1,625 Hz.
- Reference attack energy below 500 Hz: 90.8-95.0%; EG-089: 3.7-4.7%.
- Reference attack energy above 1.5 kHz: 0.4-0.7%; EG-089: 45.0-62.3%.
- Reference long decay: roughly -1.0 to -2.1 dB/s; EG-089: -4.3 to
  -4.8 dB/s at maximum Sustain.
- Disabling direct pick mix, attack modes, pick transient, or contact layer
  leaves residuals 51-77 dB below production and does not move aggregate attack
  metrics. The mismatch is the modal core, not an additive pick-click layer.
- Maximum String Age remains much too bright (1,197 Hz centroid, 19.8% above
  1.5 kHz).
- Reference fretting-hand damping preserves onset peak and reduces the early
  body by about 24-28 dB. Current palm mute cannot match both simultaneously.
- Human attack spread is contextual at 2.9-6.0 dB p10-p90 versus 0.3-1.9 dB in
  matching production groups. Maximum Player Feel reaches 2.47 dB for
  independent downstrokes but not the wider up/alternate range.
- Production repeated note-ons restart the legacy modal voice; they cannot
  preserve the moving-string state present in the alternate captures.

## Reproduction and Verification

- Report: `analysis/low-e-reference-model-comparison-20260828.md`
- MIDI generator: `scripts/create-low-e-model-evaluation-midi.py`
- Analyzer: `scripts/analyze-low-e-model-reference.py`
- Ignored evidence directory:
  `build-vs2022-x64/diagnostics/eg0104-low-e-reference-model`
- NumPy 2.1.3 and SciPy 1.15.1 were used locally.
- Both new scripts pass `py_compile`.
- The generator produces 6/12/6/6 note events for ringing-independent,
  ringing-alternate, damped-independent, and damped-alternate.
- Existing three reference-capture workflow tests still pass.

## Next Gate

Preserve EG-089 as control and build an offline 2x2 fixture:

1. current production;
2. empirical low-E modal-envelope correction only;
3. slower low-E modal decay only;
4. envelope plus decay.

Target the observed low-E spectral and -1 to -2 dB/s decay regions, but require
loudness-matched human listening before production promotion. Only afterward
add a separate onset-preserving fretting-hand damping intervention. Preserved-
state repicking and contextual human scatter follow those foundation checks.
