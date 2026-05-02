# Plan 0057 — Amp Feedback

## Goal

Add a simple one-knob approximation of a guitar being brought closer to a loud amp speaker.

The control should cover subtle loud-rig sustain through obvious harmonic feedback, while staying bounded enough for clean DI rendering and host automation.

## Scope

- Add an `Amp Feedback` parameter and Performance-page slider.
- Keep the model inside the existing physical voice path rather than adding an amp/cab/mic simulation.
- Inject feedback into active string voices before pickup/tone shaping.
- Expose the control in the offline renderer as `--amp-feedback`.
- Verify with both phrase/arpeggio MIDI and a long single-note stress test.

## Non-Goals

- No full acoustic room feedback loop.
- No modeled guitar-body modal network yet.
- No cross-string resonant coupling in this pass.
- No amp distortion, cabinet, or microphone model.

## Implementation Notes

- Low values lift modal decay slightly for sustain.
- Higher values lock onto harmonically related modal components.
- The top range adds a small direct harmonic howl component.
- Modal amplitude is clamped and long-note render levels are checked to avoid runaway.

## Result

- Added `Amp Feedback`, default 0%.
- Updated model label to `StringVoice EG-049 AmpFeedback`.
- Added offline render flag `--amp-feedback`.
- Added docs in `docs/dsp-notes.md`, `docs/audition-midi.md`, `DECISIONS.md`, and `LEARNINGS.md`.
- Verified Release offline renderer and VST3 builds.
- Rendered A/B diagnostics for player-articulation phrases and single-note feedback stress tests.
