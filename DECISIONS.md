# Decisions

## 2026-05-17 — Preserve chord assignment in mixed MPE event groups

Guitar AG will treat same-sample MPE pitch, pressure, and CC74 messages as expression setup for the note-ons in that sample, not as a reason to bypass chord-aware string assignment.

The audio engine should dispatch note-offs and expression messages at the sample boundary, then assign the remaining simultaneous note-ons as one guitar chord. This preserves Bitwig-style MPE ordering while keeping dyads such as E2+B2 on separate physical strings.

## 2026-05-17 — Expose MIDI/DAW octave interpretation as an input mode

Guitar AG will expose an `Input Octave` choice rather than silently assuming every host labels octaves the same way. `MIDI E2=40` keeps the physical/scientific mapping where MIDI note 40 is guitar low E. `DAW E2=52` subtracts one octave at MIDI ingestion so DAWs that display MIDI note 40 as E1 can still be authored with visible guitar names.

The transpose happens before fretboard assignment, note-off matching, per-note aftertouch, Player Feel, finger noise, and Auto Strum grouping. This preserves voice routing and avoids treating the correction as a pitch-shift effect after the guitar model.

## 2026-05-17 — Preserve block chords ahead of automatic legato

Guitar AG will treat same-instant and near-same-instant chord tones as independent chord voices before considering automatic hammer-on, pull-off, or tap interpretation.

When chord grouping has already selected a string for a note, that explicit chord assignment wins over legato-source lookup. Legato candidates must also have existed briefly before they can be used as sources, so a just-started chord tone cannot steal the next chord tone on the same string.

## 2026-05-17 — Treat host panic messages as performance-state resets

Guitar AG will handle MIDI All Sound Off and All Notes Off messages as full voice and performance-interpreter cleanup events.

The host may send these messages when stopping or rewinding instead of sending ordinary note-offs for every held note. Ignoring them can leave the fretboard mapper believing a string is still occupied after audio has stopped, especially after expressive gestures such as `Neck Slide`.

## 2026-05-17 — Expose a manual and automatable panic reset

Guitar AG will expose `Panic Reset` as both a visible editor button and an automatable host parameter.

This gives DAW sessions a direct recovery and diagnostic tool even when MIDI CC panic messages are awkward to author or hidden behind host-specific controller lanes.

## 2026-05-17 — Add assignment diagnostics to the plugin surface

Guitar AG will expose a live six-string status strip and a copyable JSON diagnostics log for fretboard assignment debugging.

The string strip distinguishes mapper occupancy from actual active voices, because the suspected failure mode is stale assignment state rather than audio voice count alone. The log is bounded to the last 1000 events and is preallocated so diagnosis does not add audio-thread allocation or blocking.

Use this file to record important project decisions so they do not get lost in chat history.

## Template

```markdown
## YYYY-MM-DD — Decision title

Decision:
Short statement of the decision.

Reason:
Why this choice was made.

Status:
Accepted / Rejected / Superseded / Under Review

Consequences:
What this means for implementation.
```

## 2026-04-25 — Use MPE as the primary expressive control model

Decision:
The instrument will prioritize MPE for independent per-note pitch bend and expression.

Reason:
Independent bending per note is central to the project. Standard MIDI pitch bend is normally channel/global and is not sufficient for the intended playing style.

Status:
Accepted

Consequences:
The MIDI layer must track channel-to-voice assignment and route pitch bend, pressure, and CC74/timbre per active note.

## 2026-04-25 — Start with a simple physical string model

Decision:
The first sound engine should use a simple Karplus-Strong or digital waveguide-style model rather than a complete scientific guitar simulation.

Reason:
This keeps the MVP achievable and lets the project prove the plugin, polyphony, and MPE behavior before deeper modeling.

Status:
Accepted

Consequences:
The first version should focus on musical playability and stable voice behavior. More advanced guitar realism can be added later.

## 2026-04-25 — Exclude amp and cabinet simulation from the MVP

Decision:
The MVP should output a clean DI-style electric guitar tone and rely on external amp/cab sims.

Reason:
Amp and cabinet modeling would expand the scope and distract from the core physical-model/MPE problem.

Status:
Accepted

Consequences:
The tone section should focus on string, pickup, tone, and output shaping only.

## 2026-04-25 — Use CMake with external JUCE for the plugin shell

Decision:
The initial VST3 plugin shell uses CMake and JUCE, with JUCE supplied externally through `JUCE_PATH` or an installed CMake package.

Reason:
JUCE is the preferred framework for the MVP, and keeping it external avoids vendoring a large framework into the project before the plugin architecture is proven.

Status:
Accepted

Consequences:
The repository now contains a small JUCE/CMake plugin structure. A developer must install CMake and provide JUCE before the VST3 target can be configured and built.

## 2026-04-25 — Keep placeholder synthesis separate from plugin wrapper

Decision:
The milestone-one test tone lives in `src/dsp/`, while JUCE lifecycle and editor code live in `src/plugin/`.

Reason:
This keeps the first shell simple while preserving the intended boundaries for later string-model and MPE-routing work.

Status:
Accepted

Consequences:
Future milestones can replace the test-tone voice with a modeled string voice without moving DAW lifecycle or editor code.

## 2026-04-25 — Use Karplus-Strong for the first string voice

Decision:
The first physical string voice uses a small Karplus-Strong-style delay-line model.

Reason:
It is simple, real-time friendly, easy to debug, and enough to replace the sine test tone with a plucked physical-model sound before adding pickup modeling, MPE, or guitar-specific articulations.

Status:
Accepted

Consequences:
The first string voice will not yet be fully guitar-realistic. Future work should improve tuning accuracy, damping, pluck position, pickup position, and expressive pitch changes without losing the current buildable milestone.

## 2026-04-25 — Show build identity in the plugin editor

Decision:
The plugin editor should show version, model label, and Git short commit.

Reason:
During Bitwig testing, stale or duplicate plugin entries can make it unclear which VST3 binary is loaded.

Status:
Accepted

Consequences:
Build identity is generated by CMake and displayed in the UI. Future listening feedback can refer to a visible model label and commit.

## 2026-04-25 — Approximate left-hand release before full fret modeling

Decision:
Short-term note-off behavior will use a duration-sensitive damping approximation inside `StringVoice`.

Reason:
The instrument needs more guitar-like note endings before the project has a full fret/finger interaction model. Short notes should mute quickly, while longer notes should release more gently.

Status:
Accepted

