# 0079 - Auto Strum Interpreter

## Context

Human audition of the pick-stroke/player-feel exercises exposed a boundary between MIDI authoring and guitar interpretation:

- The current audition MIDI writes strummed chords as already-staggered note starts.
- That makes the DAW clip carry the right-hand stroke timing directly.
- Guitar AG should eventually accept a block chord and interpret it as a downstroke or upstroke across the assigned strings.
- The same interpreter should feed pick direction, player feel, and articulation instead of each note pretending to be an unrelated fresh event.

## Goal

Add an optional Auto Strum layer that can convert near-simultaneous chord note-ons into a physical right-hand stroke.

The model should understand:

- stroke direction: down, up, alternate/economy;
- string order from fretboard assignments;
- inter-string timing;
- velocity/energy contour across the stroke;
- interaction with `Player Feel`, `Pick Stroke`, and future left/right hand load models.

## First Small Step

Before adding note scheduling, EG-078 makes Player Feel strum-aware:

- Very short cross-string note-on clusters are treated as one continuing right-hand stroke.
- Continuing strum notes add much less cognitive load.
- Continuing strum notes add reduced dexterity load, because the left hand is usually holding a shape while the right hand sweeps.

This keeps existing MIDI compatible while reducing the misleading load-meter spike from already-staggered audition chords.

## Later Implementation Shape

Likely phases:

- Detect chord note-on clusters at the same sample time or within a short collection window.
- Assign strings/frets before scheduling the final note starts.
- Sort assigned strings by stroke direction.
- Schedule per-string note-ons internally with a strum spread time.
- Apply deterministic per-string timing/energy variation from the same player-feel state.
- Keep Auto Strum optional so precisely authored MIDI and MPE clips can bypass it.

## 2026-05-10 First Implementation Result

Implemented `StringVoice EG-079 AutoStrum`.

This first slice adds a host parameter named `Strum Speed`:

- `0%`: same-sample chord note-ons remain simultaneous.
- `100%`: adjacent strings are spaced by about 100 ms, yielding roughly a half-second spread across all six strings.
- Intermediate values use a gentle curve so moderate settings are useful for normal strums.

Engine behavior:

- `AudioEngine::render` groups MIDI messages that share the exact same block sample position.
- If the group contains at least two note-ons and `Strum Speed` is above zero, the engine predicts fretboard assignments from the current `FretboardMapper`.
- It sorts the chord by `Pick Stroke` direction and schedules note-ons internally.
- It remembers each predicted string assignment until the delayed note-on fires, so sorting and final voice assignment stay coherent.
- It passes the generated strum delay into Player Feel so load/recovery and deterministic timing variation are calculated against the generated picked-event time.

UI/audition behavior:

- The Articulation page now places `Strum Speed` after `Pick Stroke`.
- The `Player Feel` slider now has Bot/Pro/Loose landmarks at 0/50/100.
- `scripts/create-auto-strum-audition-midi.py` generates `tests/midi/guitar-ag-auto-strum-audition.mid` with same-time block chords, repeated block grooves, partial grips, and a single-note control section.

Deferred:

- Near-time collection tolerance remains future work. This implementation does not gather notes that arrive in adjacent samples or within a 1 ms window, because that requires an explicit latency/lookahead policy.

## Open Questions

- Should Auto Strum remain one global amount, or eventually gain a mode/CC trigger for selective chord interpretation?
- How should it interact with DAW-authored staggered strums?
- Should the collection window require lookahead/latency, or should exact-same-time chord note-ons remain the live-mode boundary?
