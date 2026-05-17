# Plan 0082 — Fretboard Heuristic Improvements: Drop Tuning + Register Affinity

## Goal

Make the fretboard heuristic smarter for two specific cases:

1. Notes below E2 (standard low-string open pitch) that require drop tuning to be assigned to string 0.
2. Close-interval partial chords on lower and upper strings that should use register-appropriate strings rather than collapsing to whatever is nearest the current position memory.

## Scope

- Auto drop tuning in `FretboardMapper` when a note arrives below the current string-0 open pitch.
- Register-affinity scoring term in `scoreCandidate` to nudge partial-chord notes toward register-appropriate strings.
- Chord-aware string assignment for non-strum block chords: extend `handleAutoStrumGroup` to run pitch-sorted previewFretboard assignment even when `Strum Speed = 0%`.
- Expose `getDropSemitones()` on `FretboardMapper` for diagnostics.

## Non-Goals

- No user-visible drop tuning selector or parameter.
- No multi-string retuning (only string 0 is drop-tunable).
- No phrase-level dynamic programming for chord voicing.
- No change to existing strummed-chord behavior (the strum path is unchanged).

## Result

Implemented in `src/dsp/FretboardMapper.cpp/.h` and `src/dsp/AudioEngine.cpp`:

- `openNotes[0]` is now a mutable member that auto-lowers when a sub-E2 note arrives. Persists until `reset()`.
- `scoreCandidate` gains a register-affinity term (weight 2.5) using `(noteNorm - stringNorm)²`.
- `handleAutoStrumGroup` no longer returns early when `strumAmount == 0`; `perStringSeconds` collapses to 0 naturally, delivering chord-aware string placement with zero strum delay.
- Velocity balance scaling is guarded by `strumAmount > 0.0001f` to avoid unintended velocity differences in non-strum chords.

## Verification

- Build `GuitarAG_VST3` and `GuitarAGOfflineRender`.
- Render a MIDI clip containing D2 (MIDI 38) and confirm it assigns to string 0 fret 0 (drop D detected).
- Render D2 + A2 simultaneously and confirm they land on string 0 fret 0 + string 1 fret 0 (open power chord in drop D).
- Render a 2-note block chord with Strum Speed = 0% and confirm both notes get register-appropriate string assignments.
- Render existing audition MIDI clips and confirm no regression in strummed behavior.
- Install VST3 and listen in Bitwig.