Consequences:
This is a pragmatic listening-driven model. A future fret/finger system may replace it with a more physically detailed contact/damping model.

## 2026-04-25 — Add fixed pickup/tone stage before UI controls

Decision:
Add a fixed post-string `ElectricGuitarTone` stage before exposing pickup/tone parameters.

Reason:
The calibration comparison suggests the current string voice lacks electric-guitar-like upper harmonic structure. A fixed pickup-position/tone experiment is the smallest way to test that hypothesis before designing UI controls.

Status:
Accepted

Consequences:
The first pickup model is intentionally crude and may be replaced or moved per voice later. Listening and calibration analysis should decide whether the direction is useful.

## 2026-04-25 — Move first pickup readout into each string voice

Decision:
Use a per-voice pickup readout for the next pickup experiment instead of one fixed post-mix comb delay.

Reason:
The KS004 fixed post-mix pickup/tone stage increased upper-harmonic measurements but reduced body and applied the same delay relationship to every note. A per-voice readout can scale pickup position with the modeled string length while keeping the experiment small.

Status:
Accepted

Consequences:
`StringVoice` now owns the current simplified pickup readout. `ElectricGuitarTone` remains as post-mix conditioning and can later grow into tone/circuit/output shaping without hiding note-specific pickup behavior.

## 2026-04-25 — Treat listening feedback as primary for realism experiments

Decision:
When analysis metrics and careful listening disagree on perceived guitar realism, keep the listening result as the primary direction and use analysis to explain or refine it.

Reason:
The `KS016 ModalCluster` offline render scored worse in the current log-spectral analyzer, but listening feedback identified it as much more realistic than the lower-distance hybrid/pickup candidate. The analyzer is useful, but it is still matching narrow spectral properties of particular reference takes rather than a robust perceptual model of electric-guitar string identity.

Status:
Accepted

Consequences:
The active experiment line moves to `StringVoice KS-017 ModalCluster`. Future analysis work should add metrics that capture the qualities that made this candidate convincing by ear, such as discrete ringing partials, metallic attack identity, inharmonic side modes, and string-like decay behavior.

## 2026-04-25 — Add a heuristic fretboard mapper before full performance interpretation

Decision:
Add a small `FretboardMapper` between MIDI note handling and `StringVoice` start, using standard tuning and a moving position heuristic.

Reason:
The current sound model is good enough to expose a guitar-performance problem: non-open notes need the string class of the string a guitarist would plausibly use. Research and guitar practice both frame string/fret choice as a cost problem over fretboard positions; a small heuristic is enough for this milestone without committing to a full dynamic-programming phrase interpreter yet.

Status:
Accepted

Consequences:
`StringVoice` no longer decides wound/plain behavior from exact open-string MIDI pitches. It receives a string/fret assignment from `AudioEngine`, leaving room for a future phrase-aware performance interpreter, user style controls, MPE-aware routing, and alternate tunings.

## 2026-04-26 — Use JUCE parameter state for adjustable sustain

Decision:
Expose the first user-facing sound control as a JUCE `AudioProcessorValueTreeState` parameter named `Sustain`.

Reason:
The KS024 tail sustain is useful but musical context dependent. It should be automatable, saved with host projects, and smoothed before affecting DSP.

Status:
Accepted

Consequences:
Plugin state now stores parameter state. Future controls should follow the same APVTS pattern unless a stronger reason emerges.

## 2026-04-26 — Model pick material as excitation parameters first

Decision:
Add `Pick Stiffness` and `Pick Texture` as VST parameters that shape note-start excitation.

Reason:
The current sound engine does not yet have a full pick/string contact solver. A bounded excitation-layer approximation is the smallest useful step that lets us audition pick material behavior without risking the core string sustain and tone work.

Status:
Accepted

Consequences:
The controls affect newly triggered notes. A future physical contact model may replace or reinterpret these parameters while preserving their musical intent.

## 2026-04-26 — Make anticipatory finger noise an optional latency mode

Decision:
Add finger approach/release noise through an optional lookahead mode rather than always delaying the instrument.

Reason:
Human finger noises can occur before the note or release they imply. A plugin can only synthesize those anticipatory sounds from MIDI by delaying the musical event and reporting latency to the host. That is useful for rendered playback, but it would make live playing feel delayed.

Status:
Accepted

Consequences:
`Lookahead` defaults to Off. 150 ms and 250 ms modes report plugin latency and delay note-on/note-off synthesis internally. Hosts may require transport restart or plugin reload to fully refresh latency compensation after changing the setting.

## 2026-04-26 — Add global finger vibrato before MPE pitch bend

Decision:
Add a global finger-vibrato layer controlled by sliders and optional MIDI CC1 modulation before implementing full MPE pitch bend.

Reason:
The instrument needs musically useful fretting-hand motion before the more complex MPE voice-routing work lands. A global vibrato layer is smaller, easy to audition, and does not conflict with later per-note MPE pitch handling.

Status:
Accepted

Consequences:
`Vibrato Speed`, `Vibrato Depth`, and `Vibrato Delay` affect all active voices. Each voice still runs its own vibrato phase. CC1 can add to speed and/or depth when enabled. Future MPE work may add per-note bend on top of this or replace the global layer for MPE channels.

## 2026-04-26 — Treat standard pitch wheel as global whammy before MPE

Decision:
Use the standard MIDI pitch wheel as an optional global whammy-bar control before implementing MPE per-note pitch bend.

Reason:
The user wants normal pitch-wheel motion to behave like a tremolo/vibrato arm affecting all currently ringing strings. This is musically useful now and remains distinct from the future MPE requirement, where per-note pitch bend must only affect the intended voice.

Status:
Accepted

Consequences:
At the time of this decision, pitch wheel drove all active voices through `Whammy Up Range`, `Whammy Down Range`, and `Whammy String Spread`. Later MPE work must avoid confusing global whammy behavior with per-note expression routing.

## 2026-04-26 — Add key aftertouch bend before full MPE pressure

Decision:
Map MIDI key/poly aftertouch to a configurable per-note pitch bend before implementing the full MPE pressure and CC74 routing layer.

Reason:
Keyboard players can use per-note pressure as a practical upward bend gesture now. JUCE exposes key/poly aftertouch with both note number and channel, so it can be routed to the matching active voice without requiring the full MPE member-channel allocator yet.

Status:
Accepted

Consequences:
`Aftertouch Bend` defaults to +2 semitones and allows -12 to +12 semitones. At the time of this decision, channel pressure remained unmapped. Future MPE work should preserve this behavior where useful, but distinguish key aftertouch from MPE channel pressure on member channels.

