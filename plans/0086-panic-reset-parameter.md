# Plan 0086 — Panic Reset Parameter

## Goal

Give the human a direct recovery and diagnostic control inside the VST for stuck assignment or voice-state cases.

The control must be available in two ways:

- a visible plugin UI button that can be clicked immediately;
- an automatable host parameter that can be pulsed in Bitwig automation.

## Implementation

- Add a `Panic Reset` APVTS parameter.
- Trigger panic cleanup when the parameter crosses upward past 50%.
- Add a Setup-page `Panic Reset` button that requests the same cleanup path.
- Reuse the EG-085 panic cleanup path so the reset clears voices, queued notes, fretboard occupancy, articulation memory, finger-noise state, feedback focus, controller state, and player-feel memory.

## Verification

- New instances show `StringVoice EG-086 PanicReset`.
- The Setup page shows a `Panic Reset` button.
- The host exposes a `Panic Reset` parameter that can be automated as a short 0% to 100% pulse.
- Existing E2+B2 regression and panic fixtures still render with max 2 active string voices.
