# Plan 0108 — Low/High-E Endpoint Model

## Goal

Use the completed open-high-E ringing captures with the accepted low-E evidence
to determine whether Plan 0105's pluck geometry and decay correction generalize
across the instrument, then prepare the smallest justified listening candidate.

## Evidence Boundary

- two approved high-E batches each for independent down, independent up, and
  12-stroke continuous down-first alternate picking;
- the approved Plan 0103 low-E reference distributions;
- the EG-089 production model at matched humbucker/neck pickup settings;
- Plan 0105's accepted fixed 0.14 pluck position / 1.50x body decay and Plan
  0106's accepted 1 ms legacy-repick de-click.

No muted high-E capture is required for this checkpoint. Fretting-hand damping
will be requested only if the ringing endpoint comparison identifies a question
that cannot be answered from the existing low-E damping evidence.

## Work

- [x] Validate high-E headers, onsets, long tails, pitch, and capture quality.
- [x] Render matching current and accepted-low-E-candidate high-E exercises.
- [x] Compare attack spectrum, body spectrum, decay, level, and variation at
      both endpoints.
- [x] If one uniform candidate does not generalize, bracket the narrowest
      register-dependent geometry or decay interpolation offline.
- [x] Produce a compact loudness-matched A/B for human listening.
- [x] Keep all experimental controls out of the production VST3 until the
      endpoint candidate passes listening and real-time/polyphony/MPE review.

## Stop Point

Stop after the endpoint A/B. Do not request A/D/G/B or high-E damping until the
human verdict identifies the next useful evidence.

## Human Verdict

Rejected. The candidate's numerical 60 ms match did not survive listening: it
was dark, large, flat, and less convincing than production. Production remains
the preferred high-E baseline. Continue with Plan 0109's transient-preserving
time-domain bracket rather than increasing amplitude damping.
