# Plan 0003 — MPE Voice Routing and Independent Pitch Bend

## Goal

Add the first working MPE behavior: independent per-note pitch bend.

This is the most important proof-of-concept milestone.

## Scope

- Add MPE mode.
- Track MIDI channel per active voice.
- Route pitch bend to the voice assigned to that MIDI channel.
- Keep normal MIDI fallback behavior working.
- Add a configurable pitch bend range if practical.
- Add basic debug visibility for active voices/channels if practical.
- Update `docs/mpe-behavior.md` with the actual behavior implemented.

## Out of Scope

- Do not implement every possible MPE-zone edge case yet.
- Do not add complex guitar fingering logic.
- Do not add pressure/CC74 mapping unless it is small and safe to include after bend works.
- Do not redesign the sound engine unless absolutely necessary.

## Implementation Notes

Pass condition:

> Two notes can be held at the same time, and bending one note does not bend the other.

Voice identity should be based on note/channel in MPE mode, not only note number.

Pitch changes must be smoothed enough to avoid obvious zipper noise or clicks.

## Verification

The human should be able to verify:

- [ ] MPE mode can be enabled.
- [ ] Two held notes can exist on separate channels.
- [ ] Pitch bend on one channel affects only the matching voice.
- [ ] The other voice remains at its original pitch.
- [ ] Normal MIDI mode still plays chords.
- [ ] `docs/mpe-behavior.md` has been updated with actual behavior.

## Completion Notes

Fill this in after implementation.
