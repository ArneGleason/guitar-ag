# Studio Handoff: Codex Plastic Pick Bite Retune

- Timestamp: 2026-05-10T09:36:03-04:00
- Status: review-request
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review EG-073 Plastic Pick Bite and string-age coupling
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

The human auditioned EG-072 and found `Pick Stroke = Alternate` helpful, but the attack still suggested a hard brittle/glassy pick rather than a normal plastic plectrum. They also requested an adjustable control for the strength of the pick-contact effect, while emphasizing that we should keep thinking physically: the pickup hears the string motion caused by pick/string interaction, not an independent pick-noise layer.

Please pull `main` from GitHub before reviewing:

```bash
git fetch origin
git switch main
git pull --ff-only origin main
```

## Code Changes

- Added `Pick Bite` as a host/APVTS parameter and Articulation-page slider.
- Added offline-renderer `--pick-bite`.
- Updated the model label to `StringVoice EG-073 PlasticPickBite`.
- Kept `Pick Stroke` Down/Up/Alternate from EG-072 intact.
- Reinterpreted the adjustment as contact imprint strength, not pick-noise volume:
  - lower values keep mostly string displacement/release;
  - higher values scale edge imprint, short scrape, contact ring, chirp modes, and rough/high-texture contact.
- Reduced brittle/glassy default behavior:
  - rough `highTexture` behavior starts later in the `Pick Texture` range;
  - local pick contact is broader/complier at normal settings;
  - contact ring level, decay, and carrier frequency are reduced;
  - chirp modes are lower, shorter, and more strongly gated by `Pick Bite`.
- Added deterministic pick-depth and pick-angle scatter to vary pluck offset, contact strength, carrier jitter, and attack-mode gain.
- Coupled `String Age` more explicitly into picked attacks:
  - lowers coherent ring/chirp brightness;
  - shortens bright contact decay;
  - lowers the pick-contact scratch high-pass coefficient so aged strings do not keep the same brand-new bright contact fingerprint.
- Updated docs/history:
  - `CHANGELOG.md`
  - `DECISIONS.md`
  - `LEARNINGS.md`
  - `docs/dsp-notes.md`
  - `docs/test-plan.md`
  - `docs/build-notes.md`
  - `plans/0075-plastic-pick-bite.md`
  - `.agent/PROJECT_LOG.md`
  - `.agent/REVIEW_QUEUE.md`
  - `.agent/session.json`

## Verification Run

- `scripts/install-vst3.sh --build`
  - Build succeeded.
  - Installed VST3 to `~/Library/Audio/Plug-Ins/VST3/Guitar AG.vst3`.
  - Existing JUCE `createWriterFor` deprecation warning remains in `tools/render-calibration.cpp`.
- Offline single-note renders:
  - default `--pick-bite 0.5`
  - repeated default render
  - low bite `--pick-bite 0.15`
  - high bite `--pick-bite 1.0`
  - aged strings `--string-age 0.5`
- Determinism:
  - repeated default renders had identical SHA-256 prefixes and zero sample diff.
- First-attack 20 ms relative RMS differences vs default:
  - low bite: `0.0142`
  - high bite: `0.0257`
  - string age 50%: `0.3513`
- Full-render relative RMS differences vs default:
  - low bite: `0.003613`
  - high bite: `0.006481`
  - string age 50%: `0.353870`
- Larger audition renders:
  - `tests/midi/guitar-ag-player-articulation-audition.mid`: `36.688x` realtime, max 4 string voices.
  - `tests/midi/guitar-ag-feature-audition.mid`: `31.037x` realtime, max 6 string voices.

## Review Focus

- Confirm `pickBite` is wired end-to-end: processor layout, raw parameter, UI slider, `AudioEngine`, `StringVoice`, and offline renderer.
- Confirm `Pick Bite` changes contact imprint terms without muting the main string displacement/release.
- Confirm `String Age` darkens pick-contact components in a physically plausible way.
- Confirm the pick-depth/pick-angle variability is deterministic and does not break repeatable offline renders.
- Confirm the EG-073 retune remains scoped to picked attack/contact paths and does not affect slide squeak, MPE, whammy, vibrato, or feedback logic.
- Confirm the UI still fits on the Articulation page and the info note accurately distinguishes contact strength from separate volume.

## Known Limitations

- This remains a compact ordinary plastic plectrum approximation.
- Fingerpicking, fingernail attack, pick material families, explicit pick-depth UI, and a fuller plectrum beam/contact solver are still deferred.
- Human DAW audition is still needed to judge whether the attack now reads more like plastic than glass.