## 2026-04-26 — Default MPE pitch bend range to Bitwig's 48 semitones

Decision:
Expose `MPE Bend Range` as a visible parameter and default it to ±48 semitones.

Reason:
Bitwig commonly defaults MPE note-expression pitch bend to 48 semitones. Matching that default reduces trial-and-error: a two-semitone drawn bend in the DAW should produce a two-semitone bend in the plugin when both ranges match.

Status:
Accepted

Consequences:
`MPE Mode` routes member-channel pitch wheel per MIDI channel, while non-MPE mode keeps the existing global whammy behavior. As of the lower-zone MPE whammy pass, channel 1 pitch wheel can also remain global whammy in MPE mode. Users can set smaller ranges for guitar-focused bends, but the plugin default favors immediate Bitwig compatibility.

## 2026-05-02 — Map MPE pressure separately from aftertouch bend

Decision:
Route MIDI channel pressure and CC74/timbre by channel, but map channel pressure to tone/sustain/intensity rather than reusing the `Aftertouch Bend` pitch-bend mapping.

Reason:
Key/poly aftertouch has note identity and is useful as a keyboard bend gesture. MPE channel pressure is a broader per-note expression lane and should not unexpectedly bend pitch when the user only wants pressure dynamics.

Status:
Accepted

Consequences:
`MPE Pressure Amount` and `MPE CC74 Amount` scale channel-scoped expression. Pressure adds held-note sustain, level, and upper-mode intensity. CC74 leans the held note brighter/more bridge-like. Independent behavior still requires the DAW to place simultaneous notes on separate MPE member channels.

## 2026-04-27 — Use tabbed editor pages instead of vertical sections

Decision:
Replace the tall collapsible editor stack with fixed-height tabbed pages.

Reason:
The control set has grown enough that a vertical accordion makes the plugin window too tall for comfortable DAW use. Tabs keep the controls discoverable while giving each group enough room without requiring a skyscraper window.

Status:
Accepted

Consequences:
Only one parameter group is visible at a time. The editor stays at a compact fixed height, and future controls should be added to the relevant page or a new tab rather than extending one long vertical stack.

## 2026-05-02 — Add player articulation as a conservative heuristic layer

Decision:
Add `Legato Articulation` as a first automatic player-articulation control that chooses among picked, hammer-on, pull-off, and right-hand tap gestures using recent same-string context.

Reason:
The project needs audible guitar-player interpretation before a full phrase/fingering solver exists. A bounded heuristic can make the feature testable now while preserving the current picked behavior at 0%.

Status:
Accepted

Consequences:
The first pass uses deterministic probability and distinct excitation profiles, but it still starts a fresh destination voice rather than physically preserving and retuning the source string state. A future pass should move same-string legato into a true string-transition method.

## 2026-05-02 — Model amp feedback as one environment control

Decision:
Add `Amp Feedback` as a single performance/environment control that injects controlled harmonic sustain into active string voices before pickup/tone shaping.

Reason:
The user wants the useful musical gesture of moving a guitar toward a loud speaker, not a full amp, cab, microphone, room, or electromagnetic feedback simulation. A one-knob model keeps the feature testable and DAW-friendly while preserving the clean DI focus of the instrument.

Status:
Accepted

Consequences:
Low settings extend resonance subtly; high settings bias harmonic modes and add controlled howl. The model is intentionally bounded and voice-local, so it does not yet simulate body-wide cross-string coupling or acoustic room feedback.

## 2026-05-02 — Add a dominant-band feedback loop above local sustain

Decision:
Keep `Amp Feedback` as one user-facing control, but make the high range use a global resonator loop that listens to the shaped output and feeds only the dominant band back into matching string modes.

Reason:
The first amp-feedback pass made all active strings and harmonics feel too evenly excited. Real intentional guitar feedback usually has loop memory and a winner: one amp/speaker/string resonance builds, then another can take over as pitch, damping, or tension changes.

Status:
Accepted

Consequences:
`AudioEngine` now owns a small feedback resonator bank and sends dominant frequency/amount/signal values into `StringVoice`. This is still a bounded approximation, not an acoustic room or body simulation, but it better matches the musical behavior of feedback used as an effect.

## 2026-05-03 — Cap core guitar allocation at six physical string voices

Decision:
The core guitar voice pool is capped at six `StringVoice` instances, and new notes reuse any still-ringing voice assigned to the same physical string.

Reason:
The feedback CPU investigation showed that high amp-feedback settings can keep nearly all eight previous voices alive. A standard guitar has six physical strings, so allowing more than six independent ringing string voices is both less realistic and more expensive.

Status:
Accepted

Consequences:
The voice allocator now behaves more like a six-string instrument. Dense MIDI input can still play, but a seventh simultaneous string assignment replaces an existing physical string voice instead of adding a generic synth voice. MPE expression remains routed by note/channel to the selected voice.

## 2026-05-03 — Make the third pickup model spaced single-coils out of phase

Decision:
The third pickup model is now `Singles OOP`: two wider-spaced single-coil readouts subtracted from each other, with `Pickup Position` moving the pair together.

Reason:
The old `Humbucker OOP` model subtracted two very nearby humbucker coil positions, creating a small differential notch rather than the more familiar neck/middle-style two-pickup out-of-phase sound.

Status:
Accepted

Consequences:
Existing projects using pickup model index 2 will now get the revised spaced single-coil out-of-phase sound. The first two pickup choices remain unchanged.

## 2026-05-03 — Keep whammy available in lower-zone MPE mode

Decision:
When `MPE Mode` is enabled, pitch wheel on channel 1 now feeds the global whammy path, while pitch wheel on member channels 2-16 remains per-note MPE pitch bend.

Reason:
The first MPE implementation protected independent note bends by sending every MPE pitch wheel to per-channel pitch bend, which meant `Pitch Wheel Whammy` stopped working when MPE mode was enabled. In lower-zone MPE, channel 1 is the practical master/global channel, so it can safely carry a global whammy gesture without stealing member-channel note expression.

Status:
Accepted

Consequences:
Bitwig-style MPE note bends remain independent on member channels, and channel 1 pitch wheel can still move all active strings like a tremolo arm. Upper-zone MPE master-channel behavior is still not modeled.

## 2026-05-03 — Add string focus to amp feedback loop

Decision:
Keep `Amp Feedback` as the main amount control, but make the high-feedback loop choose a dominant physical string as well as a dominant frequency band.

