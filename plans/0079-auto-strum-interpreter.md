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

## Open Questions

- Should Auto Strum be a global amount, a mode, or a per-note/CC trigger?
- How should it interact with DAW-authored staggered strums?
- Should the collection window require lookahead/latency, or should it only work for exact-same-time chord note-ons in live mode?
