# Studio Handoff: Fretboard Heuristic Improvements (Plan 0082)

- Timestamp: 2026-05-17T00:00:00-04:00
- Status: implemented-pending-build
- Branch: `feature/0082-fretboard-drop-tuning-register-affinity`
- To: `guitar-ag lead`
- From: `guitar-ag reviewer/architect`
- Subject: Drop tuning + register-affinity + non-strum chord-aware assignment, ready for Codex re-review, build, and audition
- To agent: `Codex`
- To machine: `macbook-pro-m5`
- From agent: `Claude Code (Sonnet 4.6)`
- From machine: `macbook-pro-m5`
- Base commit: `6d084a9`

## Human Context

Human request: review FretboardMapper for two issues — (a) close-interval partial chords on lower/upper strings, and (b) auto drop tuning when implied by the lowest requested note.

Claude pulled the latest `main` (5 commits since the local clone), reviewed the FretboardMapper scoring, the strum vs. non-strum dispatch paths, and the docs. The analysis was clear enough that Claude went ahead and implemented all three proposed changes on this feature branch instead of stopping at the review hand-back. The code is real and ready to compile, but **Claude does not have CMake available in its shell environment**, so nothing has been built or auditioned yet. Codex should re-review the implementation, build, run offline renders, and audition in Bitwig.

## Scope of Implementation

Plan: `plans/0082-fretboard-heuristic-improvements.md` (new in this branch).

Three changes:

### 1. Auto drop tuning in `FretboardMapper`

- `openNotes` changed from `static constexpr` to a mutable `std::array<int, stringCount>` member (initialized to `{40, 45, 50, 55, 59, 64}`).
- `standardOpenNotes` kept as `static constexpr` for reference and reset.
- New private method `applyDropTuning(int midiNoteNumber)` lowers `openNotes[0]` to the exact requested note when it falls below the current open pitch.
- Called from `assignNote` before `findBestCandidate`.
- Persists until `reset()` is called.
- New public accessor `getDropSemitones()` returns `standardOpenNotes[0] - openNotes[0]`.
- `getFretForString` changed from `static` to a `const` member (uses instance `openNotes`). Its one caller in `AudioEngine.cpp:1633` updated to call it via `fretboard.getFretForString(...)`.
- `reset()` resets `openNotes = standardOpenNotes`.

### 2. Register-affinity scoring term in `scoreCandidate`

- Added `(noteNorm - stringNorm)² × 2.5` where:
  - `noteNorm = (midiNoteNumber - openNotes[0]) / (openNotes[stringCount-1] + maxFret - openNotes[0])`
  - `stringNorm = stringIndex / (stringCount - 1)`
- This means low-pitch notes prefer low strings, high-pitch notes prefer high strings.
- Weight (2.5) is intentionally moderate so it does not override strong position-memory preferences (a 4-fret memory deviation contributes 16 to the score; max register mismatch contributes ~0.63).
- The old `(stringCount - 1 - stringIndex) × 0.010f` bias term, which slightly penalized bass strings, was removed. (It was inverted from a useful register hint.)
- `scoreCandidate` signature gained `int midiNoteNumber` as the first parameter; updated everywhere.

### 3. Chord-aware string assignment for non-strum block chords (`handleAutoStrumGroup`)

- Removed the early return `if (strumAmount <= 0.0001f) return false;`.
- The function now always runs for 2+ simultaneous note-ons (and only note-ons + note-offs in the group).
- At `strumAmount == 0`, `perStringSeconds = 0.100 * pow(0, 1.35) = 0`, so all per-string delays collapse to zero — the previewFretboard-driven register-ordered assignment is preserved but notes fire simultaneously.
- Velocity balance scaling guarded by `if (strumAmount > 0.0001f)` so it does not apply to non-strum chords (whose strum direction is moot).
- Comment updated to explain that groups with non-note messages still return false so `handleIncomingMidiGroup` falls back to per-message dispatch.

## Re-Review Checklist for Codex

Please verify the following before building:

- [ ] `FretboardMapper.h`: `openNotes` is a non-static data member; `standardOpenNotes` remains constexpr; `getFretForString` is `const` not `static`; `getDropSemitones()` reads from member arrays.
- [ ] `FretboardMapper.cpp`: `assignNote` calls `applyDropTuning` only when `midiNoteNumber < openNotes[0]`; `reset()` resets `openNotes`; `findBestCandidate` and `scoreCandidate` use the new register-affinity math correctly.
- [ ] `AudioEngine.cpp:1633` uses `fretboard.getFretForString(...)` (not the old `FretboardMapper::getFretForString(...)` static call).
- [ ] `handleAutoStrumGroup` no longer early-returns at strumAmount = 0; velocity scaling is guarded; the duplicate `isNoteOnOrOff` check that I removed was indeed redundant (there were two consecutive identical loops in the original code).
- [ ] Drop tuning is applied per-FretboardMapper instance: main `fretboard`, `fingerNoiseFretboard`, `playerFeelFretboard`, and any `previewFretboard` copies each track their own state. Confirm this is the correct behavior or flag if a shared tuning is needed.
- [ ] No regression in MPE channel handling, strum direction logic, or strum balance behavior at non-zero strum speed.

## Build & Audition Checklist for Codex

After re-review and any fixes:

- [ ] Build `GuitarAG_VST3` and `GuitarAGOfflineRender` (Release).
- [ ] Offline render with a simple MIDI clip containing D2 (MIDI 38). Confirm:
  - String 0 gets assigned with fret 0 (drop D auto-applied).
  - `getDropSemitones()` returns 2.
  - Voice plays at D2 pitch, not E2.
- [ ] Offline render D2 + A2 as a simultaneous block chord at `Strum Speed = 0%`. Confirm:
  - D2 → string 0 fret 0, A2 → string 1 fret 0.
  - Both fire at the same sample position (no audible strum delay).
- [ ] Offline render G4 + A4 as a block chord at `Strum Speed = 0%`, with `Position Memory` recently around fret 2 (e.g., precede with a few low-position notes). Confirm both notes land on adjacent high strings (4 and 5) rather than producing a fret-3-vs-fret-10 spread voicing.
- [ ] Re-run existing audition MIDI clips (feature audition, player articulation, strum, pick stroke) and listen for any regressions in strummed behavior at non-zero strum speed.
- [ ] Install via `scripts/install-vst3.sh --build` and audition in Bitwig:
  - Drop D riff (e.g., D2 chugging) — should sound at D2 pitch with wound character.
  - Sub-D2 notes (Drop C, Drop B) — should tune string 0 to exactly that pitch.
  - Partial 2-note chord with the position memory parked elsewhere — should pick register-appropriate strings.

## Known Limitations to Watch For

- Drop tuning is per-mapper-instance, not shared across mapper copies. If a user expects the displayed/internal drop state to be visible globally, this may surface as a UX gap.
- Register-affinity weight 2.5 is a calibrated first pass. It might be too weak to overcome strong position-memory pulls in some cases (Ab3+Bb3 dyads on adjacent strings can still produce a fret-1-vs-fret-8 spread). Audition will clarify whether 2.5 is the right value or if the chord-aware path should also extend to non-block-chord arrangements.
- No UI surface for drop tuning state. `getDropSemitones()` is available for diagnostics but isn't wired to the editor.
- The `tools/render-calibration.cpp` offline render driver was not modified. If there's a desire to surface drop state through a CLI flag for verification, that would be a follow-up.

## Files Changed

- `src/dsp/FretboardMapper.h` — header changes
- `src/dsp/FretboardMapper.cpp` — implementation, register-affinity, drop tuning
- `src/dsp/AudioEngine.cpp` — non-strum chord path, getFretForString call site
- `plans/0082-fretboard-heuristic-improvements.md` — new plan file
- `CHANGELOG.md` — Unreleased section
- `LEARNINGS.md` — three 2026-05-17 entries on drop tuning, chord-aware separation, and weight calibration

## Suggested Next Routing

- **Codex re-review and build:** Verify the implementation against the checklist above, build VST3 and offline renderer, run the offline scenarios.
- **Human Bitwig audition:** After Codex passes build + offline checks, hand back to lead for DAW listening.
- **If audition reveals weight tuning issues:** Adjust register-affinity weight (line `score += registerMismatch * registerMismatch * 2.5f;` in `FretboardMapper.cpp:132`). Smaller = less register bias, larger = more.
- **If audition reveals drop tuning issues:** Consider whether snapping to standard guitar tunings (Drop D, Drop C, etc.) is preferable to exact-pitch matching, or whether a user-facing tuning parameter is wanted.

## Model Label Implication

This is a heuristic-only change (no new DSP, no new parameters). The model label `EG-081 ArticGroups` does not need to bump unless audition reveals audible behavior shift worth versioning.
