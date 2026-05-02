# Plan 0055 — Player Articulation Model

## Goal

Design the next layer above the raw string model: a guitar-player interpretation system that chooses plausible left-hand positions and turns suitable MIDI gestures into guitar-like articulations.

## Motivation

The current fretboard mapper assigns each note to a plausible string/fret position, but it does not yet reason about phrases, held chord shapes, finger reach, hand position, or whether a new note should be picked, hammered-on, pulled-off, or tapped.

The next feature family should make MIDI feel less like independent synth notes and more like a guitarist deciding what the hands can plausibly do.

## Proposed Controls

- `Legato Articulation` or similar player-mode amount.
- Separate hammer-on and pull-off behavior if the single control is too blunt.
- 0-20%: no automatic hammer-ons/pull-offs.
- 20-70%: increasingly use left-hand hammer-ons and pull-offs when the opportunity is clear.
- 70-100%: increasingly allow right-hand taps and more adventurous threaded legato/tapping behavior.

## Heuristic Direction

- Track active notes as a possible fretting-hand shape, not just independent voices.
- Prefer realistic positions near the current hand position unless the phrase strongly implies a shift.
- Preserve open strings when musically plausible.
- Treat same-string nearby note transitions as hammer-on/pull-off candidates.
- Treat larger or faster same-string leaps as possible tapping candidates only in the high range of the control.
- Never block notes that are physically awkward; fall back to picked notes when the heuristic is uncertain.

## Sound Model Needs

- Hammer-on: no pick excitation, lower/no pick texture, quick left-hand impulse into an already chosen string.
- Pull-off: release/pluck from the fretting finger, often softer and slightly noisier than a pick.
- Right-hand tap: stronger finger-impact excitation with little/no pick scrape and a sharper fret/contact onset.
- Legato transitions should preserve some string state where possible instead of always starting a brand-new picked voice.

## Out Of Scope For First Pass

- Full tablature-quality fingering solver.
- Exact anatomical hand model.
- Alternate tunings.
- User-configurable guitar setup profiles.
- Machine-learned performance model.

## Verification Ideas

- MIDI with clear same-string one- or two-fret moves should become hammer-ons/pull-offs as the control rises above 20%.
- Chords should keep stable plausible left-hand assignments rather than jumping strings every note.
- Above 70%, fast high-register note patterns should start to use tap-like articulation.
- At 0%, the current picked-note behavior should remain effectively unchanged.
