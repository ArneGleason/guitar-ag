# Plan 0103 — Low-E Capture Exercise

## Goal

Replace the ambiguous broad starter inventory with the smallest complete low-E
reference set needed for a focused physical-model comparison.

## Capture Gate

Reuse the approved input noise floor when the interface, guitar, and gain remain
unchanged. Record one approved batch for each low-E condition:

1. ringing independent downstrokes: four strokes, controlled resets, long final decay;
2. ringing independent upstrokes: four strokes, controlled resets, long final decay;
3. ringing alternate repicking: 12 continuous strokes, down first;
4. hand-damped downstrokes: six separated strokes;
5. hand-damped upstrokes: six separated strokes;
6. hand-damped alternate picking: 12 continuous strokes, down first.

Natural performance timing is intentional. Do not use a metronome. Leave one
second of initial silence and any comfortable hand-transition time; analysis
will find and align the events.

Keep the Focusrite at 44.1 kHz for the complete exercise so every new recording
matches the approved noise-floor reference.

## Scope Boundary

- Remove foam damping from the active inventory; it is not required.
- Do not request high E, middle strings, force, depth, or pick-material variants
  before the six-item low-E model comparison.
- Use new request IDs so older single-event sessions remain preserved but cannot
  accidentally satisfy the revised exercise.
- Regenerating with `--force` replaces request/inventory definitions only. It
  must not delete or alter any existing session manifest or WAV.

## Verification

- [x] Generator emits seven items across Phase 0 and Phase 1.
- [x] Phase 1 stroke counts are exactly 4, 4, 12, 6, 6, and 12.
- [x] Every Phase 1 request says no metronome and requests one approved batch.
- [x] No active request requires foam or cloth.
- [x] Existing noise/ringing/hand-damped session WAVs remain untouched.
- [x] Default inventory routes to the first new incomplete low-E exercise.
- [x] Active low-E requests preserve the existing 44.1 kHz capture rate.
- [x] Human completed two approved batches for all six exercise items and
      stopped for Plan 0104 model analysis.
