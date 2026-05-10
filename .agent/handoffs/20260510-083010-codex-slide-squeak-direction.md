# Studio Handoff: Codex Slide Squeak Direction

- Timestamp: 2026-05-10T08:30:10-04:00
- Status: implementation
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review EG-071 direction-aware slide squeak controls
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

The human accepted the EG-070 UI grouping and noticed a useful quirk during DAW audition: the slide squeak amount controlled upward slide squeak, but did not give a convincing balance for downward squeak/return motion. They suggested that separate up/down slide squeak volumes may be useful now that the slide controls are tucked under the `Neck Slide` disclosure.

They also raised a larger preset-model question. That is documented as a separate plan in `plans/0072-preset-model.md`; no preset browser implementation is included in this patch.

## Changes Made

- Preserved the existing `slideSqueak` parameter ID and renamed its host-facing parameter name to `Slide Squeak Up`.
- Added a new `slideSqueakDown` APVTS parameter with host-facing name `Slide Squeak Down`.
- Added `Squeak Up` and `Squeak Down` rows under the `Neck Slide` disclosure.
- Added info notes for both controls.
- Added `AudioEngine::setSlideSqueakDown` and a smoothed `slideSqueakDown` value.
- Changed `StringVoice::renderSample` and `renderContactLayer` to accept up/down slide squeak amounts.
- Added `slideFretDirection` to `StringVoice`; it updates from nonzero `Neck Slide` deltas and persists through the short decaying scrape tail.
- Added offline-renderer `--slide-squeak-down` and `--slide-squeak-up`; `--slide-squeak` remains a legacy alias for upward squeak.
- Updated `CHANGELOG.md`, `DECISIONS.md`, `LEARNINGS.md`, `docs/dsp-notes.md`, `docs/test-plan.md`, `docs/build-notes.md`, `docs/audition-midi.md`, and `plans/0070-performance-ui-groups.md`.
- Added `plans/0071-slide-squeak-direction.md`.
- Added `plans/0072-preset-model.md` for the future stock/user preset workflow.
- Changed the model label to `StringVoice EG-071 SlideSqueakDirection`.

## Review Focus

- Confirm the old `slideSqueak` parameter ID is preserved and should keep existing session/automation compatibility for the upward amount.
- Confirm `slideSqueakDown` is wired from APVTS to UI to `AudioEngine` to `StringVoice`.
- Confirm `slideFretDirection` updates only from real slide movement and does not alter pitch, MPE, whammy, `Fret Steps`, or `Slide Lift`.
- Confirm both info notes are understandable and fit the EG-070 UI taxonomy.
- Review `plans/0072-preset-model.md` for preset architecture sanity, but treat it as planning-only.

## Checks Run

- `git diff --check`
- `scripts/install-vst3.sh --build`
- Directional offline smoke renders with `GuitarAGOfflineRender`:
  - Upward slide, `Squeak Up` active vs muted: `diff_rms=0.001051120`, `diff_peak=0.075339198`.
  - Upward slide, changing `Squeak Down` only: `diff_rms=0.000000000`, `diff_peak=0.000000000`.
  - Downward slide, `Squeak Down` active vs muted: `diff_rms=0.001013305`, `diff_peak=0.070655465`.
  - Downward slide, changing `Squeak Up` only: `diff_rms=0.000000000`, `diff_peak=0.000000000`.

## Suggested Manual Checks

- Pull latest `main`.
- Build/install the VST3.
- Confirm the UI header shows `v0.2.6 / StringVoice EG-071 SlideSqueakDirection`.
- On the Performance page, expand `Neck Slide` and confirm `Squeak Up` and `Squeak Down` appear beneath `Slide Lift`.
- Draw an upward `Neck Slide` move with `Squeak Up` at 0% and `Squeak Down` raised; upward squeak should be muted.
- Draw a downward `Neck Slide` return with `Squeak Down` at 0% and `Squeak Up` raised; downward squeak should be muted.
