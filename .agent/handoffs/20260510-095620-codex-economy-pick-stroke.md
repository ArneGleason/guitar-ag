# Studio Handoff: Codex Economy Pick Stroke

- Timestamp: 2026-05-10T09:56:20-04:00
- Status: review-request
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review EG-074 Economy Pick Stroke
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

The human auditioned the pick-stroke controls and clarified the next right-hand behavior: `Pick Stroke = Alternate` should not blindly toggle on every picked note. It should alternate on repeated same-string notes, but if the line moves across strings, the stroke should follow the efficient direction of pick travel. Low-string to high-string movement should use downstrokes; high-string to low-string movement should use upstrokes. String skips should follow the same travel rule. No new mode should be exposed; keep the simple `Alternate` label.

Please pull `main` from GitHub before reviewing:

```bash
git fetch origin
git switch main
git pull --ff-only origin main
```

## Code Changes

- Updated the model label to `StringVoice EG-074 EconomyPickStroke`.
- Changed `AudioEngine::resolvePickStrokeDirection` to receive the current `FretboardAssignment`.
- Added right-hand memory in `AudioEngine`:
  - `lastPickedStringIndex`
  - `lastPickStrokeDirection`
- Updated `Pick Stroke = Alternate` behavior:
  - first picked note after reset/mode switch defaults to downstroke;
  - increasing string index resolves to downstroke;
  - decreasing string index resolves to upstroke;
  - same-string picked repeats alternate from the previous picked stroke.
- Preserved forced `Down` and `Up` behavior.
- Preserved the rule that hammer-ons, pull-offs, and taps do not consume or update the picked-stroke pattern.
- Reset right-hand memory in `reset()` and when switching into `Alternate`.
- Updated the Articulation-page `Pick Stroke` info note.
- Updated docs/history:
  - `CHANGELOG.md`
  - `DECISIONS.md`
  - `LEARNINGS.md`
  - `docs/dsp-notes.md`
  - `docs/test-plan.md`
  - `docs/build-notes.md`
  - `plans/0076-economy-pick-stroke.md`
  - `.agent/PROJECT_LOG.md`
  - `.agent/REVIEW_QUEUE.md`
  - `.agent/session.json`

## Verification Run

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
- Larger audition renders:
  - `tests/midi/guitar-ag-player-articulation-audition.mid`: `36.758x` realtime, max 4 string voices.
  - `tests/midi/guitar-ag-feature-audition.mid`: `30.977x` realtime, max 6 string voices.

## Review Focus

- Confirm the string-index convention is correct for the existing fretboard mapper:
  - `0 = low E`
  - `5 = high E`
  - increasing index should be treated as low-to-high/downstroke travel.
- Confirm `Alternate` only alternates on same-string picked repeats.
- Confirm cross-string and string-skip picked notes follow travel direction.
- Confirm non-picked gestures do not update `lastPickedStringIndex`, `lastPickStrokeDirection`, or `nextAlternatePickDown`.
- Confirm forced `Down` and `Up` modes still behave exactly as named.
- Confirm reset/mode-switch behavior is deterministic and cannot inherit stale right-hand state.
- Confirm this change is isolated to stroke direction/attack coloring and does not affect MPE, whammy, vibrato, slide squeak, feedback, or fret-slide logic.

## Known Limitations

- This is still a per-note heuristic. It does not yet detect a near-simultaneous strum group and treat that group as one continuous hand stroke.
- It does not model pick depth, pick escape, rest strokes, or explicit notation-supplied stroke marks.
- Human DAW audition should focus on repeated same-string lines, arpeggios, string skips, and low-to-high/high-to-low chord fragments.
