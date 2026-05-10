# 0071 — Slide Squeak Direction

## Context

Human DAW listening after EG-070 found that the slide squeak balance was useful, but the audible control behavior did not feel symmetric: upward slide squeak was easy to balance, while downward slide returns needed their own volume control.

The slide contact model already knows movement direction through `Neck Slide` deltas inside `StringVoice::updateSlideFretContact`, so this can be handled as a small control split rather than a new sound model.

## Implementation

- Preserve the existing `slideSqueak` parameter ID and rename its host-facing label to `Slide Squeak Up`.
- Add a new `slideSqueakDown` parameter with the host-facing label `Slide Squeak Down`.
- Show the two controls as `Squeak Up` and `Squeak Down` under the `Neck Slide` disclosure.
- Track the most recent nonzero slide direction inside `StringVoice` so the short decaying squeak/scrape tail keeps the direction of the motion that generated it.
- Keep both controls in the 0% to 100% range with 20% defaults.
- Keep `--slide-squeak` as the legacy/upward offline-renderer flag and add `--slide-squeak-up` plus `--slide-squeak-down`.
- Update the model label to `StringVoice EG-071 SlideSqueakDirection`.

## Verification Targets

- New plugin instances show `v0.2.6 / StringVoice EG-071 SlideSqueakDirection`.
- `Squeak Up` controls upward `Neck Slide` contact noise without changing downward returns.
- `Squeak Down` controls downward `Neck Slide` contact noise without changing upward throws.
- Both controls remain scoped to slide contact squeak/scrape and do not alter pitch, `Fret Steps`, `Slide Lift`, MPE, whammy, or the older `Finger Noise` layer.
- Existing sessions that referenced the previous `slideSqueak` parameter recall that value as the upward squeak amount.

## Local Checks

- `git diff --check` passed.
- `scripts/install-vst3.sh --build` passed and installed the VST3 locally.
- Offline directional smoke renders confirmed the active direction control changes the render while the inactive direction control is sample-identical for that motion direction.