Reason:
The global resonator band alone can still sound like it is exciting every compatible string evenly. Real musical feedback usually feels like one string/harmonic wins, then sometimes hands off as the instrument angle, damping, or pitch relationship changes.

Status:
Accepted

Consequences:
`AudioEngine` now tracks the string most coupled to the current feedback band and passes that focus into `StringVoice`. Focused strings receive more loop return while non-focused strings and local all-string sustain are suppressed. This preserves the six-string cap and keeps the global feedback model bounded.

## 2026-05-03 — Add optional distorted feedback return

Decision:
Add a `Feedback Return Distorted` switch that clips only the signal feeding the feedback resonator loop.

Reason:
The plugin should remain a clean DI instrument, but the sound that drives real guitar feedback often comes back from a loud, distorted amp. Clipping just the feedback return gives the loop more harmonic material without adding a full amp/cab model or distorting the main output path.

Status:
Accepted

Consequences:
The default return remains cleaner. Enabling the switch hardens the feedback loop input and can make takeover easier or more amp-like, while still leaving external amp sims responsible for the audible amp/cab tone.

## 2026-05-03 — Duck amp feedback on new note attacks

Decision:
New note-ons temporarily reduce the effective amp-feedback amount, then let it bloom back toward the user-set slider value.

Reason:
Listening showed that feedback sounded more convincing when it was manually held down during a picked chord attack and raised during the sustain. The model should let the dry guitar attack establish the string/chord energy before the amp loop starts choosing a resonant winner.

Status:
Accepted

Consequences:
The `Amp Feedback` slider remains the single amount control, but internally the effective feedback amount is shaped by a note-on bloom envelope. Picked attacks duck the loop most, legato gestures duck it less, and clustered chord notes share one duck event. A fresh attack also lightly reduces previous loop state and clears string focus so the next note or chord can take over.

## 2026-05-03 — Default feedback return to distorted

Decision:
`Feedback Return Distorted` now defaults on for new plugin instances and offline renders.

Reason:
Auditioning showed that the clipped return sounded more natural and reduced the clean-return chirp during early feedback bloom. This better approximates a loud amp signal driving the guitar while the main output remains clean DI-style.

Status:
Accepted

Consequences:
The default high-feedback sound is now the clipped-return path. Users can still turn `Distorted Return` off for a cleaner alternate behavior or diagnostic comparison. Existing saved DAW projects may retain their stored parameter value.

## 2026-05-09 — Use Neck Slide as the first global slide lane

Decision:
Expose the first global slide-control parameter as `Neck Slide`, with an internal slide-offset meaning and a `-24.0 st` to `+24.0 st` range.

Reason:
Single-note slides can already be authored with MPE per-note pitch bend, but chord-shape slides need one automatable guitar-wide lane. Antigravity reviewed the phase-1 slide plan and recommended `Neck Slide` as the more guitar-idiomatic host-facing name, with a two-octave range for more precise 1- or 2-fret automation than a `-48..+48 st` lane.

Status:
Accepted

Consequences:
`Neck Slide` is a global performance parameter layered additively with MPE pitch bend, whammy, vibrato, and aftertouch bend. The final pitch ratio is clamped before modal phase-step calculation, and individual modal pitch steps are also kept below the safe high-frequency limit. Physical slide scrape, fret-crossing texture, and same-string speaking-length preservation remain later phases.

## 2026-05-09 — Make Neck Slide octave-ranged and fret-stepped

Decision:
Reduce `Neck Slide` to `-12.0 st` to `+12.0 st` and add a separate `Fret Steps` amount control that blends the lane toward semitone plateaus with small fret-crossing contact noise.

Reason:
Human DAW listening confirmed the first `Neck Slide` lane changed pitch, but the two-octave smooth response felt more like tensioning strings or using a whammy control than sliding a fretted hand position. A practical fast neck slide rarely needs more than an octave up/back, and a fretted slide should imply repeated engagement/disengagement of adjacent frets rather than a perfectly continuous bend.

Status:
Accepted

Consequences:
At `Fret Steps` 0%, `Neck Slide` remains a smooth global pitch offset for diagnostic or special-effect use. Higher `Fret Steps` values pull the slide pitch toward semitone plateaus and add a light fret-crossing contact layer while the slide lane moves. This is still an approximation: same-string speaking-length preservation, explicit slide-in/out tail modes, and notation import remain later phases.

## 2026-05-09 — Add gated Slide Tail modes

Decision:
Expose a `Slide Tail` performance choice with `Normal`, `Muted`, `Open`, and `Slide Off` modes. Non-normal modes only alter release behavior when a voice has recent `Neck Slide` motion.

Reason:
Slide-out gestures need different endings than ordinary note-offs, but always changing release behavior would make normal playing unpredictable. Gating the special release colors behind recent slide motion lets the first tail implementation stay behavior-neutral for ordinary notes while giving DAW automation a way to author muted tails, more open/ringing tails, and slide-off scrapes.

Status:
Superseded by `2026-05-09 — Replace Slide Tail with Slide Lift`

Consequences:
`Normal` remains the default and preserves existing release behavior. `Muted`, `Open`, and `Slide Off` are first-pass release colors, not complete same-string/open-string speaking-length models. The true physical update of string/fret assignment remains in the later same-string slide phase.

## 2026-05-09 — Replace Slide Tail with Slide Lift

Decision:
Remove the host-facing `Slide Tail` mode choice and replace it with a continuous `Slide Lift` parameter.

Reason:
Human DAW audition showed that note-off is the wrong anchor for slide-ending behavior. In Bitwig loop playback, the note boundary can be ambiguous and the EG-065 tail modes did not feel convincing. A slide lift is a more explicit guitar gesture: the player progressively eases fretting pressure while the hand is still sliding.

Status:
Accepted

Consequences:
`Slide Lift` is a 0% to 100% automatable performance slider. At 0%, slides keep full fretted pressure. Higher values fade fret-step engagement, add light lifted string scrape, and damp modal sustain while `Neck Slide` is moving. The lift envelope is motion-driven rather than note-off-driven: 50% reaches its lift over about 500 ms and 100% reaches full lift over about 50 ms.

## 2026-05-09 — Revoice finger noise as stick-slip squeak

Decision:
Model fretting-hand approach/release noises and slide-lift scrape as filtered friction plus short stick-slip squeak impulses, not as a mostly broadband noise layer.

