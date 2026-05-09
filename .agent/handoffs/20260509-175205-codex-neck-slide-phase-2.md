# Studio Handoff: Codex Neck Slide Phase 2 Implementation

- Timestamp: 2026-05-09T17:52:05-04:00
- Status: handoff
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review Phase 2 global Neck Slide implementation
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

Antigravity reviewed Phase 1 and cleared Codex to proceed with Phase 2, with these adjustments:

- Use `Neck Slide` as the host-facing parameter name.
- Use a `-24.0 st` to `+24.0 st` range.
- Clamp stacked pitch motion so combined Neck Slide, MPE bend, whammy, vibrato, and aftertouch bend stay bounded.

Codex implemented the first playback lane. This is still pitch-motion only; physical slide scrape and same-string speaking-length changes remain future phases.

## Pull Instructions

Before reviewing, pull the latest `main` from GitHub:

```bash
git fetch origin
git switch main
git pull --ff-only origin main
```

## Files To Review

- `CMakeLists.txt`
- `src/plugin/PluginProcessor.h`
- `src/plugin/PluginProcessor.cpp`
- `src/plugin/PluginEditor.h`
- `src/plugin/PluginEditor.cpp`
- `src/dsp/AudioEngine.h`
- `src/dsp/AudioEngine.cpp`
- `src/dsp/StringVoice.h`
- `src/dsp/StringVoice.cpp`
- `tools/render-calibration.cpp`
- `scripts/create-slide-gesture-midi.py`
- `docs/dsp-notes.md`
- `docs/mpe-behavior.md`
- `docs/audition-midi.md`
- `plans/0064-slide-gesture-notation.md`

## Implemented

- Added an automatable VST parameter named `Neck Slide`.
- Parameter range: `-24.0 st` to `+24.0 st`.
- Added `AudioEngine::setNeckSlideSemitones`.
- Smoothed the value in `AudioEngine` and passed it to all active voices per sample.
- Layered neck slide into `StringVoice::updatePitchRatio`.
- Clamped final aggregate pitch ratio against the active voice base frequency.
- Clamped adjusted per-mode pitch steps to the safe modal frequency range before sine/cosine cache updates.
- Added `--neck-slide` to `GuitarAGOfflineRender`.
- Changed the visible model label to `StringVoice EG-062 NeckSlide`.
- Added a clean held-chord automation bed to the slide audition MIDI for DAW-side `Neck Slide` automation.

## Checks Run

- Built `GuitarAGOfflineRender`.
- Built `GuitarAG_VST3`.
- Installed the built VST3 to the normal user VST3 folder with `scripts/install-vst3.sh --build`.
- Regenerated `tests/midi/guitar-ag-slide-gesture-audition.mid`.
- Parsed the regenerated MIDI with `scripts/extract-midi-notes.py`.
- Rendered the slide audition with `--neck-slide 0`.
- Rendered the slide audition with no `--neck-slide` flag and confirmed it matched `--neck-slide 0` byte-for-byte.
- Rendered the slide audition with `--neck-slide 12` and confirmed the output changed.
- Rendered an extreme stacked pitch case using `--neck-slide 24`, full pitch wheel whammy, and full aftertouch bend to exercise the clamp path.
- Ran `git diff --check`.

## Checks Not Run

- No DAW/VST3 host automation test yet.
- No manual listening check yet.

## Review Focus

- Confirm parameter/state/UI wiring is correct and real-time safe.
- Confirm `0.0 st` is behavior-neutral.
- Confirm the pitch-ratio and per-mode clamp approach is safe and musically reasonable.
- Confirm the slide audition automation bed is useful for DAW testing.
- Confirm it is acceptable that Phase 2 is pitch-motion only, with physical slide scrape and same-string state preservation left for future phases.

## Suggested Next Routing

If accepted, route to the human operator for DAW/VST3 testing with `tests/midi/guitar-ag-slide-gesture-audition.mid`, drawing `Neck Slide` automation over the held-chord automation bed.
