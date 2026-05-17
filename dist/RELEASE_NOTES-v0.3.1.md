# Guitar AG v0.3.1 - Fretboard Assignment and Diagnostics

This release tightens Guitar AG's performance interpreter around real DAW authoring, especially Bitwig/MPE sessions. It keeps the v0.3.0 slide, strum, pick, and player-feel work, then focuses on making string assignment easier to control, diagnose, and recover.

## Highlights

- Added `Input Octave` so DAWs that label guitar low E as E2 can send the expected visible note names while the engine still uses MIDI E2=40 internally.
- Added auto drop tuning for notes below standard E2; the low string retunes down to the requested note and stays there until reset.
- Improved same-sample chord assignment so partial chords choose a combined guitar grip instead of letting one note greedily trap the next note on an awkward string.
- Fixed mixed MPE chord groups: pitch wheel, CC74, and pressure resets in the same sample no longer disable chord-aware assignment.
- Added `Panic Reset` as both a Setup-page button and an automatable parameter.
- Added a live six-string status strip and copyable JSON assignment diagnostics log.
- Added PolyForm Noncommercial 1.0.0 licensing, notices, and contribution/commercial-use guidance.

## The Bitwig Assignment Fix

The v0.3.1 diagnostic log caught a practical host-ordering case: Bitwig can place member-channel pitch wheel, CC74, and channel-pressure reset messages in the same sample as chord note-ons. Guitar AG previously treated that mixed group as ordinary per-message MIDI, bypassing the chord-aware assignment path. After enough position-memory drift, E2+B2-style parts could collapse onto the low E string.

The engine now dispatches same-sample note-offs and expression messages at the sample boundary, then assigns the remaining simultaneous note-ons as one guitar chord. Bass-register duplicate-note scoring also resists choosing high-fret low-string alternatives when a natural low-fret string is available.

## Manual Audition Notes

- Open a fresh plugin instance and confirm the header shows `v0.3.1 / StringVoice EG-088 MixedMpeChordAssignment`.
- In Bitwig, replay the section that previously broke string assignment and confirm E2+B2 returns to low E plus A-string B.
- Try `Input Octave = DAW E2=52` if the DAW piano roll labels MIDI note 52 as E2.
- Try D2+A2 to confirm the low string auto-retunes for drop-D-style parts.
- Use `Copy Log` after a problem section; incoming MIDI events now preserve before/after occupancy snapshots, and assignment events show preferred/strum strings plus stolen-string details.

## Build

- Version: `0.3.1`
- Model label: `StringVoice EG-088 MixedMpeChordAssignment`
- Tag: `v0.3.1`

## Assets

- `GuitarAG-v0.3.1-Windows-vst3.zip` contains the Windows VST3 bundle.
- `GuitarAG-v0.3.1-macOS-vst3.zip` should be attached after the MacBook Codex builds the macOS VST3 from the same tag.
