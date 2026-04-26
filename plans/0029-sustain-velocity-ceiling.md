# Plan 0029 — Sustain and Velocity Ceiling Retune

## Goal

Respond to listening feedback after the first fretboard mapper: keep the mapper, add more DI-style sustain, and restrain the very top of the pick-strike velocity range.

## Scope

- Preserve KS020 fretboard mapping.
- Keep full MIDI velocity useful for loudness.
- Cap strike-character velocity around the previous 67-68% region.
- Lengthen modal, side-cluster, and wound-string mode decay.

## Non-Goals

- No compressor or automatic gain control.
- No pickup redesign.
- No MPE or performance interpreter expansion.
- No UI controls yet.

## Result

Implemented as `StringVoice KS-021 SustainVelCeil`.

The change separates level velocity from strike-character velocity and increases held-note modal sustain.
