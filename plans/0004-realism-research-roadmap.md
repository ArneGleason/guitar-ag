# Plan 0004 — Realism Research and Roadmap

## Goal

Capture a research-backed long-term vision for realistic electric guitar synthesis and guitar-like performance interpretation.

This plan does not implement those features. It gives future milestones a shared target and a set of references.

## Scope

- Collect references for physical string, fret, pick, pickup, and player-interaction modeling.
- Collect references for MIDI-to-guitar interpretation and expressive performance rendering.
- Create a vision document with practical hypotheses and a staged experiment ladder.
- Update architecture notes if a future component boundary becomes clear.

## Out of Scope

- Do not implement the string model.
- Do not implement MPE.
- Do not add neural synthesis code.
- Do not change product requirements unless a contradiction is discovered.

## Implementation Notes

Keep the outcome useful for small future milestones. Prefer hypotheses and experiment ideas over broad promises.

## Verification

The human should be able to verify:

- [ ] `docs/realism-vision.md` exists.
- [ ] It separates the sound model from the player interpretation layer.
- [ ] It includes links to relevant research.
- [ ] It suggests a staged implementation ladder.
- [ ] Existing near-term plans remain small and testable.

## Completion Notes

2026-04-25:

- Added `docs/realism-vision.md`.
- Added a future `Performance Interpreter` boundary to `docs/architecture.md`.
- Added a pointer from `docs/dsp-notes.md`.
