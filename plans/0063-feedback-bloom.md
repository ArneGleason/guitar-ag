# 0063 — Feedback Bloom

## Goal

Make `Amp Feedback` behave more like a guitar near a loud amp by letting new note attacks establish the dry string/chord energy before the feedback loop blooms into the sustain.

## Motivation

Bitwig auditioning showed that high feedback sounded more convincing when the slider was kept near zero during the attack and raised after the note or chord settled. Without that onset behavior, the loop can grab too early and make feedback feel like even string excitation.

## Implementation

- Added a global feedback-bloom envelope inside `AudioEngine`.
- Triggered the bloom duck from MIDI note-ons.
- Made picked notes duck the loop more than hammer-ons, pull-offs, or right-hand taps.
- Treated notes inside a short attack cluster as one duck event so chords do not repeatedly flush the loop.
- Applied the bloom to the effective feedback amount before both string feedback and the global resonator loop.
- Reduced old loop state and cleared string focus on fresh attacks so the next note/chord can become the winner.
- Prevented the resonator bank from listening at full input strength when effective feedback is near zero.

## Verification

- Release build succeeded.
- Feature-audition render at 100% `Amp Feedback`, clean return: 9.749x realtime, max 6 string voices.
- Feature-audition render at 100% `Amp Feedback`, distorted return: 9.663x realtime, max 6 string voices.
- Player-articulation render at 100% `Amp Feedback`, clean return: 23.841x realtime, max 3 string voices.
- Follow-up audition made `Distorted Return` the default for EG-057 because it reduced clean-return chirp and sounded more natural.

## Manual Audition

- Use a held picked chord with `Amp Feedback` high.
- Confirm the attack stays clear before feedback blooms into the sustain.
- Confirm new plugin instances open with `Distorted Return` on.
- Compare `Distorted Return` off/on after the bloom is audible.
- Repeat with a second chord after an existing feedback tail and confirm the new chord can reset the feedback winner.
