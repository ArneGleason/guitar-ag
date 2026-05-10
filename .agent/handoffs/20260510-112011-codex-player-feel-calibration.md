# Studio Handoff: Codex Player Feel Calibration

- Timestamp: 2026-05-10T11:20:11-04:00
- Status: handoff
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review EG-076 Player Feel calibration, meters, and settings export
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

Then review the latest pushed `main` commit for EG-076.

## Human Context

Human DAW audition found `Player Feel = 100%` in EG-075 natural and useful, not exaggerated enough. The preferred recovery was around 2 to 2.5 seconds. The human also asked for visible load gauges and a simple way to export/copy current plugin settings so audition context can be pasted into the Studio thread.

## Implementation Summary

- Changed model label to `StringVoice EG-076 PlayerFeelCalibration`.
- Recalibrated `Player Feel` so 50% is near the old EG-075 100% natural feel, while 100% intentionally overshoots into sloppier timing and velocity/energy variation.
- Changed `Feel Recovery` default from 0.85 seconds to 2.0 seconds.
- Extended `Feel Recovery` range from 4.0 seconds to 8.0 seconds.
- Added audio-engine Player Feel meter output for:
  - cognitive load
  - dexterity load
  - endurance
- Decayed the visible load state at audio-block boundaries so the meters visibly clear during rests, not only when the next note arrives.
- Stored those meter values in processor atomics after render.
- Added Articulation-page live meters polled by the editor timer.
- Added `Export Settings`, which opens a copyable JSON popup containing current APVTS parameter values plus live Player Feel meter values.
- Updated `CHANGELOG.md`, `DECISIONS.md`, `LEARNINGS.md`, `docs/dsp-notes.md`, `docs/test-plan.md`, `docs/audition-midi.md`, `docs/build-notes.md`, `plans/0078-player-feel-model.md`, and Studio Pattern state.

## Checks Run

- Built and installed the VST3 with `scripts/install-vst3.sh --build`.
- Rendered the pick-stroke audition MIDI with:
  - `--player-feel 0 --player-feel-recovery 2.0`
  - `--player-feel 0.5 --player-feel-recovery 2.0`
  - `--player-feel 1.0 --player-feel-recovery 2.0`
  - repeated `--player-feel 1.0 --player-feel-recovery 2.0`
- Verified the 0%, 50%, and 100% renders produce different SHA-256 hashes.
- Verified repeated 100% renders are byte-identical.

## Review Focus

- Confirm the scaling change is bounded and does not make `Player Feel = 0` non-neutral.
- Confirm recovery defaults/ranges are synchronized across APVTS, `AudioEngine`, and offline renderer defaults.
- Confirm meter transfer from DSP to UI remains real-time safe.
- Confirm meter decay remains deterministic and visibly clears during rests/easier passages.
- Confirm the settings export JSON is valid and includes all current parameter IDs plus meter values.
- Confirm the Articulation-page layout remains usable at the new preferred height and does not hide existing controls.

## Known Limitations

- The Player Feel model still delays note starts only. Early timing still needs a future lookahead-aware pass.
- The export is a copyable context snapshot, not a full preset import/export system.
- Manual DAW audition is still needed to judge the new top-half sloppiness range and whether 2.0 seconds should remain the default recovery.
