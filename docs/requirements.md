# Physical Model Electric Guitar Instrument — Top-Level Requirements

## 1. Product Goal

Build a virtual electric guitar instrument that uses physical modeling rather than sample playback.

The instrument should respond like a playable guitar-style synth: each note should behave like an independently vibrating string, with expressive control over pitch bend, pluck behavior, damping, muting, and tone.

The primary performance control should support **MPE — MIDI Polyphonic Expression** — so individual notes can bend, slide, and change expression independently.

## 2. Platform Requirements

The instrument should be built as a real-time audio plugin.

### Required MVP target

- VST3 plugin.
- Built in C++ using JUCE, or another suitable real-time audio plugin framework.
- Runs in common DAWs such as Bitwig, Ableton Live, Reaper, and similar hosts.
- macOS and Windows support are desirable, but the first implementation may target one platform first.

### Preferred architecture

- Audio engine separated from UI code.
- Physical model code separated from MIDI/MPE handling.
- Preset/state system separated from DSP internals.
- Small, testable components rather than one large monolithic plugin.

## 3. Core Sound Engine Requirements

The instrument should synthesize guitar-like sound using a physical model.

### Required MVP behavior

- Generate sound from modeled vibrating strings.
- Support multiple simultaneous notes.
- Each active note should have its own independent string voice.
- Each voice should support:
  - pitch
  - velocity
  - pluck intensity
  - damping
  - decay
  - pitch bend
  - pressure/expression modulation
  - release behavior

### Recommended first model

Start with a **digital waveguide / Karplus-Strong-style string model**, then improve from there.

The first version does not need to be a perfect scientific model. It should be musically playable, stable, and expressive.

### Future physical-model extensions

Later versions may add:

- string stiffness / inharmonicity
- fret interaction
- fret buzz
- sympathetic resonance
- bridge/body coupling
- nonlinear string behavior
- pick direction
- pick hardness
- palm mute modeling
- per-string calibration
- string gauge and material controls

## 4. Electric Guitar Tone Requirements

The instrument should sound like an **electric guitar source**, not an acoustic guitar.

### MVP electric guitar model

The plugin should include a simple electric guitar signal path:

1. Physical string model
2. Pickup model
3. Basic guitar controls
4. Optional simple output shaping

### Required controls

- pickup position
- pickup blend or pickup type
- tone control
- volume/output gain
- string damping / sustain
- brightness
- pluck position
- pick hardness

### Pickup modeling goals

The pickup model should approximate how different pickup positions emphasize or cancel harmonics.

For MVP, this can be simplified to filtering and harmonic shaping rather than a deep electromagnetic simulation.

### Non-goal for MVP

Do not build a full amp/cab/effects suite in the first pass. The plugin should output a good DI-style electric guitar tone that can be sent into existing amp sims.

## 5. MPE Control Requirements

The plugin must support **MPE as a primary input mode**.

This is central to the project.

### Required MPE behavior

Each note should be assigned to its own expression channel/voice so that the following controls can operate independently per note:

- per-note pitch bend
- per-note pressure/aftertouch
- per-note timbre, commonly CC74
- note velocity
- release velocity, if available

### Pitch bend

- Each active note must be able to bend independently.
- Pitch bend range should be configurable.
- Default MPE bend range should support wide bends, for example ±48 semitones, but guitar-focused presets may use smaller ranges such as ±2, ±12, or ±24.

### Expression mapping

Default mappings should be:

- velocity → pluck force
- per-note pitch bend → string pitch
- pressure → sustain, damping, brightness, or pickup intensity
- CC74 / timbre → pluck position, tone, brightness, or pick hardness
- release velocity → release damping or finger lift noise, if implemented

### Fallback MIDI mode

The plugin should also work with normal MIDI input.

In non-MPE mode:

- pitch bend may apply globally
- aftertouch may apply globally
- polyphonic expression may be limited
- the instrument should still be playable from a normal keyboard

## 6. Guitar-Style Voice Allocation Requirements

The instrument should behave more like a guitar than a generic synth where practical.

### MVP voice behavior

- Each incoming note creates an independent modeled string voice.
- Voices decay naturally unless muted or released.
- Re-triggering the same pitch should create a believable new pluck.
- Voice stealing should avoid clicks and abrupt artifacts.

