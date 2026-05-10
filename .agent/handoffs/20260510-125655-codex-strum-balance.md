# Studio Handoff: Codex Strum Direction Balance

- Timestamp: 2026-05-10T12:56:55-04:00
- Status: handoff
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review EG-080 Strum Balance follow-up
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

Then review the latest pushed `main` commit for EG-080.

## Human Context

Human DAW audition found the EG-079 Auto Strum implementation musically convincing. The requested follow-up is a simple direction-energy control: generated downstrokes and upstrokes should be balanced at center, and moving the control to either side should reduce one stroke direction rather than boosting the other. This avoids velocity clipping when source MIDI is already loud and supports near-ghost upstrokes or downstrokes at the extremes.

## Implementation Summary

- Changed model label to `StringVoice EG-080 StrumBalance`.
- Added `Strum Balance` as a bipolar host/VST parameter with center default.
- Added an Articulation-page `Strum Balance` slider directly under `Strum Speed`.
- Added an info note explaining that the control affects generated Auto Strum note-ons only.
- Added offline-renderer `--strum-balance`.
- Updated the settings JSON export with `strumBalance`.
- Applied balance inside `AudioEngine::handleAutoStrumGroup`:
  - center preserves EG-079 generated velocities;
  - positive values reduce generated upstroke note velocities;
  - negative values reduce generated downstroke note velocities;
  - reduction reaches about 94% at the extremes;
  - the opposite direction is not boosted.
- Updated changelog, decisions, learnings, DSP notes, test plan, audition notes, build notes, plan 0079, review queue, project log, and session state.

## Review Focus

- Confirm `Strum Balance = 0` is neutral for EG-079 Auto Strum behavior.
- Confirm positive values reduce generated upstroke velocities only.
- Confirm negative values reduce generated downstroke velocities only.
- Confirm the control is a reducer, not a booster, so high input velocities cannot clip because of this control.
- Confirm the control affects generated Auto Strum note-ons before Player Feel variation.
- Confirm single-note picking and already-staggered authored MIDI remain unchanged.
- Confirm the UI row, label text, and info popup are readable and do not clip.
- Confirm `--strum-balance` works in the offline renderer.

## Known Limitations

- Auto Strum still only groups exact same-sample block chords. Near-time chord collection remains deferred.
- Direction balance applies to generated Auto Strum strokes, not to arbitrary authored staggered strums.
- There is no separate natural-default preset yet; center is intentionally balanced per the human request, and musical defaults can be captured later through the preset model.

## Checks Run

- `git diff --check`.
- Built `GuitarAGOfflineRender`.
- Built `GuitarAG_VST3`.
- Rendered the Auto Strum audition with `--strum-balance 0.0`, `0.65`, and `-0.65`.
- Re-rendered the `0.65` case and confirmed the WAV hash was byte-identical.
- Installed the VST3 with `scripts/install-vst3.sh --build`.