Reason:
Human DAW audition showed the previous noise layer read like white or pink hiss rather than finger-on-string squeak. Friction research points toward intermittent acoustic bursts during slip, and guitar-string friction measurements support differentiating wound and unwound string behavior.

Status:
Accepted

Consequences:
`Finger Noise` remains the existing 0% to 100% control, but internally it now uses string/fret-dependent squeak carriers, impulse spacing, and wound-string weighting. Slide-lift scrape uses the same contact-noise direction. This is still a compact approximation; future work could add explicit string-gauge or sliding-speed spectral calibration from recorded references.

## 2026-05-10 — Drive slide squeak from slide motion

Decision:
Slide squeak must be driven by `Neck Slide` movement speed and current string pitch rather than by a mostly fixed scrape/noise carrier.

Reason:
Human spectrum/listening feedback on EG-067 showed a consistent hiss-like layer with only small ridges, regardless of whether the slide moved quickly or slowly. A real finger/string squeak is a contact event caused by relative motion; it should intensify with speed, soften as speed drops, and stop when the finger stops sliding.

Status:
Accepted

Consequences:
`StringVoice` now keeps a short slide-motion drive envelope. Slide squeak pulse spacing, amplitude, and carrier frequency follow recent `Neck Slide` delta and current string frequency. The random scrape component remains only as a supporting texture and decays quickly after movement stops.

## 2026-05-10 — Separate slide squeak amount from finger noise

Decision:
Expose `Slide Squeak` as a separate Performance-page amount control for motion-coupled slide finger/string noise.

Reason:
Human DAW listening found EG-068 useful enough to keep, but the musical need shifted to balancing the slide squeak against the main guitar volume. The existing `Finger Noise` control governs older note approach/release noises, which are not yet as convincing and should not be required just to balance slide gestures.

Status:
Accepted

Consequences:
`Slide Squeak` defaults to 100%, preserving the EG-068 balance. It ranges from 0% to 200%, scaling only slide contact squeak/scrape output. `Finger Noise` remains available for approach/release noises and can be revisited separately.

## 2026-05-10 — Group Performance controls by primary gesture

Decision:
Reorganize the Performance page so primary gesture controls stay visible while related character/timing controls live under small disclosure rows.

Reason:
The Performance page had enough controls that `Amp Feedback` return-character controls were pushed out of reach. Human feedback also made the control taxonomy clearer: `Neck Slide` is the primary gesture, while `Fret Steps`, `Slide Lift`, and `Slide Squeak` shape that gesture; `Lookahead` is a timing detail for `Finger Noise`; `Return Clip` is a character detail for `Amp Feedback`.

Status:
Accepted

Consequences:
The page now has a two-level structure without changing the existing top-level tabs. Secondary sliders remain column-aligned with primary sliders, but their labels are visually quieter. `Slide Squeak` is capped at 100% with a 20% default based on human audition.

## 2026-05-10 — Split slide squeak by movement direction

Decision:
Keep the existing `slideSqueak` parameter ID as the upward slide-squeak amount, rename it host-facing as `Slide Squeak Up`, and add a new `Slide Squeak Down` parameter for downward slide motion.

Reason:
Human DAW listening found that the previous amount control changed the audible upward slide squeak but did not provide a useful independent balance for down-neck returns. Since slide direction is already known inside the fret/contact update path, exposing separate up/down balances is a small, musically useful extension.

Status:
Accepted

Consequences:
Existing sessions and automation that target `slideSqueak` continue to drive upward squeak. New sessions get both up and down defaults at 20%. The split affects only the slide contact squeak/scrape layer; `Neck Slide` pitch, `Fret Steps`, `Slide Lift`, MPE expression, and the older `Finger Noise` approach/release layer remain independent.

## 2026-05-10 — Use APVTS state chunks plus file presets for the preset model

Decision:
Define the future preset system around the current `AudioProcessorValueTreeState` parameter state, saved as small versioned preset files for in-plugin stock/user preset management.

Reason:
The plugin already has host session recall through `getStateInformation` and `setStateInformation`, while JUCE's program API is currently a single stub program. A file-backed preset layer can sit above APVTS without changing the DSP model or depending on host-specific preset formats.

Status:
Planned

Consequences:
Stock presets should be loaded from bundled resources or an installed stock preset folder, while user presets should live in the user's application support preset folder. Adding user presets should not require recompilation. Updating bundled stock presets can happen through release packaging or a preset-folder installer; embedding them directly in the binary would require a rebuild.

## 2026-05-10 — Start pick realism with one plastic plectrum model

Decision:
Implement one direction-aware normal plastic plectrum model first, with `Pick Stroke` choices for `Down`, `Up`, and `Alternate`, instead of adding a broad pick/finger/nail material menu.

Reason:
Human DAW audition found that strummed chords are already convincing, while isolated single-note picked attacks expose a synthetic tonal chirp. The smallest useful first pass is to improve the ordinary plectrum onset itself: deterministic per-attack variation, stroke direction, and a less tonal default contact/ring layer.

Status:
Accepted

Consequences:
`Pick Stiffness` and `Pick Texture` remain the main continuous pick-character controls. `Pick Stroke` defaults to `Alternate` and only advances on picked note starts, leaving hammer-ons, pull-offs, and taps out of the alternating count. The implementation still intentionally avoids fingerpicking/nail models until the normal plectrum baseline is musically solid.

## 2026-05-10 — Treat pick controls as interaction variables, not sample-layer volume

Decision:
Add `Pick Bite` as the strength of the pick/string contact imprint rather than as a separate pick-noise volume control.

Reason:
In this physical model, the pickup does not hear an independent pick-click track. It hears the string motion caused by the pick displacing, scraping, and releasing the string, then filtered by speaking length, pickup position, and string damping. A user-facing control still needs to balance the audible attack, but the implementation should scale physically interpretable contact terms: edge imprint, scrape, short contact ring, and attack chirp.

Status:
Accepted

Consequences:
`Pick Bite` leaves the main string displacement/release intact at low values and changes the contact imprint layered into the attack. `String Age` now damps the bright pick-contact fingerprint more explicitly because an older string should not present the same clean high-Q contact response as a brand-new string. This pattern should guide future player-interpretation controls: map UI to plausible player/string interactions first, then only use "amount" controls where the physical interaction really needs a balance.

## 2026-05-10 — Make Alternate picking economy-aware across strings

Decision:
Keep the user-facing `Pick Stroke = Alternate` mode, but interpret it as right-hand-aware alternate picking: same-string picked notes flip direction, while cross-string picked notes use the direction implied by movement across the string set.