### Future guitar-specific behavior

Later versions may include:

- six-string mode
- automatic string assignment
- per-string pitch ranges
- repeated note behavior on the same string
- legato slides
- hammer-ons
- pull-offs
- muted notes
- strums
- chord voicing logic
- alternate tunings

For the first build, do **not** require perfect six-string guitar logic. Start with expressive polyphonic strings, then add guitar-specific constraints later.

## 7. Performance and Stability Requirements

The plugin must be suitable for real-time use.

### Required

- Low-latency playback.
- No audio-thread memory allocation during performance.
- No blocking calls on the audio thread.
- No UI operations on the audio thread.
- Stable behavior at common sample rates:
  - 44.1 kHz
  - 48 kHz
  - 96 kHz if practical
- Smooth parameter changes without clicks.
- Denormal protection.
- Sensible CPU usage with multiple simultaneous voices.

### MVP performance target

The first usable version should comfortably handle:

- at least 6 active voices
- ideally 12–24 active voices
- real-time MPE control without zipper noise

## 8. User Interface Requirements

The UI should expose the instrument as a playable guitar model, not as a scientific simulator.

### MVP UI sections

- String / Pluck
- Pickup / Tone
- Expression Mapping
- MPE Settings
- Output

### Example controls

**String / Pluck**

- pluck force
- pluck position
- pick hardness
- sustain
- damping
- brightness

**Pickup / Tone**

- pickup position
- pickup type or blend
- tone
- output gain

**Expression Mapping**

- velocity amount
- pressure amount
- CC74/timbre amount
- bend range

**MPE Settings**

- MPE on/off
- pitch bend range
- fallback MIDI mode
- expression smoothing

## 9. Preset Requirements

The plugin should support saving and recalling presets.

### MVP preset examples

Include a few simple starting presets:

- Clean Electric Pluck
- Bright Bridge Pickup
- Warm Neck Pickup
- Muted Palm Style
- Long Sustain
- MPE Lead Guitar
- Wide Bend Experimental

Presets should save:

- physical model parameters
- tone/pickup settings
- MPE settings
- expression mappings
- output level

## 10. Testing Requirements

The project should include simple tests or debug tools that make it easier to verify the audio engine.

### Required checks

- note on/off works
- multiple notes play independently
- per-note pitch bend affects only the intended note in MPE mode
- global pitch bend works in normal MIDI mode
- pressure/CC74 changes affect the correct voice
- no clicks on note start/release
- no crashes when many notes are triggered quickly
- plugin state saves and reloads correctly

### Helpful debug mode

Add an optional developer/debug view showing:

- active voices
- assigned MIDI channels
- current pitch bend per voice
- pressure value per voice
- CC74/timbre value per voice
- voice age / release state

This would make MPE bugs much easier to diagnose.

## 11. MVP Scope

### MVP should include

- VST3 plugin shell
- basic UI
- physical string voice
- polyphonic note playback
- MPE input parsing
- independent per-note pitch bend
- basic pickup/tone shaping
- preset save/load
- a small set of test presets

### MVP should not include yet

- full amp simulation
- cabinet simulation
- realistic fretboard UI
- automatic chord/strum engine
- full six-string fingering logic
- complex fret buzz
- sample import
- audio-to-MIDI
- standalone app unless easy to generate from the same codebase

## 12. Suggested Implementation Order

1. Create a basic JUCE VST3 plugin that outputs sound.
2. Implement one physical string voice.
3. Add polyphony.
4. Add normal MIDI note handling.
5. Add MPE parsing and voice-channel tracking.
6. Verify independent per-note pitch bend.
7. Add pressure and CC74 mapping.
8. Add pickup/tone shaping.
9. Add UI controls.
10. Add presets and state saving.
11. Tune the model for musical playability.

## 13. Core Acceptance Criteria

The build can be considered a successful MVP when:

- The plugin loads in a DAW as a VST3.
- It can play multiple notes at once.
- Notes are generated by a physical/string model, not samples.
- In MPE mode, one held note can bend while another held note stays fixed.
- Per-note pressure or timbre can change the sound of only the intended note.
- The sound is recognizably electric-guitar-like when sent through an amp sim.
- The plugin does not crash during normal DAW use.
- Parameters and presets recall correctly after saving/reopening a session.
