# 0076 - Economy Pick Stroke

## Context

Human DAW audition of the plastic-pick work found the `Pick Stroke = Alternate` mode useful, but the right-hand interpretation was still too literal. On a real guitar, repeated notes on one string commonly alternate down/up, but motion across strings often follows the physical travel of the pick: moving from low strings toward high strings naturally continues as downstrokes, while moving back from high strings toward low strings naturally uses upstrokes. This is especially important for strummed or arpeggiated chord shapes and string skips.

The UI should keep the simple `Alternate` name. The smarter behavior is an interpretation detail, not a new playing mode.

## Physical Mental Model

For the current MVP-level player model:

- Increasing string index means moving from low E toward high E and resolves to a downstroke.
- Decreasing string index means moving from high E toward low E and resolves to an upstroke.
- Repeated picked notes on the same string alternate against the previous picked stroke.
- Hammer-ons, pull-offs, and taps are left-hand gestures. They do not consume or advance the pick-stroke memory.
- Forced `Down` and `Up` modes still mean exactly that, while also updating the last picked string so returning to `Alternate` starts from the latest right-hand context after an explicit mode change reset.

This is closer to economy picking than a pure metronomic alternation, but it remains exposed as `Alternate` because that is the user-facing intent.

## Implemented Scope

EG-074 implements:

- `Pick Stroke = Alternate` now compares the current `FretboardAssignment::stringIndex` to the previous picked string.
- Low-to-high string travel chooses downstroke, including string skips.
- High-to-low string travel chooses upstroke, including string skips.
- Same-string picked repeats alternate from the previous picked stroke.
- Non-picked gestures return a neutral stroke direction and do not update pick memory.
- `reset()` and switching into `Alternate` clear the remembered string and restore the default first picked attack to downstroke.
- The Articulation-page `Pick Stroke` info note now explains the economy-aware interpretation.
- Model label updated to `StringVoice EG-074 EconomyPickStroke`.

## Verification

Local checks run on 2026-05-10:

- `scripts/install-vst3.sh --build`
  - Build succeeded.
  - Installed VST3 to `~/Library/Audio/Plug-Ins/VST3/Guitar AG.vst3`.
  - Existing JUCE `createWriterFor` deprecation warning remains in `tools/render-calibration.cpp`.
- Generated temporary MIDI fixtures:
  - `/private/tmp/guitar-ag-economy-ascending.mid`
  - `/private/tmp/guitar-ag-economy-repeated.mid`
- Rendered comparison files:
  - ascending string run with `--pick-stroke alternate`
  - ascending string run with `--pick-stroke down`
  - repeated same-string notes with `--pick-stroke alternate`
  - repeated same-string notes with `--pick-stroke down`
- Comparison results:
  - ascending alternate vs forced down: byte-identical, relative RMS difference `0.0`
  - repeated same-string alternate vs forced down: not byte-identical, relative RMS difference `0.388409`
- Larger audition renders completed:
  - `tests/midi/guitar-ag-player-articulation-audition.mid`: `36.758x` realtime, max 4 string voices
  - `tests/midi/guitar-ag-feature-audition.mid`: `30.977x` realtime, max 6 string voices

## Human Audition Targets

- With `Pick Stroke = Alternate`, play ascending string runs and strummed/arpeggiated low-to-high chord fragments; they should feel like continuous downstroke travel rather than strict down/up/down/up.
- Play descending high-to-low fragments; they should favor upstroke travel.
- Play repeated notes on a single string; they should still alternate.
- Check string skips, especially low E to D/G/B and high E back to G/D/A.
- Confirm hammer-ons, pull-offs, and taps do not audibly disturb the pick alternation pattern.

## Deferred

- Explicit right-hand sweep/economy UI modes.
- Pick-hand position, escape motion, and rest-stroke/free-stroke modeling.
- Multi-note strum grouping that treats near-simultaneous notes as one continuous hand stroke.
- Learned or notation-driven picking directions beyond the current string-travel heuristic.
