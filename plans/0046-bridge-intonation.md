# Plan 0046: Bridge Intonation

## Goal

Add a first setup/intonation imperfection control and keep the growing UI manageable.

## Change

- Label this pass `StringVoice EG-039 BridgeIntonation`.
- Add a `Bridge Intonation` parameter.
- 0% is perfect bridge intonation.
- 100% is noticeably imperfect but still musical.
- Use the fretboard assignment so open strings remain in tune, low frets move very little, and higher frets drift more.
- Apply deterministic per-string saddle offsets so the six strings do not all drift in the same direction.
- Add collapsible editor sections:
  - `Setup`: sustain, string age, bridge intonation
  - `Pickup`: pickup model and position
  - `Articulation`: pick stiffness, pick texture, palm mute, harmonic touch
- Add `--bridge-intonation` to the offline renderer.

## Verification

- Build the offline renderer and VST3.
- Render diagnostics at 0% and 100% bridge intonation.
- Install the VST3 for Bitwig audition.
