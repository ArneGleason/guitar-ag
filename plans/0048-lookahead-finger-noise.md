# 0048 — Lookahead Finger Noise

## Goal

Add optional playback-oriented lookahead so the instrument can create human performance noises that occur before the main MIDI note onset.

## Motivation

Finger approach/release noises are not just note-start transients. A realistic player may touch, slide, scrape, or release a string just before the clean note speaks. To synthesize that from MIDI, the plugin needs a small internal delay so it can output preparation noise before the delayed note while reporting latency to the host.

## Proposed Controls

- `Lookahead`: Off, 150 ms, 250 ms.
- `Finger Noise`: 0-100% amount for pre-note approach and release/fret-contact noise.

## Implemented

- Added `Lookahead` as an APVTS choice parameter.
- Added `Finger Noise` as an APVTS percent parameter.
- Added matching editor controls and info popovers in the `Performance` section.
- Added JUCE latency reporting for 150 ms and 250 ms modes.
- Added a fixed-size MIDI lookahead scheduler in `AudioEngine`.
- Added fixed-size finger-noise voices that render filtered scrape/ridge noise before delayed note-on and note-off events.
- Added offline renderer flags `--lookahead-ms` and `--finger-noise`.

## Proposed Implementation

- Keep `Lookahead` defaulted to Off for live playability.
- When lookahead is enabled, report the matching latency with JUCE `setLatencySamples`.
- Delay note-on and note-off synthesis internally by the chosen lookahead amount.
- Schedule approach noises inside that lookahead window so host latency compensation can align the actual note while preserving pre-note human motion.
- Use the fretboard mapper to estimate movement distance, string changes, fretted/open state, and wound-string amount.
- Start with a fixed-size scheduler and fixed-size finger-noise voices to avoid audio-thread allocation.

## First Sound Model

- Pre-note finger approach: short filtered scrape/noise, stronger for quick moves, lower strings, wound strings, and larger fret/string distance.
- Note release: small damping/squeak component when leaving a fretted note.
- Slides between positions can be added after the first scheduler works.

## Risks

- Some DAWs may not fully update plugin latency until transport restart, plugin reload, or project reload.
- With lookahead enabled, live playing will feel delayed by design.
- MIDI timing and note-off handling must stay exact so existing non-lookahead playback remains unchanged.

## Verification

- Built `GuitarAGOfflineRender`.
- Rendered `build/diagnostics/guitar-ag-eg042-lookahead-off.wav`.
- Rendered `build/diagnostics/guitar-ag-eg042-lookahead-150-finger100.wav`.
- Confirmed the off/on WAV files differ.
- Built `GuitarAG_VST3`.
- Remaining human check: verify Bitwig compensation and subjective usefulness of the noise layer.
