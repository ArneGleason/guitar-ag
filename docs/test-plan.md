# Test Plan

## Purpose

This document defines the basic manual and technical checks needed as the project evolves.

## MVP Test Areas

- Plugin loads in a DAW.
- Plugin produces sound.
- MIDI note on/off works.
- Polyphony works.
- MPE routing works.
- Independent per-note pitch bend works.
- Per-note pressure and CC74 affect the correct voice.
- Parameters change smoothly without clicks.
- State saves and reloads.
- CPU use is reasonable.

## Manual DAW Test Checklist

### Plugin Loading

- [ ] VST3 is visible to the DAW.
- [ ] Plugin opens without crashing.
- [ ] UI opens without crashing.
- [ ] Audio output is silent when no note is playing.

### Basic MIDI

- [ ] Note-on triggers sound.
- [ ] Note-off releases sound.
- [ ] Velocity changes pluck intensity.
- [ ] Chords play polyphonically.
- [ ] Fast repeated notes do not crash or click badly.

### MPE

- [ ] MPE mode can be enabled.
- [ ] Two notes can be held on separate channels.
- [ ] Bending one note does not bend the other.
- [ ] Pressure affects only the intended note.
- [ ] CC74 affects only the intended note.
- [ ] Normal MIDI mode still works when MPE is disabled.

### State and Presets

- [ ] Parameter changes can be saved in the DAW session.
- [ ] Reloading the session restores the same state.
- [ ] Presets can be saved and recalled if preset support exists.

## Suggested Hosts to Try

- Bitwig Studio, especially for MPE behavior.
- Reaper, for general VST3 compatibility.
- Ableton Live, if available.
- Logic/AU later, if AU support is added.

## Audio Quality Checks

- [ ] Note starts are not distractingly clicky.
- [ ] Note releases are not distractingly clicky.
- [ ] Pitch bend is smooth enough to be musical.
- [ ] Output does not clip by default.
- [ ] Tone is useful through an external amp sim.
