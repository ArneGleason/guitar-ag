# Plan 0005 — Build Identity and Pick Excitation Polish

## Goal

Make plugin builds easier to identify in a DAW and improve the first string voice attack so it reads more like a picked string and less like a hard tap.

## Scope

- Show plugin version/build identity in the editor.
- Include the current Git short commit in builds where Git is available.
- Improve the install script output so it reports what was installed.
- Refine the string excitation model while keeping the implementation small and fixed.
- Capture listening feedback in `docs/dsp-notes.md`.

## Out of Scope

- Do not implement MPE.
- Do not implement pickup modeling.
- Do not add fret interaction, hammer-ons, pull-offs, or slides.
- Do not add a full UI or editable parameters.
- Do not add neural or data-driven synthesis.

## Implementation Notes

The attack should move away from mostly-noise excitation and toward a shaped displacement/release with a smaller pick transient.

Keep the build self-identifying so Bitwig plugin cache issues are easier to spot.

## Verification

The human should be able to verify:

- [ ] The plugin editor displays version/build information.
- [ ] The install script reports the installed build.
- [ ] MIDI notes still trigger a plucked string voice.
- [ ] Velocity still changes intensity and brightness.
- [ ] The attack is less like a hard tap than Plan 0002.
- [ ] The project builds and installs successfully.

## Completion Notes

2026-04-25:

- Added generated build info with plugin version, model label, and Git short commit.
- Displayed the build identity in the plugin editor.
- Updated the install script to print the installed configuration and commit.
- Changed the string excitation from mostly noise to a displacement-led shape with localized scrape noise and a short pick-release transient.
- Documented Bitwig duplicate/stale plugin behavior and listening feedback.
