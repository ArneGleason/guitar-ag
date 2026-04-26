# Plan 0028 — First Fretboard Mapper

## Goal

Add a small real-time-safe string/fret assignment layer so the current string model can choose wound or plain behavior from a plausible guitar string, not only from the MIDI pitch.

## Scope

- Standard tuning only: E2, A2, D3, G3, B3, E4.
- Six-string, 24-fret range.
- Bias toward first/second position.
- Track occupied strings so chord notes prefer separate strings.
- Keep a small hand-position memory that moves up faster than it returns down.

## Non-Goals

- No alternate tunings yet.
- No full phrase dynamic programming.
- No finger assignment.
- No pick direction, legato, slides, bends, or MPE routing.
- No UI controls for playing style yet.

## Result

Implemented `FretboardMapper` in `src/dsp/` and wired it through `AudioEngine`.

`StringVoice KS-020 FretboardMap` preserves the KS019 sound model while assigning wound/plain behavior from the selected string class.

## Verification

- Built `GuitarAGOfflineRender`.
- Built `GuitarAG_VST3`.
- Rendered the velocity ladder through the offline renderer.
- Installed the VST3 with `scripts/install-vst3.sh --build`.
