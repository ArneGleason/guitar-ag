# Studio Handoff: Codex Pick Stroke Plectrum Implementation

- Timestamp: 2026-05-10T09:13:36-04:00
- Status: review-request
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review EG-072 Pick Stroke plectrum pass
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

The human approved a first implementation pass for one normal plastic plectrum model, with `Pick Stroke` options `Down`, `Up`, and `Alternate`, plus deterministic per-attack variation. The goal is to make isolated single-note picked attacks less synthetic before adding broader pick/finger/nail models.

Please pull `main` from GitHub before reviewing:

```bash
git fetch origin
git switch main
git pull --ff-only origin main
```

## Code Changes

- Added `Pick Stroke` as a host/APVTS choice parameter:
  - `Down`
  - `Up`
  - `Alternate` default
- Added the Articulation-page `Pick Stroke` combo box and info note.
- Added `AudioEngine::setPickStrokeMode`, alternate-stroke state, deterministic attack seeding, and `PickStrokeDirection` handoff into `StringVoice::start`.
- `Alternate` advances only for `PlayerGesture::Picked`; hammer-ons, pull-offs, and right-hand taps do not consume an alternate stroke.
- Re-seeded `StringVoice` before any note-start randomness is consumed.
- Retuned the normal plastic-pick attack by:
  - reducing exposed `pickContactRing` level and decay;
  - reducing/shortening chirp-mode contribution at default texture;
  - shortening/darkening the standalone pick transient;
  - making down/up strokes slightly alter pluck position, polarization, contact scale, and contact carrier scale.
- Added offline-renderer `--pick-stroke down|up|alternate`.
- Updated docs/history:
  - `CHANGELOG.md`
  - `DECISIONS.md`
  - `LEARNINGS.md`
  - `docs/dsp-notes.md`
  - `docs/test-plan.md`
  - `docs/build-notes.md`
  - `plans/0074-pick-stroke-plectrum.md`
  - `.agent/PROJECT_LOG.md`
  - `.agent/REVIEW_QUEUE.md`
  - `.agent/session.json`

## Verification Run

- `scripts/install-vst3.sh --build`
  - Build succeeded.
  - Installed VST3 to `~/Library/Audio/Plug-Ins/VST3/Guitar AG.vst3`.
  - Existing JUCE `createWriterFor` deprecation warning remains in `tools/render-calibration.cpp`.
- Offline single-note renders:
  - `--pick-stroke down`
  - `--pick-stroke up`
  - `--pick-stroke alternate`
  - repeated alternate render
- Determinism:
  - two repeated alternate renders had identical SHA-256 prefixes and zero sample diff.
- Stroke differences:
  - down vs up relative RMS diff: `0.366`
  - down vs alternate relative RMS diff: `0.257`
  - up vs alternate relative RMS diff: `0.264`
- Larger audition renders:
  - `tests/midi/guitar-ag-player-articulation-audition.mid`: `36.750x` realtime, max 4 string voices.
  - `tests/midi/guitar-ag-feature-audition.mid`: `31.052x` realtime, max 6 string voices.

## Review Focus

- Confirm `pickStroke` is wired end-to-end: processor layout, raw parameter, UI combo, `AudioEngine`, `StringVoice`, and offline renderer.
- Confirm the alternate counter advances only on picked note starts.
- Confirm deterministic attack seed reset behavior is safe for offline repeatability and host playback.
- Confirm the plectrum retune is scoped to note-start pick/contact behavior and does not couple into slide squeak, MPE, whammy, vibrato, or feedback.
- Confirm the Articulation page still fits and the info note accurately describes the behavior.

## Known Limitations

- This is still one compact ordinary plastic plectrum approximation, not a full pick beam/contact solver.
- Fingerpicking, fingernail attack, pick material families, and pick-depth UI are deliberately deferred.
- Human DAW audition is still needed to judge whether the isolated single-note attack is musically improved enough.
