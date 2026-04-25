# Plan 0006 — Left-Hand Release Damping

## Goal

Make note-off behavior feel more like a fretting finger lifting from a guitar string instead of a simple synth release.

Short notes should damp quickly, as if the finger is yanked off and/or followed by hand damping. Longer notes should release more gently.

## Scope

- Track note age per voice.
- On note-off, choose damping behavior based on how long the note was held.
- Keep the implementation inside the string voice for now.
- Update DSP notes with listening feedback and model behavior.
- Build and install for Bitwig verification.

## Out of Scope

- Do not implement full fret interaction.
- Do not implement hammer-ons, pull-offs, slides, or MPE.
- Do not implement pickup modeling.
- Do not add editable UI controls.

## Implementation Notes

This is a pragmatic left-hand damping approximation, not a full physical fret/finger model.

The release model should:

- be fast for very short notes
- transition for medium held notes
- be gentler for sustained chord-like notes
- avoid hard note cutoffs

## Verification

The human should be able to verify:

- [ ] Short notes stop quickly without sounding like a hard gate.
- [ ] Longer notes release more gently than short notes.
- [ ] The plugin still shows the correct build/model label in the editor.
- [ ] The VST3 builds and installs successfully.

## Completion Notes

2026-04-25:

- Added per-voice note age tracking.
- Added duration-sensitive left-hand damping on note-off.
- Updated model label to `StringVoice KS-003`.
- Documented listening feedback around twang/scrape and release behavior.
