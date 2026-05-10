# 0078 - Player Feel Model

## Context

Human audition of the pick-stroke MIDI exposed two related modeling gaps:

- The MIDI audition should distinguish a held fretting-hand shape from repeated right-hand articulations. A rhythm guitarist usually holds the chord while the pick restrikes strings; the left hand does not release each note for every stroke.
- Randomized DAW humanization is too blunt. Timing and velocity variation should come from a player model: cognitive load, dexterity load, and endurance/fatigue, with deterministic repeatability.

This plan extends the earlier player-articulation work in `plans/0055-player-articulation-model.md`.

## Core Mental Model

Separate the performance into two hands and one player state:

- **Left hand / fretting state:** which string/fret is held, how stable the hand shape is, how much pressure is applied, and whether a note change is a new fret event or a held-shape repick.
- **Right hand / articulation state:** which string is struck, pick stroke direction, pick depth/angle, whether the motion is repeated alternate picking, economy travel, strumming, or a skipped-string move.
- **Player state:** short-memory estimates of cognitive load, dexterity load, and endurance recovery.

The note event remains the DAW/MIDI input, but internally it should be interpreted as one or more physical events:

- fret/shape change
- right-hand string strike
- left-hand legato strike
- release/damping gesture

## Load Signals

### Cognitive Load

Represents the attention cost of deciding what comes next.

Increases with:

- chord shape changes
- ambiguous string/fret assignment
- large hand-position shifts
- dense polyphony
- sudden phrase-direction changes
- syncopated or irregular inter-onset timing

Decays with:

- rests
- repeated patterns
- stable held chord shapes
- predictable same-string or same-direction motion

### Dexterity Load

Represents mechanical difficulty.

Increases with:

- fast repeated picking
- string skips
- awkward stroke reversals
- large fret jumps
- wide fretting-hand spread
- rapid chord changes
- high note density on one hand or one string

Decays with:

- slower phrase rate
- economy picking in the direction of travel
- repeated motor patterns that settle into a groove
- rests

### Endurance

Endurance is accumulated physical and attention fatigue. It should not be pure random drift; it is a pressure that grows under sustained demand and recovers over time.

Track at least:

- right-hand endurance
- left-hand endurance
- cognitive endurance

Endurance should raise the probability and magnitude of small timing delays, velocity compression, pick-depth changes, and occasional recovery accents. It should never make the instrument feel broken at normal settings.

## Deterministic Variation

The model may use noise, but not arbitrary noise.

Variation should be:

- seeded from stable musical context: sample time, note, string/fret, stroke direction, phrase counter, and current load buckets
- deterministic across identical renders
- scaled by the load model
- biased toward recovery, correction, and motor-pattern settling

This keeps "humanization" explainable: a note is late or lighter because the model is under load, crossing strings awkwardly, recovering from fast repetition, or preserving a groove.

## Output Targets

The interpreter can shape:

- attack timing
- velocity/energy scale
- pick bite/depth/angle micro-variation
- fret pressure/lift
- contact noise intensity
- whether a repeated same-string note is a repick of an existing fretted state rather than a fresh left-hand event

For real-time play without lookahead, timing changes can only safely delay notes. With the existing `Lookahead` mode enabled, a future pass can center timing variation around the compensated grid and allow both early and late human timing relative to the audible note.

## First Implementation Slice

Start with a small, reviewable right-hand feel pass:

- Add a single `Player Feel` or `Pick Feel` amount.
- Track right-hand load in `AudioEngine` using only current and recent picked notes.
- Increase load for:
  - fast same-string repetition
  - abrupt same-string stroke reversals
  - string skips
  - dense strumming
- Reduce load for:
  - rests
  - economy travel across adjacent strings
  - stable repeated chord strums after the first stroke or two
- Apply a small deterministic picked-note delay and velocity/energy scale:
  - default/off must be byte-identical or effectively neutral
  - normal musical range should be subtle, around a few milliseconds and a few percent energy
  - extreme values can be useful for auditioning but should not be the default

This first pass should not attempt a full fingering solver, offline lookahead optimizer, or error model.

## Second Implementation Slice

Add held-shape repick semantics:

- Detect a same-string/same-fret note-on while the fretting state is still held.
- Treat it as a right-hand restrike instead of a new left-hand fretting event.
- Inject new pick energy into the string while avoiding unnecessary fretting release/reacquire noise.
- Keep MIDI compatibility by still accepting ordinary note-on/note-off clips.

This is likely more physically correct for rhythm-guitar strums and repeated chord articulations than only adjusting MIDI note lengths.

## Audition Guidance

Use `tests/midi/guitar-ag-pick-stroke-audition.mid` with:

- `Pick Stroke = Alternate`
- `Legato Articulation = 0%`
- `Pick Bite` around 20% to 60%

Listen specifically for:

- whether strums feel like a stable left hand plus moving right hand
- whether repeated picking has a controlled build-up of effort
- whether string skips feel slightly harder than adjacent economy motion
- whether stable grooves settle rather than remain uniformly random

## 2026-05-10 First Implementation Result

Implemented `StringVoice EG-075 PlayerFeel`.

- Added `Player Feel` amount.
- Added `Feel Recovery` time.
- Added UI `Reset Feel`, which requests an immediate load-state reset from the audio engine.
- Added offline flags:
  - `--player-feel`
  - `--player-feel-recovery`
  - `--player-feel-reset-at`
- Added a separate real-time-safe player-feel fretboard mapper inside `AudioEngine`.
- Tracked cognitive load, dexterity load, and one endurance bucket.
- Applied deterministic picked-note delay and note-on velocity scaling from the accumulated load.

Verification:

- Build/install succeeded locally.
- `Player Feel = 0` matched the default pick-stroke audition render byte-for-byte.
- `Player Feel = 100%` produced a different render from neutral.
- Repeating the same `Player Feel = 100%` render produced byte-identical WAVs.
- `--player-feel-reset-at 45` produced a different render from the non-reset feel render.

Remaining limitation:

This first pass only delays note starts. Early timing requires a lookahead-aware model. The engine also does not yet inject a fresh right-hand pick event into an already-held fretted string without treating it as a new note start.
