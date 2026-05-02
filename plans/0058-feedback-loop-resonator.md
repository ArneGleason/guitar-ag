# Plan 0058 — Feedback Loop Resonator

## Goal

Make `Amp Feedback` behave less like even string sustain and more like an amp/speaker feedback loop where one resonance can build, dominate, and later hand off.

## Hypothesis

The first feedback pass was too local to each voice. A more realistic approximation needs global loop memory after pickup/tone shaping, plus a dominant band that feeds back into matching modal frequencies.

## Scope

- Keep the public control as `Amp Feedback`.
- Add a small global resonator bank in `AudioEngine`.
- Track per-band envelopes and select a dominant band with hysteresis.
- Pass dominant loop frequency, amount, and signal into `StringVoice`.
- Reduce the older even per-note harmonic lift as the loop takes over.
- Verify long-note and phrase renders for level safety.

## Non-Goals

- No full amp, cabinet, microphone, room, or guitar-body simulation.
- No idle open-string auto-triggering with no MIDI voice yet.
- No cross-string bridge/body coupling network yet.

## Result

- Updated model label to `StringVoice EG-050 FeedbackLoop`.
- Added eight fixed loop bands from low-mid to upper-mid feedback regions.
- The loop listens after pickup/tone processing and feeds back into modal components near the winning absolute frequency.
- Long-note diagnostics stayed below clipping while showing stronger high-feedback tail energy than the 0% render.
