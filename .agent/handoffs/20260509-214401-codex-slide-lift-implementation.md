# Studio Handoff: Codex EG-066 Slide Lift Implementation

- Timestamp: 2026-05-09T21:44:01-04:00
- Status: implemented
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: EG-066 Slide Lift replacement for EG-065 Slide Tail
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

Human Bitwig audition superseded EG-065 `SlideTail`: note-off-based slide tails were not convincing and may have contributed to a loop-boundary CPU spike. Codex replaced the `Slide Tail` mode choice with a continuous `Slide Lift` slider.

## Implementation Summary

- Removed the live `SlideTailMode` enum and note-off tail release path.
- Restored note-off release to the normal `StringVoice::startLeftHandRelease` path.
- Added `Slide Lift` as a 0% to 100% automatable parameter.
- Added `AudioEngine::setSlideLift` and per-sample smoothing.
- Passed `Slide Lift` into `StringVoice::renderSample`.
- Added a motion-driven `slideLiftEnvelope` inside the slide-contact update path.
- `Slide Lift` fades effective `Fret Steps`, adds light lifted string scrape, and adds modal damping while recent `Neck Slide` motion exists.
- 50% lift rises over about 500 ms; 100% lift rises over about 50 ms.
- Replaced offline `--slide-tail` with `--slide-lift`.
- Updated the visible model label to `StringVoice EG-066 SlideLift`.

## Checks Run

- `git diff --check`
- `cmake --build build --config Release --target GuitarAGOfflineRender GuitarAG_VST3`
- Offline no-slide neutrality:
  - `--slide-lift 0` vs `--slide-lift 1` rendered byte-identical when `Neck Slide` stayed at zero.
- Offline moving-slide difference:
  - `--neck-slide 5.3 --neck-slide-at 0.001 --slide-fret-steps 0.25 --slide-lift 0/1` produced different renders.

## Known Limitations

- `Slide Lift` is still an approximation of pressure easing. It does not retune to open strings or update same-string speaking length.
- The offline `--neck-slide-at` hook remains block-granular and is only a calibration helper.

## Suggested Review Focus

- Confirm no live `Slide Tail` parameter, enum, UI control, or note-off tail behavior remains.
- Confirm `Slide Lift` is neutral at 0% and when `Neck Slide` is static.
- Confirm the lift path is isolated from MPE, whammy, vibrato, and aftertouch pitch controls.
- Confirm the UI info text clearly explains the control.
