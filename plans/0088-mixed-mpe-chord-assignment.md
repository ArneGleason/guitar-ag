# Plan 0088 - Mixed MPE Chord Assignment

## Goal

Keep chord-aware string assignment active when an MPE host sends pitch wheel,
CC74, or pressure resets in the same sample as the chord note-ons.

## Scope

- Let same-sample mixed MIDI groups dispatch note-offs and expression messages
  before grouping the remaining note-ons for Auto Strum / chord assignment.
- Keep bass-register notes from drifting to high frets on lower strings when a
  natural low-fret alternative exists.
- Make incoming MIDI diagnostics report occupancy as a before-processing
  snapshot instead of leaving the after fields at zero.

## Verification

- Rebuild the VST3 and offline renderer.
- In Bitwig, replay the logged section around 19-26 seconds and confirm E2+B2
  returns to low E plus A-string B instead of both notes stealing low E.
- Copy the diagnostics log and confirm MPE controller resets no longer prevent
  `strumPreferredString` from appearing on same-sample chord assignments.
