# 0047 — Fret Pressure and Parameter Info

## Goal

Add lightweight in-plugin explanations for the growing parameter set, then add a first fretting-performance pitch imperfection control.

## Scope

- Add info buttons/tooltips for project intent, parameters, pickup landmarks, and harmonic bands.
- Add a `Performance` editor section.
- Add `Fret Pressure` as an automatable VST parameter.
- Use the current fretboard assignment so the effect only applies to fretted notes, scales more strongly on lower strings, and increases higher up the neck.
- Keep the implementation note-start scoped for now.

## Out of Scope

- Continuous aftertouch-style squeezing of already-ringing notes.
- MPE routing.
- New fret-noise or finger-noise layers.

## Verification

- Build `GuitarAGOfflineRender`.
- Render a high-fret diagnostic MIDI with `--fret-pressure 0.0` and `--fret-pressure 1.0`.
- Build and install the VST3 for DAW audition.
