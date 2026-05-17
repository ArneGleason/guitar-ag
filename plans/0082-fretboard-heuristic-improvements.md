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
- After Codex re-review, partial same-sample chords smaller than six notes now use `FretboardMapper::assignNoteGroup`, a fixed-size no-allocation search over string/fret candidates. This keeps the single-note register-affinity weight modest while fixing greedy dyad spreads such as G4+A4 after low-position context.
- The visible model label is `StringVoice EG-082 FretboardHeuristics` for DAW audition clarity.

## Verification

- Build `GuitarAG_VST3` and `GuitarAGOfflineRender`.
- Mapper probe: D2 (MIDI 38) assigns to string 0 fret 0 and reports `getDropSemitones() == 2`.
- Mapper probe: D2 + A2 simultaneous group assigns to string 0 fret 0 plus string 1 fret 0.
- Mapper probe: G4 + A4 after low-position context assigns to string 4 fret 8 plus string 5 fret 5, avoiding the previous fret-3/fret-10 spread.
- Offline render: D2 favored D2 over E2 by 56.12 dB in the analysis window.
- Offline render: D2 + A2 at `Strum Speed = 0%`, `Player Feel = 0%` starts at the expected note-on sample window and shows both requested pitch bands.
- Offline render: G4 + A4 context render shows both requested pitch bands.
- Existing audition MIDI clips rendered successfully: feature audition, player articulation, auto-strum, and pick-stroke.
- Install VST3 and listen in Bitwig.
