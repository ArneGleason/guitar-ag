# 0077 - Pick Stroke Audition MIDI

## Context

After EG-074 made `Pick Stroke = Alternate` economy-aware, the human asked for a dedicated audition MIDI file that puts the right-hand model through practical guitar material: repeated single-string picking, alternate picking, string crossings, string skips, chord strums, back-and-forth strumming, and common exercise-like riffs.

The goal is not to quote known songs. The file should use original but familiar guitar idioms so the plugin can be auditioned without copyright baggage or musical ambiguity.

## Implemented Scope

Added:

- `scripts/create-pick-stroke-audition-midi.py`
- `tests/midi/guitar-ag-pick-stroke-audition.mid`

The generated MIDI is normal channel-1 MIDI and contains marker/text events for these sections:

- open-string low-to-high and high-to-low map
- same-string tremolo picking on low E and high E
- two-string adjacent crossings
- wide string skips
- down/up open-position chord strums
- back-and-forth rhythm-guitar strum groove
- crosspicking/classical-style rolling arpeggio exercises
- pedal-tone riff studies
- melodic scale fragments across strings
- mixed finale with strum, run, tremolo, and final chord

## Suggested Audition Setup

- `Pick Stroke`: `Alternate`
- `Legato Articulation`: `0%`
- `Pick Bite`: about `20%` to `60%`
- `String Age`: to taste, often around `35%` to `50%`
- Compare against forced `Down` and forced `Up` only after first listening to `Alternate`.

Keeping `Legato Articulation` at `0%` matters for this file because the purpose is to hear picked stroke direction, not hammer-on/pull-off substitutions.

## Verification

Local checks run on 2026-05-10:

- Regenerated the MIDI:
  - `scripts/create-pick-stroke-audition-midi.py`
- Extracted note events:
  - `scripts/extract-midi-notes.py tests/midi/guitar-ag-pick-stroke-audition.mid --output /private/tmp/guitar-ag-pick-stroke-audition-notes.csv`
  - Extracted `497` note events.
- Rendered `Alternate`:
  - `31.591x` realtime
  - average `2.047` string voices
  - max `6` string voices
- Rendered forced `Down`:
  - `31.544x` realtime
  - average `2.047` string voices
  - max `6` string voices
- Confirmed the `Alternate` and forced `Down` WAVs are not byte-identical.

## Deferred

- A notation-driven stroke-direction file with explicit down/up marks.
- A strum-group-aware player interpreter that treats near-simultaneous chord notes as one hand gesture.
- A separate preset pack that stores recommended audition settings.
