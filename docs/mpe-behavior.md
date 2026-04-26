# MPE Behavior

## Purpose

MPE support is a core requirement of this project.

The instrument must allow independent per-note expression, especially independent pitch bend for each note.

## Key Requirement

When two notes are held at the same time, bending one note must not bend the other note.

## Terminology

- **MPE**: MIDI Polyphonic Expression.
- **Member channel**: A MIDI channel used for one active note and its expression messages.
- **Master/global channel**: A MIDI channel used for shared/global messages, depending on the MPE zone configuration.
- **CC74 / Timbre**: Commonly used as per-note timbre control in MPE workflows.

## MVP Assumptions

The MVP can start with a practical MPE implementation rather than a complete configurable MPE-zone system.

Default assumptions:

- MPE mode can be toggled on/off.
- Each note-on in MPE mode is associated with its MIDI channel.
- That channel routes pitch bend, pressure, and CC74 to the matching voice.
- Bend range is configurable.

## Required MPE Messages

### Note On

Creates or retriggers a voice.

The voice should remember:

- MIDI note number
- MIDI channel
- velocity
- initial pitch
- active state

### Note Off

Releases the voice associated with the same note/channel pair where possible.

If the same note exists on multiple channels, release only the matching channel's voice.

### Pitch Bend

In MPE mode:

- Pitch bend on a member channel applies only to the voice assigned to that channel.
- It must not affect voices on other channels.

In normal MIDI mode:

- Pitch bend may apply globally to all active voices.

### Channel Pressure / Aftertouch

In MPE mode:

- Pressure on a member channel applies only to that channel's active voice.

Suggested mappings:

- pressure increases sustain
- pressure reduces damping
- pressure brightens tone
- pressure increases pickup intensity

Current pre-MPE behavior:

- MIDI key/poly aftertouch is routed by note number and channel to matching active voices.
- The `Aftertouch Bend` parameter maps full key/poly aftertouch pressure to a configurable pitch bend, defaulting to +2 semitones.
- Channel pressure is not mapped yet.
- This does not replace the planned MPE member-channel pressure routing.

### CC74 / Timbre

In MPE mode:

- CC74 on a member channel applies only to that channel's active voice.

Suggested mappings:

- pluck position
- pick hardness
- brightness
- tone

## Bend Range

The plugin should expose pitch bend range as a parameter.

Possible defaults:

- ±48 semitones for general MPE compatibility
- ±24 semitones for experimental slides
- ±12 semitones for guitar-like slides
- ±2 semitones for traditional guitar bend behavior

## Debugging Requirements

A debug display or logging mode should make MPE easier to verify.

Helpful fields:

- active voice count
- MIDI channel per voice
- note number per voice
- pitch bend value per voice
- pressure value per voice
- CC74 value per voice
- voice state: active / released / stealing / idle

## Manual Test Cases

### Test 1 — Independent Bend

1. Enable MPE mode.
2. Play and hold two notes on separate MPE channels.
3. Bend only one note.
4. Confirm only that note bends.

Pass condition:
Only the intended voice changes pitch.

### Test 2 — Per-Note Pressure

1. Enable MPE mode.
2. Play two notes.
3. Apply pressure to one note.
4. Confirm only that note changes tone or sustain.

Pass condition:
The other note remains unchanged.

### Test 3 — Normal MIDI Fallback

1. Disable MPE mode.
2. Play a chord from a normal MIDI keyboard.
3. Apply pitch bend.

Pass condition:
The plugin remains playable and pitch bend behaves predictably, even if global.
