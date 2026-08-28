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
- The human confirmed neck humbucker for every take, picking halfway between
  bridge and neck, and fretting-hand damping for every damped exercise.

Initial implementation/report commit: `3b945b5`. The pickup-matched correction
after human metadata is authoritative over the initial default-pickup numbers.

## Main Diagnosis

- Reference ringing attack centroid: 220-262 Hz. Pickup-matched EG-089:
  486-537 Hz; the earlier default-pickup 1,593-1,625 Hz is not the valid target.
- Reference energy below 500 Hz: 90.8-95.0%; matched EG-089: 64.0-78.3%.
- Reference energy above 1.5 kHz: 0.4-0.7%; matched EG-089: 3.5-9.0%.
- Reference long decay: roughly -1.0 to -2.1 dB/s; matched EG-089:
  approximately -2.7 dB/s at maximum Sustain.
- Disabling direct pick mix, attack modes, pick transient, or contact layer
  leaves residuals 51-77 dB below production and does not move aggregate attack
  metrics. The residual is not an additive pick-click layer, but it must not be
  attributed to the universal modal core before pluck geometry is matched.
- Matched maximum String Age reaches 316 Hz / 1.1% above 1.5 kHz, but couples
  that useful darkening to faster decay and insufficient sub-500 Hz energy.
- Reference fretting-hand damping preserves onset peak and reduces the early
  body by about 24-28 dB. Current palm mute cannot match both simultaneously.
- Human attack spread is contextual at 2.9-6.0 dB p10-p90 versus 0.2-1.3 dB in
  pickup-matched production groups.
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

Preserve EG-089 as control, use Humbucker/neck pickup, and add an offline fixed
pluck-position bracket:

1. current derived position near 0.10 speaking length;
2. 0.14;
3. 0.18;
4. 0.22.

After geometry selection, bracket slower decay toward the observed -1 to
-2 dB/s region and require loudness-matched human listening. Only afterward add
a separate onset-preserving fretting-hand damping intervention. Preserved-state
repicking and contextual human scatter follow those foundation checks.
