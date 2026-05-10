# 0070 — Performance UI Groups

## Context

After adding `Slide Squeak`, the Performance page had too many always-visible controls and pushed feedback-return character controls below the visible area. Human feedback also clarified that several controls are secondary character tweaks for a primary performance gesture.

## Taxonomy

Primary Performance controls:

- `Fret Pressure`
- `Aftertouch Bend`
- `Neck Slide`
- `Finger Noise`
- `Amp Feedback`

Secondary character/timing controls:

- Under `Neck Slide`: `Fret Steps`, `Slide Lift`, `Slide Squeak`
- Under `Finger Noise`: `Lookahead`
- Under `Amp Feedback`: `Return Clip`

## Implementation

- Added small disclosure buttons to the primary rows that own secondary controls.
- Kept all slider columns aligned, whether a row is primary or secondary.
- Muted the secondary-row labels so they read as character tweaks.
- Left the slide subgroup open by default because it is the current active workflow.
- Collapsed the finger-noise and feedback subgroups by default so `Amp Feedback` remains reachable.
- Changed `Slide Squeak` to 0% to 100%, with a 20% default after human audition found that balance more useful.
- Updated the model label to `StringVoice EG-070 PerformanceUi`.

## Verification Targets

- New plugin instances show `v0.2.6 / StringVoice EG-070 PerformanceUi`.
- Performance page shows primary controls without pushing `Amp Feedback` off the bottom.
- `Neck Slide` disclosure reveals `Fret Steps`, `Slide Lift`, and `Slide Squeak`.
- `Finger Noise` disclosure reveals `Lookahead`.
- `Amp Feedback` disclosure reveals `Return Clip`.
- `Slide Squeak` maxes out at 100% and defaults to 20%.
