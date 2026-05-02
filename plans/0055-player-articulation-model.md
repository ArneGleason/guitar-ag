# Plan 0055 — Player Articulation Model

## Goal

Design the next layer above the raw string model: a guitar-player interpretation system that chooses plausible left-hand positions and turns suitable MIDI gestures into guitar-like articulations.

## Motivation

The current fretboard mapper assigns each note to a plausible string/fret position, but it does not yet reason about phrases, held chord shapes, finger reach, hand position, or whether a new note should be picked, hammered-on, pulled-off, or tapped.

The next feature family should make MIDI feel less like independent synth notes and more like a guitarist deciding what the hands can plausibly do.

## Proposed Controls

- `Legato Articulation` or similar player-mode amount.
- Separate hammer-on and pull-off behavior if the single control is too blunt.
- 0-20%: no automatic hammer-ons/pull-offs.
- 20-70%: increasingly use left-hand hammer-ons and pull-offs when the opportunity is clear.
- 70-100%: increasingly allow right-hand taps and more adventurous threaded legato/tapping behavior.

## Heuristic Direction

- Track active notes as a possible fretting-hand shape, not just independent voices.
- Prefer realistic positions near the current hand position unless the phrase strongly implies a shift.
- Preserve open strings when musically plausible.
- Treat same-string nearby note transitions as hammer-on/pull-off candidates.
- Treat larger or faster same-string leaps as possible tapping candidates only in the high range of the control.
- Never block notes that are physically awkward; fall back to picked notes when the heuristic is uncertain.

## Research Notes

- Guitar fingering research commonly treats MIDI-to-fretboard interpretation as a phrase/path problem, not as an isolated note problem.
- Dynamic-programming and Viterbi-style systems score each possible string/fret/finger state with static costs and transition costs, then choose the lowest-cost path through the phrase.
- Useful cost families for this project are:
  - hand position movement along the neck
  - string changes across the neck
  - fretting-hand spread and finger reach
  - open-string preference or avoidance
  - preserving the current hand shape
  - matching the intended timbre of a string/fret choice
- Recent lead-guitar tablature work separates basic fingering from the later insertion of articulations and expressive techniques.
- That same work uses biomechanical eligibility first, then style/statistical targets second:
  - hammer-on is possible when a previous lower-pitched note is on the same string, either open or fretted by a lower finger.
  - pull-off is possible when a previous higher-pitched note is on the same string, with the destination lower fret or open string already plausible.
  - if there are more eligible points than the target style amount, select among eligible points probabilistically.
  - if there are fewer eligible points than the target style amount, do not force unnatural articulations.
- Right-hand tapping can be treated as an extension of hammer-ons and pull-offs using the picking hand as another fretting hand.

## Proposed `Legato Articulation` Control Map

Interpret the control as a target style amount, not a command to force every note into an articulation.

- 0-20%: always prefer picked notes except for explicit authored gestures added later.
- 20-30%: allow occasional pull-offs where the same-string descending target is obvious, especially to open or already implied lower frets.
- 30-50%: allow hammer-ons and pull-offs for nearby same-string moves, with probability increasing as the control rises.
- 50-70%: allow longer legato chains, trills, and more aggressive same-string phrase preservation.
- 70-85%: introduce right-hand tap candidates when there is a held or implied lower-fret anchor and the new note is a higher same-string leap.
- 85-100%: make tapping and tap/pull-off arpeggio behavior predominant where physically plausible, while still falling back to picked notes for chords, awkward string changes, or ambiguous input.

## Streaming Algorithm Sketch

For a first real-time pass, use a small streaming interpreter rather than full offline dynamic programming.

1. Keep a fixed-size, allocation-free history of recent note events and current per-string state:
   - note number, channel, velocity
   - string/fret assignment
   - start sample, release sample, held/released state
   - voice id if still active
   - inferred fretting-hand position
2. For each note-on, generate all valid string/fret candidates instead of immediately choosing one best mapping.
3. Score each candidate with the current fretboard costs:
   - distance from hand position
   - occupied string penalty
   - open-string and low-position bias
   - estimated reach relative to the current hand shape
   - string-change cost scaled by inter-onset time
