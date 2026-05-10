# Guitar AG v0.3.0 - Slide, Strum, and Player Feel

This release is the first Guitar AG build where the instrument starts to feel less like a modeled string testbed and more like a guitar part authoring tool.

The big theme is player gesture. Guitar AG is still not sample based, and it still produces a clean DI-style modeled electric guitar tone for external amp/cab processing. v0.3.0 adds more ways to tell the model what a guitarist is doing with both hands: sliding a held shape, striking a block chord as a strum, changing pick direction, and adding a deterministic amount of human timing and energy variation.

## Highlights

- Added `Neck Slide`, a global automatable slide/glissando lane for held notes and chord shapes.
- Added `Fret Steps`, `Slide Lift`, `Squeak Up`, and `Squeak Down` so slides can read as fretted finger motion instead of only smooth whammy motion.
- Added a direction-aware plastic plectrum model with `Pick Bite`, `Pick Stroke`, revised pick-contact sound, and deterministic per-attack variation.
- Added `Auto Strum` through the `Strum Speed` control: exact same-time block chords can be fanned across assigned strings by the instrument.
- Added `Strum Balance`, a bipolar control that can soften generated downstroke or upstroke strums without boosting already-hot MIDI velocities.
- Added `Player Feel`, `Feel Recovery`, `Reset Feel`, and live Cognitive/Dexterity/Endurance meters for deterministic timing and energy variation.
- Added a copyable `Export Settings` JSON dialog for sharing audition contexts and preferred control settings.
- Reorganized Performance and Articulation UI pages around primary controls with disclosure rows for secondary character tweaks.
- Completed a DSP performance and maintainability pass across pitch modulation, feedback weighting, transient contact math, and render helper structure.

## How To Use The New Controls

### Slides

Automate `Neck Slide` while a note or chord is held. The range is one octave up or down, which keeps the lane practical for guitar-like motion.

Use `Fret Steps` when you want the slide to catch the frets instead of bending perfectly smoothly. Use `Slide Lift` when the finger pressure should ease off during the motion. Use `Squeak Up` and `Squeak Down` to balance the finger/string noise separately for upward throws and downward returns.

A good starting point is:

- `Neck Slide`: automate only where the slide happens.
- `Fret Steps`: 100%.
- `Slide Lift`: 0% to 50%, depending on how muted or light the slide should become.
- `Squeak Up` and `Squeak Down`: around 20%.

### Pick And Strum

`Pick Bite` is now the main pick-character control. It changes how strongly the pick contact imprints on the string attack. `Pick Stiffness` and `Pick Texture` remain available under the Pick Bite disclosure for more detailed tone shaping.

`Pick Stroke` controls right-hand direction:

- `Down`: force downstrokes.
- `Up`: force upstrokes.
- `Alternate`: alternate repeated same-string notes, while using economy direction when the part moves across strings.

`Strum Speed` is for MIDI chords where all notes begin together. At 0%, the notes stay simultaneous. Raising it fans the chord across strings according to the current pick direction. This lets the DAW piano roll hold a chord shape as a block while Guitar AG supplies the right-hand strum timing.

`Strum Balance` has a balanced center. Positive values soften generated upstrokes. Negative values soften generated downstrokes. The v0.3.0 default is a subtle `-13%` because that auditioned well as a starter feel.

### Player Feel

`Player Feel` is not random humanize. It is deterministic, so the same MIDI and settings render the same result. The model tracks cognitive load, dexterity load, and endurance, then uses those loads to create timing and energy variation.

The slider landmarks are:

- `Bot`: mechanically exact.
- `Pro`: controlled human timing and energy.
- `Loose`: deliberately sloppier playing.

Use `Feel Recovery` to control how quickly accumulated load clears during easier passages or rests. `Reset Feel` clears the load state immediately, which is useful when starting a new take or audition section.

## Other Important Changes

- Lookahead now schedules continuous expression events with delayed notes, so pitch wheel, mod wheel/CC1, channel pressure, CC74, and aftertouch stay aligned with lookahead-delayed note starts.
- Toggling MPE mode now clears per-channel pitch bend, pressure, and timbre state.
- In normal MIDI mode, channel pressure and CC74 now fall back to global expression behavior.
- The visible build identity remains in the plugin header so DAW plugin-cache confusion is easier to spot.

## Build

- Version: `0.3.0`
- Model label: `StringVoice EG-081 ArticGroups`
- Tag: `v0.3.0`

## Manual Audition Notes

- Open a fresh plugin instance and confirm the header shows `v0.3.0 / StringVoice EG-081 ArticGroups`.
- Try block chords with all MIDI notes starting at the same timestamp, then raise `Strum Speed` to hear Guitar AG fan the chord across strings.
- Try `Pick Stroke = Alternate` on single-string repeated notes, string-crossing lines, and block chords.
- Automate `Neck Slide` on a held note or chord and adjust `Fret Steps`, `Slide Lift`, `Squeak Up`, and `Squeak Down`.
- Raise `Player Feel` from Bot toward Pro and Loose while watching the Cognitive, Dexterity, and Endurance meters.
- Use `Export Settings` after finding a useful tone or feel so the exact audition context can be shared.

## Assets

- `GuitarAG-v0.3.0-macOS-vst3.zip` contains the macOS VST3 bundle.
- A Windows VST3 asset can be attached to this release after the Windows build is produced.
