# DSP Notes

## Purpose

Use this file to capture physical-modeling ideas, experiments, parameter ranges, artifacts, and sound-design learnings.

## Initial DSP Direction

Start with a simple Karplus-Strong or digital waveguide-style string model.

The first goal is musical, stable, expressive behavior rather than perfect guitar realism.

## 2026-04-25 — First Basic String Voice

The placeholder sine test tone was replaced with a simple Karplus-Strong-style plucked string voice.

Current behavior:

- MIDI note-on fills a fixed-size delay line with a deterministic noise/shape excitation.
- MIDI pitch sets delay length using `sampleRate / frequency`.
- Velocity scales the excitation level.
- The feedback path averages the current and previous sample, then applies damping.
- MIDI note-off switches the voice to stronger damping so the string dies away faster.
- Voice memory is preallocated inside each voice; rendering does not allocate.
- The implementation is intentionally simple and does not yet include fractional delay, dispersion, pickup modeling, MPE pitch bend, fret collision, or guitar-specific articulation.

Initial values:

- maximum delay: 8192 samples per voice
- polyphony: 8 voices
- pluck position used for excitation shape: 0.18 of string length
- base damping: 0.9965
- release damping: 0.985
- output gain: 0.35 per voice before mixing

Expected sound:

This should sound like a basic plucked physical string rather than a sine tone. It may still sound synthetic or generic; that is acceptable for this milestone.

Listening feedback:

- The first Karplus-Strong voice had a very old-school plucked-synthesis character.
- The attack read less like a pick on a string and more like a hard tap or fretboard tap.
- Volume, sustain, and note-off behavior were acceptable enough to leave alone for the next polish step.

## 2026-04-25 — Pick Excitation Polish

The excitation was adjusted to reduce the hard-tap/noise-burst character.

Current behavior:

- The initial delay line is now dominated by a triangular displacement shape around a fixed pluck point.
- Broadband noise is reduced and localized near the pluck point as a small scrape/contact component.
- A very short velocity-dependent pick-release transient is added at note start.
- Velocity affects both amplitude and the amount of transient/noise brightness.
- The visible editor now shows plugin version, model label, and Git commit to help identify the loaded build in Bitwig.

Initial values:

- model label: `StringVoice KS-002`
- pluck position: 0.20 of string length
- displacement amount: 0.75 * velocity
- scrape noise amount: 0.025 to 0.115 depending on velocity
- pick transient: 0.02 to 0.10 depending on velocity

Expected sound:

The attack should be less dominated by tap-like noise than `StringVoice KS-001`, but it is still a simple Karplus-Strong-style voice rather than a full pick/string interaction model.

Listening feedback:

- `StringVoice KS-002` is more mellow and less harpsichord-like than the first version.
- It still wants a little more string scrape and twang/quack.
- The twang/quack may come more from pickup position, pickup filtering, or pickup mixing than from the string alone, especially for Strat-like character.
- Note-off did not yet feel like a fretting hand muting the string.

## 2026-04-25 — Left-Hand Release Damping

Note-off behavior was changed from a fixed damping value to a duration-sensitive left-hand damping approximation.

Current behavior:

- Each voice tracks `samplesSinceStart`.
- On note-off, the held duration selects a left-hand damping target and transition time.
- Very short notes get the strongest and fastest damping, approximating a quickly lifted fretting finger or immediate hand damping.
- Medium notes transition between abrupt and gentle damping.
- Longer notes release more gently, as if a held chord is being lifted with less force.
- The model label is now `StringVoice KS-003`.

Initial values:

- very short note threshold: below 0.12 seconds
- medium note range: 0.12 to 0.45 seconds
- short-note damping target: 0.55 over about 8 ms
- medium-note damping target: 0.55 to 0.82 over about 12 to 35 ms
- long-note damping target: 0.90 over about 60 ms

Expected sound:

Short detached notes should stop much more quickly without a hard digital gate. Longer notes should still have a more natural decay.

## Suggested MVP Signal Flow

```text
MIDI/MPE event
  -> voice allocation
  -> physical string voice
  -> pickup/tone shaping
  -> output gain
```

## String Voice Ideas

A basic string voice may include:

- delay line based on target frequency
- excitation/noise burst based on velocity
- damping filter in the feedback path
- fractional delay or interpolation for pitch accuracy
- smoothing when pitch bend changes delay length
- release damping for note-off behavior

Next likely improvements:

- fractional delay for better tuning accuracy
- damping/brightness controls
- a more explicitly modeled pluck position
- simple pickup-position shaping after the string voice
- smoother pitch changes for future MPE bends
- string scrape/twang experiments, likely involving both exciter and pickup-position models

## Pluck Parameters

Possible controls:

- pluck force
- pluck position
- pick hardness
- damping
- sustain
- brightness

## Electric Guitar Tone Ideas

The MVP should aim for a DI-style electric guitar tone.

Possible simplifications:

- pickup position as harmonic filtering
- tone knob as low-pass shaping
- bridge pickup as brighter/thinner
- neck pickup as warmer/rounder
- output gain with safe limiting or headroom

## Realism Research

See `docs/realism-vision.md` for collected research links and longer-term hypotheses around string/fret interaction, pickup modeling, player interpretation, and hybrid/neural directions.

## Known Risks

- Karplus-Strong can sound like a generic plucked synth if not shaped carefully.
- Pitch bending a delay line can cause clicks or zipper noise if not smoothed.
- Excess brightness can become harsh through amp sims.
- Too much damping can kill sustain before expression feels useful.
- Too much sustain can make the model feel synth-like instead of guitar-like.

## Experiment Log

Add dated notes here.

```markdown
## YYYY-MM-DD — Experiment title

Setup:
What was changed or tested.

Result:
What happened.

Useful ranges:
Any values worth keeping.

Follow-up:
What to try next.
```
