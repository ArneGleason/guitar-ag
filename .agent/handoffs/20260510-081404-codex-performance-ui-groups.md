# Studio Handoff: Codex Performance UI Groups

- Timestamp: 2026-05-10T08:14:04-04:00
- Status: implementation
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review EG-070 Performance page hierarchy
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

The human accepted the separate `Slide Squeak` control and found about 20% to be a better musical balance. They also noted that the Performance page had become too flat and dense: `Amp Feedback` was visible but its `Return Clip` switch could fall off the bottom. The requested direction was a second hierarchy where primary controls stay visible and related character controls are clearly subordinate, ideally with small expandable rows and aligned sliders.

## Changes Made

- Capped `Slide Squeak` at 100% and changed its default to 20%.
- Changed offline-renderer `--slide-squeak` default to `0.2`.
- Added disclosure buttons on the Performance page:
  - `Neck Slide` expands/collapses `Fret Steps`, `Slide Lift`, and `Slide Squeak`.
  - `Finger Noise` expands/collapses `Lookahead`.
  - `Amp Feedback` expands/collapses `Return Clip`.
- Left the slide subgroup open by default; finger-noise and feedback subgroups default collapsed.
- Kept primary/secondary slider columns aligned.
- Muted secondary labels to make them read as character tweaks.
- Updated `CHANGELOG.md`, `DECISIONS.md`, `LEARNINGS.md`, `docs/dsp-notes.md`, `docs/test-plan.md`, `docs/build-notes.md`, and added `plans/0070-performance-ui-groups.md`.
- Changed the model label to `StringVoice EG-070 PerformanceUi`.

## Review Focus

- Confirm disclosure visibility state is complete and no hidden controls remain visible/clickable.
- Confirm all Performance controls still attach to the correct APVTS parameters.
- Confirm the Performance page fits with all three disclosure groups open.
- Confirm `Slide Squeak` range/default changes are consistent across APVTS, `AudioEngine`, `StringVoice`, and offline render.

## Suggested Manual Checks

- Pull latest `main`.
- Build/install the VST3.
- Confirm the UI header shows `v0.2.6 / StringVoice EG-070 PerformanceUi`.
- Open the Performance page and toggle each disclosure.
- Confirm `Amp Feedback` and `Return Clip` are reachable.
