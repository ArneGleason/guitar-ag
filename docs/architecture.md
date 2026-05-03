# Architecture Notes

## Goal

Keep the project organized so the sound engine, MPE handling, plugin wrapper, UI, and state management can evolve without becoming tangled.

## Suggested Component Boundaries

### Plugin Host Layer

Responsible for:

- DAW/plugin lifecycle
- audio callback entry point
- MIDI event ingestion
- plugin parameters
- preset/state save and restore

Should not contain detailed DSP or physical-model logic.

### Audio Engine

Responsible for:

- voice management
- rendering voices into audio buffers
- sample-rate setup
- global output gain
- denormal protection
- smoothing top-level parameters

### String Voice

Responsible for one active modeled string voice.

Should own:

- pitch/frequency state
- excitation/pluck state
- delay line or waveguide state
- damping/decay state
- per-note expression state
- release behavior

### MPE / MIDI Router

Responsible for translating incoming MIDI/MPE messages into voice actions.

Should handle:

- note on/off
- channel-to-voice assignment
- per-note pitch bend
- per-note pressure
- CC74/timbre
- normal MIDI fallback behavior

### Performance Interpreter

Future layer responsible for translating MIDI/MPE note streams into guitar-like gestures before they reach the voice manager.

May eventually handle:

- string and fret choice
- pick direction and pick position
- legato, hammer-on, pull-off, slide, bend, vibrato, and mute decisions
- human-player variation that does not destroy authored MPE detail

The first MVP milestones can bypass this layer, but the architecture should leave room for it.

Current first implementation:

- `FretboardMapper` is a small DSP-side performance-interpreter seed owned by `AudioEngine`.
- It maps MIDI notes to standard-tuned string/fret assignments before voice start.
- `AudioEngine` keeps the core voice pool to six physical string voices.
- If a new note is assigned to a string that is still ringing, that string's existing `StringVoice` is reused for the new note.
- It does not yet choose pick direction, slides, bends, or MPE expression routing.
- The mapper is intentionally heuristic so it can later be replaced by a phrase-aware interpreter without changing the string voice's sound-generation role.

### Tone / Pickup Model

Responsible for electric guitar-like shaping after the string model.

MVP may use:

- pickup-position harmonic shaping
- simple filters
- tone control
- output gain

Current first implementation:

- `StringVoice` owns the current simple pickup readout because the read position depends on the active note's delay length.
- `ElectricGuitarTone` is now a lighter post-voice conditioning stage for high-pass, presence, body, and output shaping.
- It is not yet a full per-string, pickup-width, pickup-switching, or pickup-circuit model.

Future pickup/body microphonics:

- If the model later includes pickup microphonic behavior, body taps, handling thumps, or sound coupled through the guitar body, that should be modeled as an auxiliary non-string pickup/body source.
- This auxiliary source may behave like a seventh rendering voice or bus, but it should not count as a seventh physical string and should not weaken the six-string allocation rule.
- The intended approximation is a simple pickup/body resonance excited by non-string events or global body energy, then mixed into the DI output before or inside the pickup/tone stage.

### UI Layer

Responsible for controls and display only.

Should avoid direct DSP ownership. UI controls should update plugin parameters or safe state objects.

## Real-Time Safety

The audio callback must avoid:

- heap allocation
- file I/O
- locks where practical
- blocking calls
- UI calls
- logging in tight audio paths

## First Architecture Milestone

A minimal build should prove these boundaries without overengineering them:

- Plugin processor exists.
- Audio engine exists.
- One test oscillator or one placeholder voice renders sound.
- MIDI note on/off can trigger sound.
- Project builds cleanly.
