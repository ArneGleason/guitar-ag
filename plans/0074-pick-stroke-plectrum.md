# 0074 — Pick Stroke Plectrum

## Context

Human DAW audition found that the slide and chord/strum work is improving, but isolated single-note picking still exposed a synthetic attack. The research pass in `plans/0073-pick-attack-research.md` recommended starting with one believable everyday plastic plectrum before adding finger/nail/material menus.

## Implemented Scope

EG-072 implements the first pass:

- Add a host-facing `Pick Stroke` choice with `Down`, `Up`, and default `Alternate`.
- Resolve the stroke mode in `AudioEngine` and pass a concrete direction into `StringVoice::start`.
- Advance alternating strokes only for picked note starts, not hammer-ons, pull-offs, or right-hand taps.
- Seed each attack deterministically from note, channel, assigned string/fret, render sample time, gesture, stroke direction, and a resettable attack counter.
- Retune the normal plastic-pick region by reducing exposed contact ring/chirp energy, shortening the standalone pick transient, and letting stroke direction shape release polarization and contact balance.
- Add offline-renderer `--pick-stroke down|up|alternate` support.

## Verification

Local checks run on 2026-05-10:

- `scripts/install-vst3.sh --build`
- `GuitarAGOfflineRender` single-note renders for `--pick-stroke down`, `up`, and `alternate`
- Two repeated `--pick-stroke alternate` single-note renders were byte-identical:
  - `/private/tmp/guitar-ag-eg072-alt-a.wav`
  - `/private/tmp/guitar-ag-eg072-alt-b.wav`
- Down/up/alternate renders diverged as expected:
  - down vs up relative RMS diff: `0.366`
  - down vs alternate relative RMS diff: `0.257`
  - up vs alternate relative RMS diff: `0.264`
- Larger audition renders completed successfully:
  - `tests/midi/guitar-ag-player-articulation-audition.mid`: `36.750x` realtime, max 4 string voices
  - `tests/midi/guitar-ag-feature-audition.mid`: `31.052x` realtime, max 6 string voices

## Human Audition Targets

- Confirm the default single-note attack is less like a tonal digital/FM chirp.
- Confirm `Pick Stroke = Alternate` feels natural on repeated picked notes.
- Confirm forced `Down` and `Up` are useful enough to keep as explicit options.
- Confirm strummed chords still retain the convincing staggered engagement that motivated keeping this pass scoped.

## Open Follow-Ups

- The ordinary plectrum model is still a compact approximation, not a beam/contact solver.
- Fingerpicking, fingernail attack, pick thickness/material families, and explicit pick-depth controls remain intentionally deferred.
- If the default attack is still too tonal, the next target is the remaining chirp-mode contribution and/or a more explicit contact/release envelope rather than adding more UI.
