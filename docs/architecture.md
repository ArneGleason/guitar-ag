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

### Tone / Pickup Model

Responsible for electric guitar-like shaping after the string model.

MVP may use:

- pickup-position harmonic shaping
- simple filters
- tone control
- output gain

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
