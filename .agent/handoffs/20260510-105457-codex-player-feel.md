# Studio Handoff: Codex Player Feel Implementation

- Timestamp: 2026-05-10T10:54:57-04:00
- Status: review-request
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review EG-075 Player Feel first slice
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

The human requested a first player-feel slice that avoids random DAW-style humanization. The goal is deterministic timing and velocity/energy variation caused by the modeled player attempting the part: cognitive load, dexterity load, and endurance. Load should recover over time, and there should be an immediate reset action for starting a new take.

Please pull `main` from GitHub before reviewing:

```bash
git fetch origin
git switch main
git pull --ff-only origin main
```

## Code Changes

- Changed model label to `StringVoice EG-075 PlayerFeel`.
- Added host/APVTS parameters:
  - `Player Feel`
  - `Feel Recovery`
- Added UI controls on the Articulation page:
  - `Player Feel`
  - `Feel Recovery`
  - `Reset Feel`
- Added `AudioEngine` player-feel state:
  - cognitive load
  - dexterity load
  - endurance
  - separate feel-only `FretboardMapper`
  - deterministic context noise seeded from musical/render state
- Picked note-on events can receive:
  - small positive timing delay
  - incoming velocity scaling
- Added recovery decay and immediate reset.
- Added offline flags:
  - `--player-feel`
  - `--player-feel-recovery`
  - `--player-feel-reset-at`

## Verification Run

- `scripts/install-vst3.sh --build`
  - Build succeeded.
  - Installed VST3 to `~/Library/Audio/Plug-Ins/VST3/Guitar AG.vst3`.
  - Existing JUCE `createWriterFor` deprecation warning remains in `tools/render-calibration.cpp`.
- Pick-stroke audition renders:
  - default render matched explicit `--player-feel 0` byte-for-byte.
  - `--player-feel 1.0 --player-feel-recovery 0.85` differed from neutral.
  - repeating the same `--player-feel 1.0` render produced byte-identical WAVs.
  - `--player-feel-reset-at 45` differed from the non-reset feel render.
- Performance examples:
  - pick-stroke audition, feel 100%: `25.296x` realtime, max 6 string voices.
  - player-articulation audition, feel 50%: `36.550x` realtime, max 4 string voices.

## Review Focus

- Confirm `Player Feel = 0` is neutral.
- Confirm note-on delay and velocity scaling are deterministic and bounded.
- Confirm cognitive/dexterity/endurance loads decay according to `Feel Recovery`.
- Confirm `Reset Feel` is real-time safe and clears all feel-state memory.
- Confirm the separate feel-only `FretboardMapper` does not affect the audio voice assignment path.
- Confirm lookahead interaction is acceptable for the first pass: player-feel delay applies to note-ons only, while lookahead still groups expression events by the existing delay scheduler.

## Known Limitations

- This first pass only delays note starts. Early timing requires a future lookahead-aware model.
- Velocity scaling is applied through incoming note-on velocity, not a separate physical pick-depth parameter yet.
- Re-picked held chord tones are still represented as new note starts; held-shape repick semantics remain a future engine feature.
