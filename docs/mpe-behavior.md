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

Current implementation:

- `MPE Mode` toggles member-channel pitch-wheel routing.
- `MPE Bend Range` defaults to ±48 semitones to match Bitwig's common MPE default.
- `MPE Pressure Amount` scales channel-pressure response for the matching active voice.
- `MPE CC74 Amount` scales CC74/timbre response for the matching active voice.
- In MPE mode, pitch wheel messages on member channels are routed to active voices on the same MIDI channel.
- In lower-zone MPE, pitch wheel on channel 1 is treated as the master/global whammy source when `Pitch Wheel Whammy` is enabled.
- If multiple active voices share one MIDI channel, they will bend together; a DAW must send separate member channels for independent bends.
- In non-MPE mode, pitch wheel remains available as the global whammy-bar control.
- In non-MPE mode, channel pressure and CC74 are applied globally to all active voices and stored as the fallback value for newly started voices.
- Toggling MPE mode clears per-channel pitch bend, pressure, and CC74 state so stale expression does not carry across modes.

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
- The current implementation treats pitch bend as per-channel when `MPE Mode` is enabled.
- Lower-zone master-channel pitch bend on channel 1 feeds the global whammy path instead of per-note bend.
- Upper-zone master-channel behavior, such as channel 16 master messages, is not modeled yet.

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

Current implementation:

- MIDI key/poly aftertouch is routed by note number and channel to matching active voices.
- The `Aftertouch Bend` parameter maps full key/poly aftertouch pressure to a configurable pitch bend, defaulting to +2 semitones.
- In MPE mode, channel pressure is routed by MIDI channel to matching active voices.
- In normal MIDI mode, channel pressure is applied globally to all active voices.
- `MPE Pressure Amount` scales the channel-pressure effect.
- The first pressure mapping is intentionally conservative: it adds sustain, output intensity, and upper-mode emphasis rather than pitch bend.

### CC74 / Timbre

In MPE mode:

- CC74 on a member channel applies only to that channel's active voice.
- `MPE CC74 Amount` scales the response.
- The first CC74 mapping leans the held voice brighter and more bridge-like by emphasizing upper modes and their decay.

In normal MIDI mode:

- CC74 is applied globally to all active voices.

Suggested mappings:

- pluck position
- pick hardness
- brightness
- tone

## Bend Range

The plugin should expose pitch bend range as a parameter.

Current default:

- ±48 semitones, intended to match Bitwig's default MPE bend range.

Possible defaults:

- ±48 semitones for general MPE compatibility
- ±24 semitones for experimental slides
- ±12 semitones for guitar-like slides
- ±2 semitones for traditional guitar bend behavior

## Slide Gesture Planning Note

`plans/0064-slide-gesture-notation.md` treats single-note guitar slides as pitch-motion gestures that can be authored with existing MPE per-note pitch bend curves.

Do not confuse that guitar-slide plan with the MPE dimension that some DAWs label "Slide". In Guitar AG's current mapping, CC74/timbre remains a timbre lane. Guitar slides should use pitch bend or the planned global `Slide Offset` automation lane unless a later explicit mode changes that mapping.

The planned `Slide Offset` lane is guitar-global rather than member-channel MPE: it is intended to move a held chord shape with one automation curve, while member-channel MPE pitch bend remains available for independent per-note detail.

Current implementation:

- The first global slide lane is exposed as `Neck Slide`.
- `Neck Slide` is not an MPE member-channel lane. It applies globally to active voices and is intended for chord-shape slides or broad neck-position throws.
- Member-channel MPE pitch bend still handles independent single-note slides and can be layered with `Neck Slide`.
- The host-facing range is `-24.0 st` to `+24.0 st`.
- The final pitch ratio is clamped before adjusted modal phase steps are calculated, so stacked MPE bend, whammy, vibrato, aftertouch bend, and neck slide remain bounded.

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

### Test 4 — Normal MIDI Pressure And Timbre

1. Disable MPE mode.
2. Play a chord from a normal MIDI keyboard.
3. Send channel pressure or CC74.

Pass condition:
All active chord tones respond together, regardless of the MIDI channel used for the controller.
