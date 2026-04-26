# Plan 0051 — Per-Note Aftertouch Bend

## Goal

Add a small key/poly aftertouch mapping so keyboard pressure can bend individual held notes.

## Scope

- Add an `Aftertouch Bend` VST parameter.
- Route MIDI key/poly aftertouch by note number and channel to matching active voices.
- Smooth pressure changes inside each voice.
- Allow positive and negative bend ranges, defaulting to +2 semitones.
- Extend the offline renderer with diagnostic aftertouch flags.

## Out of Scope

- Full MPE mode.
- Channel pressure mapping.
- CC74/timbre routing.
- Pressure-to-brightness, sustain, damping, or pickup intensity.

## Verification

- Build `GuitarAGOfflineRender`.
- Build `GuitarAG_VST3`.
- Render a baseline file and an aftertouch-injected file and confirm they differ.
- Install the VST3 bundle for Bitwig auditioning.