4. Add articulation bonuses only for eligible same-string relationships:
   - hammer-on: previous or still-ringing same-string source is lower than the new fret.
   - pull-off: previous same-string source is higher than the new fret, including pull-off to open string.
   - tap: control is high, same-string lower anchor is held or strongly implied, new note is a higher fret leap, and the phrase is fast enough to sound intentional.
5. Convert the best candidate into a `PlayerGesture`:
   - `Picked`
   - `HammerOn`
   - `PullOff`
   - `RightHandTap`
6. Use deterministic probability only after eligibility and scoring:
   - probability = style amount * gesture confidence * phrase-rate factor * per-string style weight
   - seed the random choice from note timing/channel/pitch so repeated renders are stable.
7. If no gesture is strongly eligible, choose `Picked` and preserve current behavior.

## Sound Model Needs

- Hammer-on: no pick excitation, lower/no pick texture, quick left-hand impulse into an already chosen string.
- Pull-off: release/pluck from the fretting finger, often softer and slightly noisier than a pick.
- Right-hand tap: stronger finger-impact excitation with little/no pick scrape and a sharper fret/contact onset.
- Legato transitions should preserve some string state where possible instead of always starting a brand-new picked voice.

## Proposed Gesture Excitation Model

The articulation decision should drive a small excitation profile rather than only changing velocity.

Treat each note start or legato transition as an energy event with:

- `energyScale`: total energy injected into the string.
- `energyFloor`: minimum audible level when the source note is already ringing.
- `energyCeiling`: maximum level relative to a picked note.
- `attackRamp`: how abruptly energy reaches the string.
- `contactNoise`: short broadband/finger/fret noise.
- `pickScrape`: pick-specific edge and scrape components.
- `sidePluck`: lateral string displacement from a pull-off.
- `fretImpact`: hammer/tap impact against the fret.
- `highModeBoost`: short-lived upper partial emphasis.
- `stateCarry`: how much of the previous same-string vibration survives the transition.

Initial profile targets:

| Gesture | Energy | Attack | Noise / Contact | Harmonic Shape | State Behavior |
| --- | --- | --- | --- | --- | --- |
| Picked | 1.00 baseline | sharp pick release | pick scrape/contact can be strong | broad partial excitation from pick position, velocity-dependent brightness | starts a fresh voice |
| Hammer-on | 0.28-0.62 of picked | direct but rounded finger impact | little/no pick scrape, short fret/finger thump | less broadband than pick; brief upper partials from fret impact, weaker body energy | preserve previous string state if same string |
| Pull-off | 0.22-0.58 of picked | slightly grabbed/plucked, asymmetric | finger release noise and lateral slip, no pick edge | stronger low/mid displacement than hammer-on, less crisp high attack; possible small pitch/energy wobble | preserve previous state, then inject side-pluck energy |
| Right-hand tap | 0.42-0.82 of picked | direct, sharper hammer impact | little/no pick scrape, stronger fret impact than left-hand hammer-on | more high partials than hammer-on because impact is often faster and higher on the neck | preserve anchor state if part of tap/pull-off cycle |

The ranges above intentionally stay below a picked note. High-gain amp chains can compress these differences, but the DI model should expose the lower legato energy.

### Hammer-On Detail

- Eligibility: same string, destination fret above the source fret, or open string to fretted note.
- Dynamic model:
  - use lower velocity than the incoming MIDI note unless the user explicitly plays high velocity.
  - map MIDI velocity into a narrow range so hammer-ons do not become pick-loud too easily.
  - target about 30-60% of picked energy before downstream gain/compression.
- Excitation:
  - suppress pick transient, pick scrape, pick grind, and pick material layers.
  - add a short fret-impact impulse, softer than a tap.
  - emphasize a brief high-mid contact band, then decay quickly.
  - preserve some previous string vibration and retune/shorten the effective speaking length when the model can support same-string state reuse.
- Harmonics:
  - weaker fundamental displacement than a pick, because there is less total string travel.
  - a short, click-like upper-mode burst from finger/fret collision.
  - less pick-position comb character; more termination/contact character.

### Pull-Off Detail

