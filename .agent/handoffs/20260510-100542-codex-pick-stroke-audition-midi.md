# Studio Handoff: Codex Pick Stroke Audition MIDI

- Timestamp: 2026-05-10T10:05:42-04:00
- Status: audition-asset
- Branch: `main`
- To: `guitar-ag lead`
- From: `guitar-ag lead`
- Subject: New pick-stroke audition MIDI for human DAW listening
- To agent: `Codex`
- To machine: `macbook-pro-m5`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

The human asked for a MIDI workout that puts the smarter EG-074 `Pick Stroke = Alternate` behavior through practical guitar material: repeated picking, alternate picking, chord strums, string crossings, string skips, and riff-like/common exercise idioms.

This handoff is mainly for traceability and future reviewer awareness. It is not a request for code review by itself.

## Added Assets

- `scripts/create-pick-stroke-audition-midi.py`
- `tests/midi/guitar-ag-pick-stroke-audition.mid`
- `plans/0077-pick-stroke-audition-midi.md`

## Audition Setup

- `Pick Stroke`: `Alternate`
- `Legato Articulation`: `0%`
- `Pick Bite`: about `20%` to `60%`
- `String Age`: to taste, likely around `35%` to `50%`

For A/B checks, compare against forced `Down` and forced `Up` after first listening to `Alternate`.

## MIDI Sections

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

## Verification Run

- Regenerated the MIDI with `scripts/create-pick-stroke-audition-midi.py`.
- Extracted note events with `scripts/extract-midi-notes.py`.
  - Extracted `497` note events.
- Rendered with `--pick-stroke alternate`, `--legato-articulation 0`, `--pick-bite 0.35`, and `--string-age 0.35`.
  - `31.591x` realtime
  - average `2.047` string voices
  - max `6` string voices
- Rendered with forced `--pick-stroke down` using the same settings.
  - `31.544x` realtime
  - average `2.047` string voices
  - max `6` string voices
- Confirmed the `Alternate` and forced `Down` WAVs are not byte-identical.

## Notes

The material is original audition/exercise material, not copied repertoire. It is designed to make the picking model easy to judge rather than to be a polished musical piece.
