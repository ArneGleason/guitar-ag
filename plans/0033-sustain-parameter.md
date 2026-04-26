# Plan 0033 — Sustain Parameter

## Goal

Expose the KS024 tail sustain behavior as an adjustable VST parameter.

## Scope

- Add a JUCE `AudioProcessorValueTreeState`.
- Add `Sustain` parameter from 0% to 100%.
- Default to 100% to preserve the current liked sound.
- Smooth the value in `AudioEngine`.
- Pass the smoothed value into active voices.
- Save/restore parameter state.
- Add a minimal editor slider.

## Non-Goals

- No broader UI redesign.
- No presets beyond host-managed parameter state.
- No compressor or output gain control.
- No MPE changes.

## Result

Implemented as `StringVoice KS-025 SustainParam`.