- Eligibility: same string, destination fret below source fret, including pull-off to open string.
- Dynamic model:
  - usually similar or slightly softer than hammer-on unless the phrase implies an aggressive snap.
  - target about 25-55% of picked energy, with occasional strong pull-offs up to about 60%.
- Excitation:
  - no pick transient.
  - add lateral `sidePluck` energy from the fretting finger pulling the string sideways before release.
  - add a short finger-slip/grab noise burst, more irregular than hammer-on.
  - allow tiny initial pitch sharpness or wobble from sideways displacement, if this can be done without MPE confusion.
- Harmonics:
  - stronger low/mid modes than hammer-on for the same nominal velocity because the pull-off actually plucks the string.
  - less crisp upper attack than right-hand tapping.
  - release noise should be strongest on wound strings and at higher finger-noise settings.

### Right-Hand Tap Detail

- Eligibility: high `Legato Articulation`, same string, lower anchor held or strongly implied, destination is a higher fret leap.
- Dynamic model:
  - still usually below a picked note, but with a higher ceiling than left-hand hammer-ons.
  - target about 45-80% of picked energy.
  - allow velocity and the style control to push taps harder than left-hand legato.
- Excitation:
  - no pick scrape unless a later pick-tap mode is added.
  - stronger and sharper fret-impact impulse than hammer-on.
  - because taps are often high on the neck, scale the contact brightness with destination fret.
  - tap/pull-off cycles should alternate tap impact with pull-off side-pluck rather than retriggering full picked voices.
- Harmonics:
  - brighter than left-hand hammer-on at equal output level.
  - less pick-position coloration than picked notes.
  - short high partial emphasis should decay quickly so it reads as contact, not as a permanent brighter string.

### First Implementation Approximation

The current `StringVoice::start` path can support a first pass by adding an articulation/profile argument and remapping its existing excitation variables:

- scale `velocityGain`, `displacementAmount`, `horizontalAmount`, `steelPartialAmount`, and `modalGain` by `energyScale`.
- set pick-material layers near zero for hammer-on, pull-off, and tap.
- add a new finger/fret impact envelope separate from pick contact.
- add a pull-off-only lateral impulse/noise envelope.
- make attack ramp longer for hammer-on, asymmetric/noisier for pull-off, and shorter for tap.

The more physical second pass should add a same-string transition method that preserves delay/modal state and changes pitch/length instead of always creating a fresh voice.

## Out Of Scope For First Pass

- Full tablature-quality fingering solver.
- Exact anatomical hand model.
- Alternate tunings.
- User-configurable guitar setup profiles.
- Machine-learned performance model.

## Verification Ideas

- MIDI with clear same-string one- or two-fret moves should become hammer-ons/pull-offs as the control rises above 20%.
- Chords should keep stable plausible left-hand assignments rather than jumping strings every note.
- Above 70%, fast high-register note patterns should start to use tap-like articulation.
- At 0%, the current picked-note behavior should remain effectively unchanged.

## 2026-05-02 First Implementation Result

Implemented the first conservative player-articulation milestone.

- Added a `Legato Articulation` parameter and editor slider.
- Added a small real-time-safe recent-note memory in `AudioEngine`.
- Added deterministic same-string gesture selection for `HammerOn`, `PullOff`, and `RightHandTap`.
- Extended `FretboardMapper` so the interpreter can prefer an eligible same string.
- Added `PlayerGesture` excitation profiles to `StringVoice`.
- Hammer-ons, pull-offs, and taps now reduce picked-note energy, suppress pick-material layers, and add separate finger/fret contact envelopes.
- The offline renderer accepts `--legato-articulation`.

Verification:

- Built `GuitarAGOfflineRender`.
- Built `GuitarAG_VST3`.
- Rendered `build/diagnostics/guitar-ag-eg048-legato000.wav`.
- Rendered `build/diagnostics/guitar-ag-eg048-legato100.wav`.
- The two renders have different SHA-1 hashes, confirming the control changes the rendered path.

Remaining limitation:

This first pass still starts a fresh destination voice. The next physical step is a same-string transition method that preserves source string energy and changes the speaking length for hammer-ons, pull-offs, and tap/pull-off cycles.
