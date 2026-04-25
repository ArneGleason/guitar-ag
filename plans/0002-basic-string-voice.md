# Plan 0002 — Basic Physical String Voice

## Goal

Replace the placeholder test tone with one basic modeled string voice.

The string does not need to be perfectly realistic yet. It should be playable, stable, and clearly plucked rather than sample-based.

## Scope

- Implement one simple Karplus-Strong or digital waveguide-style voice.
- Trigger the voice from MIDI note-on.
- Release or damp the voice from MIDI note-off.
- Map velocity to pluck force.
- Add simple damping/decay controls if practical.
- Keep output level safe.
- Update `docs/dsp-notes.md` with the model approach and any useful parameter ranges.

## Out of Scope

- Do not implement full MPE yet.
- Do not implement pickup modeling yet unless a tiny filter is needed for usability.
- Do not implement a full guitar body model.
- Do not implement six-string guitar assignment logic.

## Implementation Notes

The model should be small and easy to reason about.

Important concerns:

- Avoid clicks at note start/release where practical.
- Avoid unstable feedback.
- Avoid runaway output levels.
- Avoid allocations on the audio thread.

## Verification

The human should be able to verify:

- [ ] MIDI notes trigger plucked-string-like sounds.
- [ ] Higher notes have higher pitch.
- [ ] Velocity changes attack/intensity.
- [ ] Notes decay naturally.
- [ ] No obvious crashes during repeated notes.
- [ ] No extreme clipping at default settings.
- [ ] `docs/dsp-notes.md` has been updated.

## Completion Notes

Fill this in after implementation.
