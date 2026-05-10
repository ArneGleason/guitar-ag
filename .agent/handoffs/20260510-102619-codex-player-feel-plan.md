# Studio Handoff: Codex Held-Strum MIDI And Player Feel Plan

- Timestamp: 2026-05-10T10:26:19-04:00
- Status: planning-plus-audition-asset
- Branch: `main`
- To: `guitar-ag lead`
- From: `guitar-ag lead`
- Subject: Left/right hand split and deterministic player-feel architecture
- To agent: `Codex`
- To machine: `macbook-pro-m5`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

The human observed that the pick-stroke audition MIDI's repeated chord strums were too note-gated: the generated MIDI made it sound like the left hand released every chord note before each new stroke. The intended guitar model is a held left-hand chord shape while the right hand re-articulates strings.

The human also asked for a deterministic player-interpretation model that can vary timing and velocity/energy from physical and cognitive causes rather than DAW-style random humanization.

## Changes

- Updated `scripts/create-pick-stroke-audition-midi.py`.
- Regenerated `tests/midi/guitar-ag-pick-stroke-audition.mid`.
- Chord-strum sections now sustain each fretted string until that string is struck again or the chord changes.
- Added `plans/0078-player-feel-model.md`.
- Updated `docs/audition-midi.md`, `docs/test-plan.md`, `CHANGELOG.md`, `.agent/PROJECT_LOG.md`, and `.agent/session.json`.

## Player Feel Architecture Summary

The proposed model separates:

- left-hand/fretting state
- right-hand/articulation state
- player state

The player state tracks:

- cognitive load
- dexterity load
- endurance/recovery

Variation should be deterministic, seeded from stable musical context, and scaled by those load signals. The first practical implementation slice is likely either:

- right-hand feel timing/energy for picked notes; or
- held-shape repick semantics inside the engine.

## Verification Run

- `python3 -m py_compile scripts/create-pick-stroke-audition-midi.py`
- `scripts/create-pick-stroke-audition-midi.py`
- `scripts/extract-midi-notes.py tests/midi/guitar-ag-pick-stroke-audition.mid --output /private/tmp/guitar-ag-pick-stroke-audition-notes.csv`
  - extracted `497` note events
- Offline render:
  - `--pick-stroke alternate`
  - `--legato-articulation 0`
  - `--pick-bite 0.35`
  - `--string-age 0.35`
  - `26.338x` realtime
  - average `2.609` string voices
  - max `6` string voices

## Notes

The average active voice count increased versus the short-gated version, which is expected because chord tones now sustain more like a real held fretting-hand shape.