Reason:
A guitarist does not blindly alternate every note when a phrase is already moving the pick through adjacent or skipped strings. Moving from low E toward high E naturally continues as a downstroke sweep; moving from high E back toward low E naturally continues as an upstroke sweep. The simpler toggle model helped repeated notes but did not model chord strokes, string skipping, or efficient cross-string motion.

Status:
Accepted

Consequences:
No new host parameter is needed. `Alternate` now remembers the last picked string and stroke direction. It still alternates on repeated same-string picked notes, but uses downstrokes for increasing string indices and upstrokes for decreasing string indices. Hammer-ons, pull-offs, and taps do not consume the right-hand stroke state.

## 2026-05-10 — Make player feel deterministic and cause-based

Decision:
Add `Player Feel` as a deterministic performance-interpretation layer driven by cognitive load, dexterity load, and one endurance bucket instead of random DAW-style humanization.

Reason:
Human timing and velocity variation should come from a playable cause: fast repetition, string skips, direction changes, fret jumps, accumulated effort, and recovery. Random note offsets can make playback less robotic, but they do not teach the instrument anything about the player attempting a physical task.

Status:
Accepted

Consequences:
At 0%, the timing/velocity path stays neutral. At higher values, picked note-ons can be delayed slightly and their incoming velocity can be scaled by accumulated load. `Feel Recovery` controls how quickly the load clears, and `Reset Feel` gives the human an immediate "new take" recovery action. This first pass delays only; early timing will require a lookahead-aware follow-up.

## 2026-05-10 — Make Player Feel midpoint natural and exportable

Decision:
Recalibrate `Player Feel` so the musically natural EG-075 maximum sits around the middle of the control, leave the top half for exaggerated/sloppier playing, default `Feel Recovery` to 2 seconds, and expose live load meters plus a copyable settings export.

Reason:
Human DAW audition found that EG-075 at 100% sounded natural, not excessive, and that recovery around 2.5 seconds felt closer to the intended player-load behavior. The plugin also needs a lightweight way to communicate exact audition settings back into Studio handoffs without guessing from screenshots or memory.

Status:
Accepted

Consequences:
`Player Feel` at 50% now approximates the previous natural high setting. `Player Feel` at 100% can get noticeably less precise. `Feel Recovery` ranges up to 8 seconds and defaults to 2 seconds. The UI shows cognitive, dexterity, and endurance meters, and `Export Settings` emits a JSON snapshot of current parameters plus those meters.

## 2026-05-10 — Make Player Feel top half genuinely sloppy

Decision:
Keep the natural-feel midpoint, but reduce load accumulation impulses and give the top half of `Player Feel` a wider nonlinear timing/energy range.

Reason:
Human DAW audition of EG-076 found the gauges useful but too quick to saturate, while `Player Feel = 100%` still sounded natural rather than meaningfully sloppy. The control needs two separate behaviors: meters that have room to breathe, and an upper range that can clearly audition timing imprecision.

Status:
Accepted

Consequences:
Load meters should climb more gradually during demanding passages. `Player Feel` near 50% remains the practical natural range, while 100% can delay picked note starts much more noticeably and widen velocity/energy variation. `Export Settings` is treated as a global audition/preset-context action and now lives in the header instead of inside the Articulation page.

## 2026-05-10 — Treat fast cross-string clusters as strum continuations

Decision:
Before implementing full Auto Strum scheduling, make Player Feel recognize very short cross-string note-on clusters as a continuing right-hand strum stroke and charge them less cognitive/dexterity load.

Reason:
Human DAW audition showed that already-staggered strum MIDI was making the load meters climb as if every string in a chord were a separate difficult picking decision. Physically, a held chord strum is often one right-hand sweep while the left hand mostly holds a shape. The player model should start separating left-hand work from right-hand stroke continuation.

Status:
Accepted

Consequences:
EG-078 reduces the Player Feel load contribution for adjacent or near-adjacent cross-string note starts inside a short strum window. This preserves existing authored strum MIDI while lowering misleading load buildup. A later Auto Strum interpreter can accept simultaneous chord notes and internally schedule string-order timing.

## 2026-05-10 — Start Auto Strum with exact block chords

Decision:
Add `Strum Speed` as an optional exact-same-sample block-chord interpreter before adding a broader near-time collection window.

Reason:
The immediate musical need is to let DAWs author a held chord as simultaneous note-ons and let Guitar AG perform the right-hand sweep. Exact same-sample grouping can be done without introducing input latency. A tolerance window for notes that arrive a fraction of a millisecond apart would require deliberate collection/lookahead behavior and should be designed separately.

Status:
Accepted

Consequences:
`Strum Speed = 0%` preserves current MIDI timing. Higher values fan simultaneous chord note-ons across predicted string assignments using the current `Pick Stroke` direction and schedule those note starts internally. The first pass does not reinterpret already-staggered strums, and it does not yet collect notes that arrive in adjacent samples or small host jitter windows.

## 2026-05-10 — Balance strum direction by reducing one stroke direction

Decision:
Add `Strum Balance` as a bipolar Auto Strum control where center is balanced, positive values reduce upstroke velocity, and negative values reduce downstroke velocity.

Reason:
Human DAW audition found EG-079 Auto Strum musically convincing, but a normal strumming pattern needs adjustable down/up energy difference. Treating this as a reducer avoids unexpected clipping or failed accents when incoming velocities are already high. It also supports ghost-like upstrokes or downstrokes at the extremes without changing the authored MIDI velocity lane.

Status:
Accepted

Consequences:
The control affects generated Auto Strum note-on velocities before Player Feel variation is applied. It does not affect single-note picking or already-staggered authored strums. A center value preserves EG-079 behavior exactly.

## 2026-05-10 — Make articulation defaults audition-ready

Decision:
Group Articulation-page controls around the primary musical action and update new-instance defaults for the release-candidate sound.

Reason:
Human DAW audition found the current strum and feel model musically strong enough to be on by default. A new user previewing the instrument should hear a light right-hand strum and controlled human feel without discovering multiple subordinate controls first. The page should make the most important gesture controls read as parents while keeping character controls close but visually secondary.

Status:
Accepted

Consequences:
`Pick Bite` is the primary pick-character row, with `Pick Stiffness` and `Pick Texture` grouped beneath it. `Pick Stroke` is the primary right-hand direction row, with `Strum Speed` and `Strum Balance` beneath it. `Player Feel` keeps the Bot/Pro/Loose parent row, with recovery/reset/meters beneath it. New instances default to `Pick Texture` 25%, `Strum Speed` 10%, `Strum Balance` -13%, and `Player Feel` 50%. Existing DAW sessions should continue loading their saved parameter states.

