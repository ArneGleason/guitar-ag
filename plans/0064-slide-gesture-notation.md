# Plan 0064 - Slide Gesture Notation And Playback

## Goal

Design an authoring and playback path for guitar slides that can be expressed with normal DAW notes plus automation curves or MPE note-expression curves.

The target gestures are:

- a held single note sliding to a new pitch and settling there,
- a held chord or chord shape sliding quickly to a new neck position,
- a dramatic slide excursion up and back down,
- a slide away from the note into a muted or open-string tail.

## Research Summary

The standard guitar-facing term is **slide**. The broader notation term is **glissando**, often shortened to **gliss**. In guitar/tab editors and lessons, useful subtypes are:

- **Legato slide**: the destination note is not picked again.
- **Shift slide**: the destination note is picked or re-attacked.
- **Slide in**: the start pitch is indefinite and the player slides into the written note.
- **Slide out**: the written note slides away into silence or an indefinite endpoint.
- **Grace-note slide**: a very quick defined approach note into a main note.

The user's "throw" description maps well to an internal **Slide Throw** preset/shape: a fast slide excursion, usually up-neck, that may return, overshoot, or fall away. It should not be the only UI term, because "slide" is the standard musician-facing umbrella.

References:

- MusicXML 4.0 `slide`: https://www.w3.org/2021/06/musicxml40/musicxml-reference/elements/slide/
- MusicXML 4.0 `glissando`: https://www.w3.org/2021/06/musicxml40/musicxml-reference/elements/glissando/
- Soundslice tab slide types: https://www.soundslice.com/help/en/creating/tablature/118/slides/
- Ableton Live MPE dimensions: https://help.ableton.com/hc/en-us/articles/360019144999-MPE-in-Live-FAQ
- StudyBass slide terminology: https://www.studybass.com/lessons/bass-technique/slides-part-1/

## Product Shape

Use **Slide Gesture** as the project umbrella for this feature family.

First-class gesture shapes:

- `Slide To`: source note or chord sustains while the fretting hand moves to a target fret/pitch and settles.
- `Slide Throw`: source note or chord makes a fast up/down excursion from an automation curve without requiring a new destination note.
- `Slide Out`: source note or chord moves away and releases into a muted or open tail.
- `Chord Shift Slide`: all eligible active voices share a common neck-position offset so a chord shape moves as one physical hand motion.

The important distinction is that a guitar slide is not only pitch bend. A plain pitch bend changes frequency. A fret slide should also be able to add finger drag, fret crossing texture, fret-pressure changes, wound-string scrape, pickup-position consequences, and eventually a physical change in speaking length.

## Authoring Model

### Single-Note Slides

For the near term, single-note slides can be authored with MPE per-note pitch bend curves on the held note.

Benefits:

- Existing MPE routing already keeps pitch bend on the intended note.
- DAWs that support MPE note expression let the human draw the pitch path directly.
- Existing `MPE Bend Range` already supports wide slide ranges.

Important naming note:

- Some MPE docs call the Y dimension "Slide", while Guitar AG currently treats CC74 as timbre. For this project, **guitar slide** should use pitch motion, not MPE CC74, unless a later mode explicitly remaps CC74.

### Chord Slides

Chord slides should not require drawing the same MPE curve on every note.

Add an automatable global performance lane, tentatively:

- `Slide Offset`
- unit: semitones for host automation, interpreted as frets when a string/fret assignment is active
- default: `0.0 st`
- initial range: `-48.0 st` to `+48.0 st`
- smoothed on the audio thread

This lane applies a common offset to active voices, so a held chord can move as a shape with one automation curve. Per-note MPE bends should layer on top of it for independent detail.

### Slide Throws And Slide Outs

Do not add a separate event format at first. A throw can be a fast automation curve on `Slide Offset`. A slide-out can be a curve plus a note release while slide velocity is still high.

Useful curve idioms:

- fast ramp up, hold, settle: `Slide To`
- fast ramp up and down: `Slide Throw`
- ramp away, note-off, damping tail: `Slide Out`
- small pre-note ramp into a note: `Slide In`

## Implementation Phases

### Phase 1 - Planning And Audition Assets

- Keep this plan as the implementation source of truth.
- Add a scripted slide audition MIDI file for the current MPE-capable slide subset.
- Extend that audition workflow with a true global `Slide Offset` automation test once the first slide parameter/control exists.
- Include at least:
  - single held-note MPE slide to target,
  - single held-note slide throw up and back,
  - triad or power-chord shift slide using duplicated MPE curves now, then global `Slide Offset` automation in a DAW test after Phase 2,
  - slide-out release with muted tail,
  - MPE pitch bend layered with pressure/CC74 now, then global slide offset after Phase 2.

### Phase 2 - Playback MVP

Add a global, automatable `Slide Offset` performance lane and make it behavior-neutral at zero.

Reviewer adjustment:

- Use `Neck Slide` as the host-facing parameter name.
- Keep `Slide Offset` as the internal design concept.
- The first implementation used a `-24.0 st` to `+24.0 st` host-facing range.

Human DAW adjustment:

- Reduce `Neck Slide` to `-12.0 st` to `+12.0 st`.
- Add `Fret Steps` as a separate amount control so the lane can move through semitone/fret plateaus instead of behaving like a smooth whammy bend.

Likely code touchpoints:

- `src/plugin/PluginProcessor.h`
- `src/plugin/PluginProcessor.cpp`
- `src/plugin/PluginEditor.h`
- `src/plugin/PluginEditor.cpp`
- `src/dsp/AudioEngine.h`
- `src/dsp/AudioEngine.cpp`
- `src/dsp/StringVoice.h`
- `src/dsp/StringVoice.cpp`

