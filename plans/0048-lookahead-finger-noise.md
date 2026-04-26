# 0048 — Lookahead Finger Noise

## Goal

Add optional playback-oriented lookahead so the instrument can create human performance noises that occur before the main MIDI note onset.

## Motivation

Finger approach/release noises are not just note-start transients. A realistic player may touch, slide, scrape, or release a string just before the clean note speaks. To synthesize that from MIDI, the plugin needs a small internal delay so it can output preparation noise before the delayed note while reporting latency to the host.

## Proposed Controls

- `Lookahead`: Off, 150 ms, 250 ms.
- `Finger Noise`: 0-100% amount for pre-note approach and release/fret-contact noise.

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

- Confirm Off mode is bit-identical or audibly unchanged aside from intentional refactors.
- Confirm 150/250 ms modes report latency and keep note onset aligned in Bitwig render.
- Render MIDI with quick position changes and compare finger-noise timing before note attacks.