## 2026-05-17 — Solve partial-chord voicing with group assignment

Decision:
Keep register affinity as a modest single-note scoring nudge, and use a fixed-size group search for same-sample partial chords smaller than six notes.

Reason:
Close-interval dyads can need a non-greedy string/fret choice. Raising the single-note register weight enough to fix those dyads would fight the moving position memory that makes melodic playing feel coherent. Same-sample partial chords are already a bounded guitar-specific interpretation point, so a small no-allocation group search is the cleaner place to solve the combined grip.

Status:
Accepted

Consequences:
Single-note playing keeps its current position-memory behavior. Partial block chords can choose a better combined layout, such as G4 on string 4 fret 8 plus A4 on string 5 fret 5 after low-position context. Full six-note block chords stay on the established sequential preview path for now.

## 2026-08-27 — Keep the first stateful string experiment offline-only

Decision:
Add the Plan 0090 stateful two-polarization engine only to `GuitarAGOfflineRender`. Keep the VST3 compiled on the legacy modal engine until deterministic A/B renders and human listening justify promotion.

Reason:
The new engine changes the physical state representation and is not yet a production replacement. Compiling its large per-string delay buffers only into the research renderer protects plugin memory/reset cost, host state compatibility, and released audio while still allowing realistic MIDI routing, polyphony, MPE, tone processing, and reference analysis to exercise the prototype.

Status:
Accepted

Consequences:
`--string-engine stateful` and `--stateful-repick 0|1` are offline development controls, not host parameters. The normal renderer path and VST3 remain legacy. A later decision must explicitly promote the stateful engine before it appears in the plugin or saved state.

## 2026-08-27 — Reject Plan 0090 stateful voice promotion

Decision:
Keep the Plan 0090 A/B harness and prototype available for offline research,
but do not promote the current `StatefulStringVoice` to an experimental VST3
and do not extend it with left-hand/fret state yet.

Reason:
In loudness-matched human A/B listening, both the preserve-repick and reset-
repick stateful variants "just sound like a synth." The legacy render still
has a spectral-chirp attack and glassy note body, but the new engine failed the
more fundamental requirement that its isolated notes read as electric-guitar
DI. Determinism, stability, spectral-distance movement, and demonstrated state
continuity do not override that listening verdict.

Status:
Accepted

Consequences:
The production VST3 remains on the legacy modal engine. The next DSP work must
refine or replace the stateful excitation/string-loss/dispersion/pickup/body
combination offline and pass an isolated-note A/B gate before repick continuity
or physical left-hand modeling becomes relevant.

## 2026-08-27 — Improve the accepted modal engine before adding persistence

Decision:
Use the legacy modal engine as the tone foundation for the next realism work.
First repair deep-pick attack behavior, high-register voicing, and finger-motion
noise. Defer persistent modal repicks until the isolated picked note and contact
layers pass listening.

Reason:
Plan 0091 showed that ordinary-setting chirp modes are minor, while removing
explicit overlays removes pick identity without changing the modal-body
character. The high E sounds like the same low-register construction shifted in
pitch, and the independent finger-noise generator sounds like a plastic comb.
Those are more immediate realism failures than note-to-note state continuity.

Status:
Accepted

Consequences:
The completed deep-pick comparison attributed the woody digital rattle to the
explicit transient/contact output rather than the short chirp-mode bank. Replace
that additive path with modal-coupled excitation. Finger noise should be rebuilt
around inferred finger motion and string-coupled friction rather than retuning
the existing ridge oscillator. Modal-state preservation remains valuable, but
it is not the next milestone.

## 2026-08-27 — Use modal force as the attack foundation and subordinate direct texture

Decision:
Continue from the Plan 0092 1.75x modal-force candidate. Add only a low-level,
higher-density version of the current direct pick texture and keep the modal
string response dominant.

Reason:
Human listening found the stronger modal-force direction useful but requested
some of the current attack for material texture. Plan 0091 already showed that
the direct layer becomes a sparse woody digital rattle when it defines the
attack. Blending it quietly and increasing event density tests whether it can
serve as surface detail without reclaiming the onset.

Status:
Accepted for offline A/B.

Consequences:
The next comparison must include the isolated hybrid texture contribution. It
remains offline-only. Register/formant correction is a separate experiment so
the attack verdict is not confounded by a simultaneous body revoice.

Human listening subsequently calibrated 12% direct mix at 2.5x event density as
a good medium setting and 22% as a good maximum. Use 12% as the fixed attack
baseline for the register/formant experiment.

## 2026-08-27 — Keep 35% register anchoring and decouple metal retention

Decision:
Use the Plan 0094 35% absolute-frequency envelope anchor as the offline
foundation for the next register experiment. Do not promote the current combined
control. Split modal decay anchoring from the amplitude envelope and test a
narrow fixed-Hz restoration of inharmonic side-mode energy.

Reason:
Loudness-matched human listening found 35% "pretty good" and much better than
the harmonic-number baseline, establishing that the formant analogy and partial
absolute-frequency correction are useful. The same candidate flattened as pitch
rose and lost metal ring/brightness, so the remaining problem is not solved by
more or less of one scalar. The current control also anchors decay curvature,
which can shorten upper fixed-Hz mode lifetimes, while the anchored amplitude
recipe attenuates side modes that can carry a restrained metal-string ring.

Status:
Accepted for offline A/B.

Consequences:
Hold the accepted 1.75x modal force, 12% direct texture, 2.5x density, and 35%
amplitude anchor fixed. Compare anchored versus harmonic-number decay and a
small plain-string-weighted side-mode restoration as a 2x2 test. Do not restore
the rejected chirp bank or broadly brighten the full harmonic body.

## 2026-08-27 — Keep the combined Plan 0095 direction and calibrate restoration down

Decision:
Use harmonic-number modal decay plus the 6x fixed-Hz side-mode restoration as
the provisional offline tone foundation. Before any production proposal, hold
decay fixed and find the lowest restoration factor that preserves the listening
improvement.

Reason:
Human loudness-matched listening called the combined Plan 0095 candidate
"pretty good." The 6x amount was deliberately chosen to make a literal -60.6 dB
E4 contribution audible, not because sixfold restoration is physically
calibrated. The next decision should therefore minimize that exaggeration rather
than add another synthesis mechanism.