Engine direction:

- Smooth `Slide Offset` similarly to other performance parameters.
- Pass the current slide semitone offset into `StringVoice::renderSample`.
- Include the offset in `StringVoice::updatePitchRatio`.
- Keep MPE pitch bend, aftertouch bend, vibrato, and whammy behavior additive with slide offset.
- Use `Fret Steps` as the first amount control for fret-stepped pitch shaping and fret-crossing contact texture.
- Keep `Slide Offset == 0.0f` neutral and sample-identical to the current path when no slide curve is active.

### Phase 3 - Physical Same-String Slide

After the MVP proves authoring, add a voice-level same-string transition instead of treating slides as only a pitch ratio.

Desired direction:

- Preserve modal/delay energy while the effective speaking length changes.
- Track source and target string/fret assignment.
- Add fret-crossing and finger-drag texture based on slide direction, speed, string class, and pressure.
- Let the final settled fret update the logical assignment so pickup/readout and intonation behavior are coherent after landing.

This phase overlaps with the remaining limitation in `plans/0055-player-articulation-model.md`: hammer-ons, pull-offs, taps, and slides all eventually want same-string state preservation.

### Phase 4 - Chord Shape Interpretation

Once the global offset works, teach the performance interpreter to understand chord-shape slides more physically.

Direction:

- Group active voices that were started close together as a probable hand shape.
- Prefer a common fret offset for fretted strings.
- Treat open strings specially: they may stay open, be muted, or become a slide-in hammer depending on mode.
- Keep the first implementation permissive. If a physical fingering is uncertain, produce the musical slide rather than blocking the chord.

### Phase 5 - Notation And Interchange

DAW notes plus automation remain the primary workflow.

Later, MusicXML or tab import can map:

- `slide` to the guitar-slide gesture family,
- `glissando` to a more generic pitch-continuity gesture,
- slide-in/out markings to indefinite start/end offsets,
- legato vs shift slides to whether the destination gets a fresh excitation.

## Out Of Scope

- Full tablature import in the first implementation.
- A complete offline fingering solver.
- Alternate tunings or capo modeling.
- Replacing the existing MPE pitch-bend behavior.
- Making CC74 mean guitar slide by default.

## Open Questions

- Should the first exposed parameter be named `Slide Offset`, `Fret Slide`, or `Neck Slide`?
- Should `Slide Offset` be on the Artic page, a new Slide page, or the MPE/Performance page?
- Should `Fret Steps` default higher or lower than 65% after more DAW listening across slow slides, fast throws, and chord moves?
- For slide-out, should the first tail be muted-only, or should a simple `Muted/Open` tail mode exist from the beginning?
- Should chord slides apply to all active voices, or only voices whose notes started within a short grouping window?

## Verification

Behavior-neutral checks:

- With `Slide Offset` at `0.0 st`, existing feature-audition renders should match the pre-slide render.
- Existing MPE independent bend tests should still pass.
- Existing whammy/pitch-wheel tests should still pass.

New checks:

- One held MPE note follows a drawn pitch-bend slide while other held MPE notes stay stable.
- A held chord follows one `Slide Offset` automation curve as a shared shape.
- Per-note MPE pitch bend and global `Slide Offset` stack predictably.
- Slide scrape/drag is strongest during fast offset movement and falls silent when the offset is static.
- A slide-out note release produces a plausible muted/fading tail without a clicked note-off.

## Completion Notes

Phase 1 started on 2026-05-09.

- Added `scripts/create-slide-gesture-midi.py`.
- Generated `tests/midi/guitar-ag-slide-gesture-audition.mid`.
- Documented the segment map and offline render command in `docs/audition-midi.md`.
- The current audition file covers MPE pitch-bend slide-to, slide-in, slide throw, slide-out, independent chord-note slide, a duplicated-MPE chord-slide proxy, and pitch slide layered with pressure/CC74.
- At the end of Phase 1, true one-lane chord-shape slide automation remained blocked until the global slide parameter existed.

Phase 2 started on 2026-05-09.

- Added a global automatable `Neck Slide` VST parameter with an internal slide-offset role.
- The initial range was `-24.0 st` to `+24.0 st`, defaulting to `0.0 st`.
- Added smoothing in `AudioEngine` and passed the value to each active `StringVoice`.
- Layered neck slide into `StringVoice::updatePitchRatio` alongside vibrato, whammy, aftertouch bend, and MPE pitch bend.
- Added final pitch-ratio and per-mode pitch-step clamping to keep stacked bends bounded.
- Added `--neck-slide` to `GuitarAGOfflineRender`.
- Added a held-chord automation bed to the slide audition MIDI for DAW-side `Neck Slide` automation.
- After human DAW listening, Phase 2b reduced `Neck Slide` to `-12.0 st` to `+12.0 st`.
- Added `Fret Steps` to blend the global slide lane toward semitone/fret plateaus and add subtle fret-crossing contact ticks/scrape while the lane moves.
- Added `--slide-fret-steps` to `GuitarAGOfflineRender`.
- After human DAW listening, Phase 2c remapped `Fret Steps` so 10% reaches the former 90% engagement point and 10% to 100% covers the audible fret-engagement zone.
- After Antigravity verified EG-064, Phase 2d added the first `Slide Tail` modes: `Normal`, `Muted`, `Open`, and `Slide Off`.
- `Slide Tail` special modes are gated by recent `Neck Slide` movement so ordinary note releases remain unchanged.
- Added offline `--slide-tail` and `--neck-slide-at` checks so slide-ending behavior can be smoke-tested without DAW automation.
- Same-string speaking-length changes remain future work.
