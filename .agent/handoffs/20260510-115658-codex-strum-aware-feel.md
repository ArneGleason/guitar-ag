# Studio Handoff: Codex Strum-Aware Player Feel

- Timestamp: 2026-05-10T11:56:58-04:00
- Status: handoff
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review EG-078 strum-aware Player Feel and Auto Strum plan
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Pull Instructions

Please run:

```bash
git fetch origin
git switch main
git pull --ff-only origin main
```

Then review the latest pushed `main` commit for EG-078.

## Human Context

Human DAW audition found that already-staggered chord strums were making the Player Feel meters accumulate too much load. The physical model should distinguish a held left-hand chord shape plus a single right-hand sweep from a sequence of unrelated single-note decisions. The human also noted that the global `Export Settings` header control needed a few more pixels of vertical space.

## Implementation Summary

- Changed model label to `StringVoice EG-078 StrumAwareFeel`.
- Added `plans/0079-auto-strum-interpreter.md` for the future block-chord-to-strum scheduling feature.
- Added a strum-continuation heuristic in `AudioEngine::processPlayerFeelNoteOn`:
  - previous note exists;
  - new note is on a different near-adjacent string;
  - note-on interval is within about 70 ms;
  - travel direction is cross-string.
- For detected strum continuations:
  - cognitive load impulse is strongly reduced;
  - dexterity load impulse is substantially reduced;
  - deterministic timing/velocity variation still applies from the resulting load.
- Increased the plugin header height and moved `Export Settings` slightly lower so it has more breathing room.
- Updated changelog, decisions, learnings, DSP notes, test plan, audition notes, build notes, and Studio Pattern state.

## Review Focus

- Confirm the strum-continuation heuristic is deterministic and bounded.
- Confirm same-string fast picking still accumulates load normally.
- Confirm staggered chord strums accumulate less cognitive/dexterity load than EG-077.
- Confirm `Player Feel = 0` remains neutral.
- Confirm the header layout does not overlap the title/subtitle/build text.
- Review `plans/0079-auto-strum-interpreter.md` and flag any architectural concerns before a future Auto Strum implementation.

## Known Limitations

- EG-078 does not schedule strums from simultaneous MIDI chord note-ons yet.
- The strum detection is heuristic and based on short cross-string timing. A future explicit Auto Strum mode should be more intentional and controllable.
