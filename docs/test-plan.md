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
- [ ] Tab buttons switch between Setup, Pickup, Perform, Vibrato, MPE, Whammy, and Artic pages.
- [ ] The editor fits comfortably on screen without excessive vertical scrolling or clipping.
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
- [ ] `MPE Bend Range` defaults to ±48 semitones.
- [ ] Bitwig's note expression pitch-bend range matches the plugin value.
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

## Lookahead Finger Noise Checks

- [ ] `Lookahead` Off feels immediate for live playing.
- [ ] `Lookahead` 150 ms and 250 ms report latency to the DAW.
- [ ] In a DAW render, compensated note onsets still line up with the MIDI grid.
- [ ] `Finger Noise` at 0% is silent or effectively inaudible.
- [ ] `Finger Noise` at high values creates audible approach/release noises before notes when lookahead is enabled.
- [ ] Changing `Lookahead` may require transport restart or plugin reload in some hosts; verify the host updates latency compensation.

## Finger Vibrato Checks

- [ ] `Vibrato Depth` at 0 cents leaves notes unmodulated.
- [ ] Raising `Vibrato Depth` creates audible pitch vibrato.
- [ ] `Vibrato Speed` changes the vibrato rate.
- [ ] `Vibrato Delay` waits before vibrato starts, then ramps in smoothly.
- [ ] `Mod Wheel To Depth` lets MIDI CC1 increase vibrato depth from the slider baseline.
- [ ] `Mod Wheel To Speed` lets MIDI CC1 increase vibrato speed from the slider baseline.
- [ ] Header/version text remains readable and does not overlap other UI text.

## Whammy Pitch Wheel Checks

- [ ] `Pitch Wheel Whammy` Off leaves pitch wheel movement inactive.
- [ ] `Pitch Wheel Whammy` On bends all active notes globally.
- [ ] `Whammy Up Range` defaults to +6 semitones.
- [ ] `Whammy Down Range` defaults to -12 semitones.
- [ ] `Whammy String Spread` at 0% bends chord tones in parallel.
- [ ] Raising `Whammy String Spread` makes wide chord bends slightly imperfect across strings.
- [ ] Returning the pitch wheel to center settles smoothly without clicks or stuck detune.

## Per-Note Aftertouch Checks

- [ ] `Aftertouch Bend` defaults to +2 semitones.
- [ ] `Aftertouch Bend` at 0 semitones leaves key/poly aftertouch inactive.
- [ ] Pressing into a held note with key/poly aftertouch bends that note smoothly upward.
- [ ] In a chord, key/poly aftertouch on one note does not bend the other held notes.
- [ ] Negative `Aftertouch Bend` values bend downward if deliberately selected.
- [ ] Channel pressure no longer drives pitch bend directly; it uses the MPE pressure/tone mapping.

## MPE Pitch Bend Checks

- [ ] `MPE Mode` Off keeps pitch wheel operating as global whammy.
- [ ] `MPE Mode` On makes pitch wheel act per MIDI channel.
- [ ] In Bitwig, set the instrument and note-expression bend range to 48 semitones.
- [ ] Play a chord with notes assigned to separate MPE channels.
- [ ] Draw or perform a pitch bend on one note only.
- [ ] Only that note bends; the other held notes remain at their original pitch.
- [ ] Reducing `MPE Bend Range` requires the DAW-side range to be reduced to match.

## MPE Pressure And CC74 Checks

- [ ] `MPE Pressure Amount` at 0% disables channel-pressure tone/sustain response.
- [ ] Raising `MPE Pressure Amount` makes channel pressure add sustain, level, and upper-mode intensity.
- [ ] `MPE CC74 Amount` at 0% disables CC74/timbre response.
- [ ] Raising `MPE CC74 Amount` makes CC74 brighten/shift only the matching voice.
- [ ] In an MPE chord, pressure on one member channel changes only the note on that channel.
- [ ] In an MPE chord, CC74 on one member channel changes only the note on that channel.
- [ ] In normal MIDI mode, channel pressure and CC74 remain playable as channel-scoped/global expression when all notes share one channel.

## Feature Audition MIDI Checks

- [ ] Drag `tests/midi/guitar-ag-feature-audition.mid` into a DAW instrument track using Guitar AG.
- [ ] Enable `MPE Mode` and leave `MPE Bend Range` at `48.0 st`.
- [ ] Confirm the open-string and chord sections sound like the current expected clean-DI model.
- [ ] Confirm the short riff releases cleanly.
- [ ] If `Mod Wheel To Depth` or `Mod Wheel To Speed` is enabled, confirm the CC1 segment adds vibrato.
- [ ] Confirm the key/poly aftertouch segment bends only the chord's top note.
- [ ] Confirm the MPE pitch bend segment bends only the middle note of the held chord.
- [ ] Confirm the MPE pressure and CC74 segment changes held-note tone/sustain without changing every note at once.