Status:
Accepted for offline calibration.

Consequences:
Compare the selected 6x version against 0x, 2x, and 4x while keeping the 35%
amplitude anchor, harmonic-number decay, and accepted pick attack unchanged.
Include the isolated 6x metal contribution. The single combined verdict does
not by itself prove that either single-axis Plan 0095 candidate was rejected.

## 2026-08-27 — Select 2x metal restoration for the consolidated offline recipe

Decision:
Replace the Plan 0095 6x audibility probe with 2x fixed-Hz side-mode restoration
in the accepted offline tone recipe.

Reason:
Human loudness-matched calibration found 2x "good." It is lower than the 6x
diagnostic while preserving the desired result, so retaining the larger
extrapolation would add unsupported side-mode energy.

Status:
Accepted for the offline promotion gate.

Consequences:
The consolidated recipe is 1.75x modal force, 12% direct texture at 2.5x event
density, 35% amplitude/formant anchoring, harmonic-number decay, and 2x side-mode
restoration. Keep the VST3/default path unchanged until this combination passes
a broader end-to-end comparison at ordinary and deep-pick settings across wound
and plain strings.

## 2026-08-27 — Promote the consolidated tone recipe after Plan 0097

Decision:
Implement the accepted offline recipe in the production `StringVoice` and make
it the VST3/default renderer tone in a separate buildable milestone. Preserve an
explicit offline previous-tone recipe for regression and comparison.

Reason:
Human loudness-matched listening confirmed that the candidate works in the
Plan 0097 stitched gate: ordinary E2–E4 picking, the deep/flexible/textured
failure regime, and a compact riff/arpeggio/upper-melody/chord phrase. The result
therefore generalizes beyond the isolated settings that selected its components.

Status:
Implemented as `StringVoice EG-089 ModalFormantTone`.

Consequences:
Production defaults become 1.75x modal pick force, 12% direct texture at 2.5x
density, 35% amplitude/formant anchoring, harmonic-number decay, and 2x fixed-Hz
side-mode restoration. No new user-facing parameters or preset fields are needed
for this milestone. Update the model identity and expected default hash, keep the
old recipe callable only in the offline renderer, and require Windows Release
VST3/offline builds plus manual installed-plugin verification.

Plan 0098 implemented the recipe without retuning it. The new default exactly
matches all three accepted Plan 0097 section renders, while
`--legacy-tone-recipe previous` exactly matches all three former-production
references and the canonical EG-088 single-note hash.

## 2026-08-28 — Keep interactive reference capture outside the VST3

Decision:
Use a separate JUCE desktop companion for human reference capture. The
companion owns audio-device selection, disk recording, request display,
audition playback, and approval state; none of those responsibilities enter the
plugin processor or its real-time DSP graph.

Reason:
The capture workflow needs a Focusrite input, external files, multiple human
takes, and durable listening judgments. These are research and file-management
concerns, not instrument behavior. Keeping them separate protects the plugin's
real-time and product boundaries while still using the same cross-platform JUCE
device layer.

Status:
Implemented in Plan 0099.

Consequences:
Agent/human interaction uses a schema-versioned request JSON and a resumable
`session.json` containing external WAV filenames, measurements, notes, and
candidate/approved/rejected states. Multiple takes may be approved because
variation is part of the fitting target. Rejected takes remain recoverable by
default, but the human may explicitly erase one after rejecting it. Captures
remain analysis evidence, not playback assets. Muted and ringing takes may be
onset-aligned and compared statistically, but their raw difference is not
defined as pure pick sound.

## 2026-08-28 — Stage the capture inventory and stop after the baseline

Decision:
Use a finite five-phase capture inventory, but ask the human to complete only
Phase 0 and Phase 1 before any later capture work. Inventory completion is driven
by approved take batches, not merely by files existing.

Reason:
An exhaustive pick/string/direction/velocity/depth/material/muting factorial
would consume substantial playing time before proving that the capture method
produces actionable model evidence. One noise-floor reference plus six low/high
E ringing/hand-damped/foam-damped conditions is the smallest useful checkpoint.

Status:
Implemented in Plan 0100.

Consequences:
The app automatically discovers the default inventory, selects the first
incomplete request, shows progress, and retains later response-axis, material,
and finger/surface phases as a visible roadmap. Finish the seven-item starting
checkpoint, analyze it, and revise or continue the later phases only from that
evidence.

## 2026-08-28 — Prefer interface-native ASIO for Windows reference capture

Decision:
Compile JUCE ASIO support into the Windows reference-capture companion and use
the installed `Focusrite USB ASIO` driver as the preferred capture device.
Retain Windows Audio backends as selectable fallbacks and do not force a
specific interface in code.

Reason:
The first real Windows Audio take contained confirmed abrupt discontinuities
and an approximately 8 ms zero-filled gap even though the recorder's disk-writer
queue did not overflow. The interface-native ASIO path keeps input/output clock,
sample rate, buffer configuration, and driver ownership within the Focusrite
driver rather than the shared Windows audio path.

Status:
Implemented in Plan 0101; a clean real-guitar ASIO take remains the human gate.

Consequences:
The Windows capture executable depends on the user installing the Focusrite
driver and on the applicable terms for JUCE's bundled ASIO SDK-derived headers.
Other ASIO clients, including DAWs, should be closed for controlled capture.
The VST3 and offline renderer compile definitions are unchanged.

## 2026-08-28 — Make capture deletion explicit and waveform levels absolute

Decision:
Treat Reject and Delete as separate actions. Reject preserves the take and its
judgment; Delete or Backspace may permanently erase only a selected rejected
take. During recording, those keys abort the take before it enters the manifest.
Show selected WAVs at a fixed full-scale amplitude rather than normalizing each
waveform for display.

Reason:
Repeated reference playing naturally produces obvious duffs that are not worth
retaining, but a single accidental key press must not erase a candidate or
approved reference. Absolute-scale waveforms let the human compare headroom and
spot near-full-scale or unusually quiet captures; normalized previews would make
all takes look equally loud and defeat that calibration purpose.

Status:
Implemented in Plan 0102.

Consequences:
Permanent saved-take deletion requires the take already be rejected and the WAV
be a direct child of the active session directory. Notes, device settings, file
choosers, and background apps suppress capture shortcuts. The waveform shows
peak, RMS, duration, a -12 dBFS guide, and hot/possible-clip cues, but visual
inspection complements rather than replaces listening.
