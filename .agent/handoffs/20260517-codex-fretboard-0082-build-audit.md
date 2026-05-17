# Studio Handoff: Plan 0082 Codex Build/Audit Pass

- Timestamp: 2026-05-17T10:26:50-04:00
- Status: implemented-build-offline-pass
- Branch: `feature/0082-fretboard-drop-tuning-register-affinity`
- From: `Codex`
- From machine: `macbook-pro-m5`
- To: `guitar-ag lead`
- Subject: Re-reviewed Claude draft, fixed partial-chord greedy assignment, built and offline-rendered Plan 0082

## Summary

Codex pulled Claude's Plan 0082 feature branch and treated commit `2e24e65` as a draft implementation. The drop-tuning lifecycle and non-strum chord-aware path were sound, but the draft still failed the requested G4+A4 partial-chord case after low-position context: it assigned G4 to high-E fret 3 and A4 to B-string fret 10.

The fix keeps the register-affinity weight at `2.5f` and adds `FretboardMapper::assignNoteGroup` for same-sample partial chords smaller than six notes. This is a fixed-size, no-allocation group search over string/fret candidates, used by `AudioEngine::handleAutoStrumGroup` before resolving pick-stroke order. Full six-note block chords keep the previous sequential preview assignment path.

## Verification

- Built `GuitarAG_VST3` Release.
- Built `GuitarAGOfflineRender` Release.
- Mapper probe:
  - D2 maps to string 0 fret 0.
  - `getDropSemitones() == 2`.
  - D2+A2 block maps to string 0 fret 0 plus string 1 fret 0.
  - G4+A4 after low-position context maps to string 4 fret 8 plus string 5 fret 5.
  - Ab3+Bb3 after low-position context maps to string 2 fret 6 plus string 3 fret 3.
- Offline scenario renders:
  - D2 render favored D2 over E2 by 56.12 dB in the analysis window.
  - D2+A2 block at `Strum Speed = 0%`, `Player Feel = 0%` starts at the expected note-on sample window and contains both pitch bands.
  - G4+A4 context render contains both requested pitch bands.
- Existing audition MIDI re-renders:
  - feature audition: 31.921x realtime, max 6 string voices.
  - player articulation: 50.759x realtime, max 3 string voices.
  - auto-strum: 20.350x realtime, max 6 string voices.
  - pick-stroke: 26.016x realtime, max 6 string voices.

## Notes for Human Bitwig Audition

- Focus on Drop D/sub-D low-string riffs, especially D2 chugging and D2+A2 open power chords.
- Check G4+A4 and Ab3+Bb3 dyads after low-position phrases; they should feel like compact partial grips rather than one low-fret note plus one far-fret neighbor.
- Recheck nonzero Auto Strum feel, because partial same-sample chords now use the group assignment before the existing stroke-order and delay logic.
- The VST3 install step should be run after the clean commit so the plugin build identity does not include a dirty suffix.
