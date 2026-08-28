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
- [ ] A six-note guitar-range chord can ring without stealing.
- [ ] A seventh simultaneous note replaces one physical string rather than raising the active string-voice count above six.
- [ ] Repeated notes on the same assigned string replace the previous string voice cleanly.
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

## EG-061 Optimization Listening Checks

- [ ] New plugin instances show `v0.2.6 / StringVoice EG-061 RenderHelpers`.
- [ ] MPE bends, whammy bends, vibrato, and aftertouch bend are smooth and do not audibly step.
- [ ] High `Amp Feedback` still blooms naturally and preserves picked attacks before harmonic takeover.
- [ ] Pick scrape, coin edge, heavy pick rasp, hammer-on/tap impact, and pull-off snap remain natural.
- [ ] With `Lookahead` at 150 ms or 250 ms, delayed notes stay aligned with pitch wheel, mod wheel/CC1, channel pressure, CC74, and key/poly aftertouch.
- [ ] Dense strums, high feedback, and MPE audition sections do not show concerning DAW CPU spikes.

## EG-064 Fret Step Curve Checks

- [ ] New plugin instances show `v0.2.6 / StringVoice EG-064 FretStepCurve`.
- [ ] The Performance page exposes `Neck Slide` with a `-12.0 st` to `+12.0 st` range and a `0.0 st` default.
- [ ] The Performance page exposes `Fret Steps` with a `0%` to `100%` range and a 65% default.
- [ ] At `0.0 st`, existing MIDI clips sound unchanged.
- [ ] Automating `Neck Slide` on a held chord moves the whole chord together.
- [ ] With `Fret Steps` at 0%, `Neck Slide` sounds like the earlier smooth global pitch lane.
- [ ] With `Fret Steps` near 10%, `Neck Slide` already has audible semitone/fret position definition rather than behaving like a pure whammy bend.
- [ ] From 10% to 100%, `Fret Steps` gives usable variation across the audible fret-engagement range instead of hiding the useful range near the top.
- [ ] Moving `Neck Slide` across fret boundaries adds subtle contact ticks/scrape, and holding it static does not produce continuous scrape.
- [ ] MPE per-note pitch bend still moves only the intended note when `Neck Slide` is at `0.0 st`.
- [ ] MPE per-note pitch bend stacks predictably with a nonzero `Neck Slide`.
- [ ] Extreme stacked pitch motion remains bounded and does not produce runaway output or obvious aliasing artifacts.
- [ ] The slide audition MIDI's held-chord automation bed is usable for drawing host `Neck Slide` curves.

## EG-066 Slide Lift Checks

- [ ] New plugin instances show `v0.2.6 / StringVoice EG-066 SlideLift`.
- [ ] The Performance page exposes `Slide Lift` as a slider and no longer exposes `Slide Tail`.
- [ ] With `Slide Lift` at 0%, releases and slides sound like EG-064/normal slide behavior.
- [ ] With `Neck Slide` at `0.0 st`, raising `Slide Lift` does not mute ordinary static notes.
- [ ] During a moving `Neck Slide`, `Slide Lift` fades fret-step engagement and adds light string scrape.
- [ ] At 50%, the lift feels gradual, roughly a half-second pressure release.
- [ ] At 100%, the lift feels quick, roughly a 50 ms pressure release.
- [ ] Bitwig loop playback does not produce the EG-065 loop-boundary CPU spike.
- [ ] Offline smoke: render a short note with `--neck-slide 5.3 --neck-slide-at 0.001 --slide-fret-steps 0.25 --slide-lift 0/1`; the lifted render should differ from the neutral render.

## EG-067 Finger Squeak Checks

- [ ] New plugin instances show `v0.2.6 / StringVoice EG-067 FingerSqueak`.
- [ ] With `Finger Noise` at 0%, note playback remains effectively unchanged.
- [ ] With `Lookahead` enabled and `Finger Noise` raised, approach and release noises read as short finger/string squeaks rather than broadband hiss.
- [ ] Wound-string assignments have stronger ridged friction than plain-string assignments.
- [ ] During `Neck Slide` movement with `Slide Lift` raised, the lifted scrape sounds like finger pressure easing against strings rather than a static noise wash.

## EG-068 Motion-Coupled Slide Squeak Checks

- [ ] New plugin instances show `v0.2.6 / StringVoice EG-068 SlideSqueakMotion`.
- [ ] Fast `Neck Slide` motion produces louder/denser slide squeak than slow motion at the same `Fret Steps`/`Slide Lift` settings.
- [ ] Holding `Neck Slide` static does not leave a continuous hiss layer.
- [ ] The slide squeak changes pitch/color as the held note or chord moves, rather than staying at one fixed pitch.
- [ ] `Finger Noise` approach/release events remain quieter and less broadband than EG-067.

## EG-069 Slide Squeak Control Checks

- [ ] New plugin instances show `v0.2.6 / StringVoice EG-069 SlideSqueakControl`.
- [ ] The Performance page shows `Slide Squeak` under the slide controls.
- [ ] `Slide Squeak` at 0% mutes slide contact squeak/scrape without disabling `Neck Slide`, `Fret Steps`, or `Slide Lift` damping.
- [ ] `Slide Squeak` at 100% matches the EG-068 balance.
- [ ] `Slide Squeak` does not change the older `Finger Noise` approach/release layer.

## EG-070 Performance UI Checks

- [ ] New plugin instances show `v0.2.6 / StringVoice EG-070 PerformanceUi`.
- [ ] `Slide Squeak` is a 0% to 100% slider and defaults to 20%.
- [ ] The Performance page shows primary rows for `Fret Pressure`, `Aftertouch Bend`, `Neck Slide`, `Finger Noise`, and `Amp Feedback`.
- [ ] The `Neck Slide` disclosure reveals `Fret Steps`, `Slide Lift`, and `Slide Squeak`.
- [ ] The `Finger Noise` disclosure reveals `Lookahead`.
- [ ] The `Amp Feedback` disclosure reveals `Return Clip`.
- [ ] Secondary rows keep the same slider alignment and do not push `Amp Feedback` out of reach.

## EG-071 Slide Squeak Direction Checks

- [ ] New plugin instances show `v0.2.6 / StringVoice EG-071 SlideSqueakDirection`.
- [ ] The `Neck Slide` disclosure reveals `Fret Steps`, `Slide Lift`, `Squeak Up`, and `Squeak Down`.
- [ ] `Squeak Up` defaults to 20%, maxes at 100%, and controls upward `Neck Slide` squeak/scrape.
- [ ] `Squeak Down` defaults to 20%, maxes at 100%, and controls downward `Neck Slide` squeak/scrape.
- [ ] Setting `Squeak Up` to 0% while leaving `Squeak Down` raised mutes upward slide contact noise without muting downward returns.
- [ ] Setting `Squeak Down` to 0% while leaving `Squeak Up` raised mutes downward slide contact noise without muting upward throws.
- [ ] Existing saved sessions that had the old `Slide Squeak` parameter recall that value as `Slide Squeak Up`.

## Pickup Checks

- [ ] `Single Coil` remains narrow and bright.
- [ ] `Humbucker` remains thicker and smoother than `Single Coil`.
- [ ] `Singles OOP` is thinner/nasal from two wider-spaced pickup positions, not just a very low-output humbucker notch.
- [ ] Moving `Pickup Position` changes the `Singles OOP` cancellation pattern while keeping a two-pickup character.

## Amp Feedback Checks

- [ ] `Amp Feedback` at 0% leaves normal sustain behavior unchanged.
- [ ] Moderate `Amp Feedback` extends held-note resonance without obvious clipping.
- [ ] High `Amp Feedback` lets one dominant harmonic band build instead of evenly boosting every string.
- [ ] High `Amp Feedback` on a held chord lets one physical string/harmonic take over more than the others.
- [ ] At high `Amp Feedback`, a freshly picked chord keeps a clear attack before feedback blooms into the sustain.
- [ ] Repeated or strummed note attacks temporarily reset the feedback winner instead of letting the previous howl dominate immediately.
- [ ] New plugin instances open with `Distorted Return` enabled.
- [ ] `Distorted Return` On makes feedback react more naturally/aggressively without distorting the main clean DI output by itself.
- [ ] `Distorted Return` Off remains available as a cleaner alternate return.
- [ ] Long held single notes at 100% feedback remain bounded and do not run away into full-scale clipping.

## Lookahead Finger Noise Checks

- [ ] `Lookahead` Off feels immediate for live playing.
- [ ] `Lookahead` 150 ms and 250 ms report latency to the DAW.
- [ ] In a DAW render, compensated note onsets still line up with the MIDI grid.
- [ ] With `Lookahead` enabled, pitch wheel, mod wheel, channel pressure, CC74, and key/poly aftertouch remain aligned with the delayed notes.
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
- [ ] With `MPE Mode` On, pitch wheel on channel 1 still bends all active notes globally.
- [ ] With `MPE Mode` On, pitch wheel on member channels 2-16 still behaves as per-note MPE bend.
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

## Pick Stroke Plectrum Checks

- [ ] New plugin instances show `v0.2.6 / StringVoice EG-074 EconomyPickStroke`.
- [ ] The Articulation page exposes `Pick Stroke` with `Down`, `Up`, and `Alternate`.
- [ ] The Articulation page exposes `Pick Bite` as a 0% to 100% control.
- [ ] `Alternate` is the default on new plugin instances.
- [ ] Repeated single-note lines at the default pick settings sound less like a tonal digital/FM attack than EG-071.
- [ ] Lowering `Pick Bite` reduces the pick-contact imprint without muting the main string note.
- [ ] Raising `Pick Bite` makes the edge/contact attack more pronounced without turning the normal midpoint into a brittle/glassy pick.
- [ ] Raising `String Age` dulls the pick-contact brightness as well as the sustained string body.
- [ ] `Pick Stroke = Down` and `Pick Stroke = Up` produce audible but bounded differences in the attack and string response.
- [ ] `Pick Stroke = Alternate` alternates repeated same-string picked notes.
- [ ] `Pick Stroke = Alternate` uses downstrokes when a picked phrase moves from low E toward high E, including string skips.
- [ ] `Pick Stroke = Alternate` uses upstrokes when a picked phrase moves from high E toward low E, including string skips.
- [ ] `Pick Stroke = Alternate` does not turn hammer-ons, pull-offs, or taps into alternating picked attacks.
- [ ] Drag `tests/midi/guitar-ag-pick-stroke-audition.mid` into a DAW and audition it with `Pick Stroke = Alternate`, `Legato Articulation = 0%`, and `Pick Bite` around 20% to 60%.
- [ ] Compare the pick-stroke audition file against forced `Down` and forced `Up` to judge whether `Alternate` feels like right-hand travel rather than a mechanical toggle.
- [ ] In the pick-stroke audition chord sections, strummed notes should feel like a held left-hand chord being re-articulated by the right hand, not like the fretting hand releases every note between strokes.
- [ ] `Player Feel` at 0% sounds and renders like the current neutral picked path.
- [ ] New plugin instances show `v0.3.1 / StringVoice EG-089 ModalFormantTone`.
- [ ] With `Input Octave = DAW E2=52`, a DAW piano-roll E2 plays the same low guitar E that previously required the DAW's E1 label in Bitwig.
- [ ] With `Input Octave = MIDI E2=40`, MIDI note 40 plays the low guitar E directly for standard/scientific MIDI files.
- [ ] In both `Input Octave` modes, the matching note-off releases the transposed note without leaving a stuck voice.
- [ ] The `Player Feel` slider shows Bot, Pro, and Loose landmarks at 0%, 50%, and 100%.
- [ ] Raising `Player Feel` adds picked-note timing/energy variation that grows during fast repetition, string skips, and awkward changes.
- [ ] `Player Feel` around 50% lands near the accepted natural EG-075 100% feel.
- [ ] `Player Feel` near 100% overshoots into visibly/audibly sloppier playing without breaking the phrase, with timing variation clearly larger than 50%.
- [ ] `Feel Recovery` defaults to 2.0 seconds and can be raised beyond 4.0 seconds, up to 8.0 seconds.
- [ ] Shorter `Feel Recovery` clears the effect quickly after rests or easier passages; longer recovery lets the load carry forward.
- [ ] `Reset Feel` immediately clears accumulated feel state for a new take.
- [ ] Cognitive, Dexterity, and Endurance meters rise during demanding picking without immediately pinning to 100%, then clear during rests/easier passages.
- [ ] Staggered chord strums raise Player Feel meters less aggressively than fast single-note runs at a similar note density.
- [ ] Header `Export Settings` opens a copyable JSON blob containing current parameter values and Player Feel meter values.
- [ ] Header `Export Settings` has enough vertical space and does not overlap the title/subtitle/header text.
- [ ] Re-rendering the same MIDI with the same `Player Feel` settings remains deterministic.
- [ ] `Pick Stiffness` and `Pick Texture` remain useful after the plastic-plectrum retune.
- [ ] Re-rendering the same MIDI with the same `Pick Stroke` setting remains deterministic.
- [ ] The Articulation page shows `Pick Bite` as the primary pick-character row, with `Pick Stiffness` and `Pick Texture` grouped beneath it.
- [ ] The Articulation page shows `Pick Stroke` as the primary right-hand direction row, with `Strum Speed` and `Strum Balance` grouped beneath it.
- [ ] The Articulation page shows `Player Feel` as the primary feel row, with `Feel Recovery`, `Reset Feel`, and the three load meters grouped beneath it.
- [ ] New plugin instances default to `Pick Texture` 25%, `Strum Speed` 10%, `Strum Balance` -13%, and `Player Feel` 50%.
- [ ] `Strum Speed` at 0% leaves same-time block chords simultaneous.
- [ ] Raising `Strum Speed` fans exact same-sample chord note-ons across assigned strings.
- [ ] `Strum Balance` centered preserves EG-079 generated strum velocities.
- [ ] Positive `Strum Balance` makes generated upstroke block strums softer while leaving downstrokes at the authored velocity.
- [ ] Negative `Strum Balance` makes generated downstroke block strums softer while leaving upstrokes at the authored velocity.
- [ ] `Pick Stroke = Down` makes block chords fan low-to-high; `Pick Stroke = Up` makes them fan high-to-low.
- [ ] `Pick Stroke = Alternate` alternates repeated block-chord strokes while still using economy direction between chord areas.
- [ ] Single-note runs remain unchanged by `Strum Speed` and `Strum Balance`.
- [ ] Drag `tests/midi/guitar-ag-auto-strum-audition.mid` into a DAW and compare `Strum Speed` at 0%, around 40-70%, and 100%.
- [ ] D2 alone sounds as actual D2 rather than E2 transposed by assignment.
- [ ] D2+A2 at `Strum Speed = 0%` feels like an open Drop D power chord with no generated strum delay.
- [ ] D2+A2 with nonzero `Strum Speed` keeps the open Drop D grip while fanning across strings.
- [ ] E2+B2 in DAW-label mode rings as low E plus B on the A string, not as a stolen/reused low-string note.
- [ ] E2+B2 in DAW-label mode still uses low E plus A-string B when same-sample MPE pitch wheel, CC74, and pressure resets are present.
- [ ] With `Legato Articulation = 100%`, E2+B2 in DAW-label mode still rings as two independent legal chord tones rather than becoming a hammer-on/tap on the low E string.
- [ ] After automating `Neck Slide` up and back to `0.0 st`, stopping/rewinding the DAW does not leave later E2+B2 assignment stuck or transposed.
- [ ] Host All Notes Off / All Sound Off messages clear active voices and fretboard occupancy.
- [ ] Clicking `Panic Reset` on the Setup page clears stuck voices and fretboard assignment state.
- [ ] Automating the `Panic Reset` parameter from 0% to 100% triggers the same cleanup path.
- [ ] The header string strip lights each occupied string and shows note/fret labels; amber indicates mapper occupancy without a matching active voice.
- [ ] `Copy Log` copies a JSON diagnostics snapshot with the last 1000 MIDI/assignment events.
- [ ] G4+A4 after a low-position phrase stays compact on upper strings rather than spreading awkwardly.
- [ ] Ab3+Bb3 after low-position context stays compact in the mid register.

## MPE Pitch Bend Checks

- [ ] `MPE Mode` Off keeps pitch wheel operating as global whammy.
- [ ] `MPE Mode` On makes member-channel pitch wheel act per MIDI channel.
- [ ] `MPE Mode` On reserves channel 1 pitch wheel for global whammy in lower-zone MPE.
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
- [ ] In normal MIDI mode, channel pressure and CC74 apply globally to all active voices.
- [ ] Toggling `MPE Mode` clears stale per-channel pitch bend, pressure, and CC74 state.

## Feature Audition MIDI Checks

- [ ] Drag `tests/midi/guitar-ag-feature-audition.mid` into a DAW instrument track using Guitar AG.
- [ ] Enable `MPE Mode` and leave `MPE Bend Range` at `48.0 st`.
- [ ] Confirm the open-string and chord sections sound like the current expected clean-DI model.
- [ ] Confirm the short riff releases cleanly.
- [ ] If `Mod Wheel To Depth` or `Mod Wheel To Speed` is enabled, confirm the CC1 segment adds vibrato.
- [ ] Confirm the key/poly aftertouch segment bends only the chord's top note.
- [ ] Confirm the MPE pitch bend segment bends only the middle note of the held chord.
- [ ] Confirm the MPE pressure and CC74 segment changes held-note tone/sustain without changing every note at once.

## Plan 0090 Offline Stateful-String Checks

- [x] Build both Windows Release targets; the VST3 excludes the experimental engine and the offline renderer includes it.
- [x] Render `single-note-calibration.mid` without `--string-engine`; its WAV SHA-256 remains identical to the pre-Plan-0090 baseline.
- [x] Render the same stateful fixture twice and confirm byte-identical WAV output.
- [x] Render `guitar-ag-stateful-waveguide-audition.mid` with `--stateful-repick 1` and `0`; audio before the repeated-F2 section is identical and the repeated-note section differs.
- [x] Render at both 44.1 and 48 kHz without instability or invalid samples.
- [x] Render the Auto Strum audition with six active stateful voices; peak remains below 0 dBFS and offline performance remains comfortably faster than real time.
- [x] Listen to the loudness-matched legacy and stateful focused renders in the A/B tool.
- [ ] Repeat the comparison through the project's normal external amp-sim chain.
- [x] Decide whether the isolated E2/E4 attacks have less separate tonal chirp and whether the less-dense stateful body is acceptable: no; both stateful variants read as synths rather than guitar DI.
- [x] Compare preserved and reset F2 repicks: neither variant passed the overall timbre gate, so repick continuity does not justify promotion.
- [ ] Treat the E2-to-G2 section as a known missing left-hand model, not a Plan 0090 acceptance gate.

## EG-089 Production-Tone Checks

- [x] Windows Release VST3 and offline-render targets compile with the promoted
      modal excitation and register/formant path enabled in production.
- [x] No-flags offline renders exactly match the accepted Plan 0097 ordinary,
      deep-pick, and compact-phrase section renders.
- [x] `--legacy-tone-recipe previous` exactly matches the three Plan 0097 EG-088
      references and the canonical EG-088 single-note hash.
- [x] Repeated EG-089 single-note renders are byte-identical at SHA-256
      `32AF0A4AE9A04B3E77F74161E963D4AE3E8431433012D642EA13A96C2C5C47D7`.
- [ ] Install the Release VST3 and confirm the editor reads
      `v0.3.1 / StringVoice EG-089 ModalFormantTone`.

## Plan 0099 Reference-Capture Checks

- [x] Windows Release `GuitarAGReferenceCapture` target compiles and launches.
- [x] A generated schema-version 1 request loads and creates a valid external
      `session.json` with the active Focusrite device metadata.
- [x] Request-generator output parses and preserves the requested take count and
      structured pick-contact context.
- [x] Reopening code restores existing WAV-backed take decisions from the
      manifest rather than replacing them with a blank session.
- [ ] Select the Focusrite instrument input, set 48 or 96 kHz, and confirm the
      meter follows only the intended guitar channel.
- [ ] Record two short takes and confirm each file is mono 24-bit PCM at the
      selected device sample rate with no writer-overflow warning.
- [ ] Play both takes through the selected output and confirm there is no live
      software-monitor feedback path.
- [ ] Approve one take, reject the other, add notes, restart the app with the
      same request, and confirm all states return.
- [ ] Run `summarize-reference-capture-session.py` and confirm it validates the
      WAV headers and prints only the approved file with `--approved-paths`.
- [ ] Record the first ringing/hand-damped Focusrite comparison set;
      do not interpret direct subtraction as a pure isolated pick signal.

## Plan 0100 Capture-Inventory Checks (Historical)

Plan 0103 supersedes this 30-item inventory for current capture work.

- [x] Dependency-free inventory generation produces 30 unique, stable request
      IDs across five ordered phases.
- [x] The default Windows inventory resolves through the system Documents known
      folder, including OneDrive-redirection on the current workstation.
- [x] Launching the app without arguments discovers the default inventory and
      creates/resumes the first incomplete request session.
- [x] A runtime smoke test with a seeded approved Phase 0 take advances the app
      to the first Phase 1 request and preserves the approval.
- [x] Re-running inventory generation without `--force` refuses to overwrite the
      current definitions.
- [ ] The human confirms that the inventory selector, `[done]` labels, current
      phase, request instructions, and research reason are legible at normal UI
      size.
- [ ] With the capture app foreground, Space starts a take and the next Space
      stops it without requiring mouse focus on either transport button.
- [ ] Space types normally in selected-take notes and does not toggle recording
      while Audio Settings has keyboard focus.
- [ ] Pressing Space in another foreground application does not control the
      background capture app.
- [x] Preserve the Plan 0100 generator/history while replacing the active
      inventory with Plan 0103's focused low-E gate.

## Plan 0101 Reference-Capture ASIO Checks

- [x] Windows Release `GuitarAGReferenceCapture` compiles with `JUCE_ASIO=1`.
- [x] The current Windows registry exposes `Focusrite USB ASIO` to 64-bit
      applications.
- [ ] With Codex dictation, DAWs, browsers, and conferencing apps closed, open
      Audio Settings and select `ASIO` / `Focusrite USB ASIO`.
- [ ] Enable only the guitar input, select 48 kHz and a 1024-sample diagnostic
      buffer, and confirm the input meter follows the instrument.
- [ ] Record a ten-second noise floor and several isolated picks. Confirm there
      are no audible clicks, zero-filled gaps, or writer-overflow warnings.
- [ ] Restart the capture app and confirm the Focusrite ASIO device, sample
      rate, buffer size, and enabled channel are restored.

## Plan 0102 Capture Discard and Waveform Checks

- [x] Windows Release `GuitarAGReferenceCapture` builds with the discard and
      waveform changes.
- [x] In a disposable session, Delete and Backspace each erase one selected
      rejected WAV and remove exactly that take from `session.json`.
- [x] Candidate and approved disposable takes survive Delete/Backspace.
- [x] Delete and Backspace each abort an active disposable ASIO recording,
      remove the partial WAV, and leave the manifest take count unchanged.
- [x] A selected valid WAV renders across its full duration at absolute scale
      with peak, RMS, duration, and -12 dBFS guide visible.
- [ ] Confirm Delete/Backspace edit selected-take notes normally and do not fire
      while Audio Settings, a file chooser, or another app has focus.
- [ ] With a real guitar ASIO take, compare the waveform against the audible
      result and confirm quiet, target-level, hot, and clipped-looking examples
      are visually useful.

## Plan 0103 Low-E Capture Exercise Checks

- [x] Generator emits seven items across Phase 0 and Phase 1.
- [x] The six Phase 1 stroke counts are 4, 4, 12, 6, 6, and 12.
- [x] Every Phase 1 request requires natural timing without a metronome and only
      one approved batch; a second batch is optional.
- [x] No active inventory request requires foam or cloth.
- [x] Fresh request IDs prevent earlier single-event ringing/hand-damped
      approvals from completing the revised exercise.
- [x] `--force` regeneration changed zero of the 19 existing session files.
- [x] The regenerated default inventory routes first to
      `low-e-eval-ringing-down` after the approved noise floor.
- [x] Complete all six low-E exercise items, then stop for the current-model
      comparison before expanding the inventory.

## Plan 0104 Low-E Reference/Model Comparison Checks

- [x] Validate all 12 approved exercise WAVs as mono 24-bit/44.1 kHz with no
      clips, writer-overflow flags, or multi-sample exact-zero gaps.
- [x] Detect and visually verify all 84 usable reference attacks, preserving
      the performed six-stroke ringing and six-event damped-alternate batches.
- [x] Render matching production EG-089 down/up/alternate and damped exercises
      at 44.1 kHz.
- [x] Compare attack spectrum, early body, long decay, direction balance,
      variation, and damping contrast at event level.
- [x] Ablate the direct pick mix, attack modes, pick transient, and contact layer
      to confirm that the residual is not an exposed additive pick-click layer.
- [x] Re-render with the confirmed neck humbucker using the virtual Humbucker /
      neck-side pickup and revise conclusions from the matched evidence.
- [x] Add an offline fixed-pluck-position bracket, then test slower decay and
      require human listening before any production promotion.
- [ ] In the DAW, replay the ordinary/deep/phrase material and confirm the plugin
      has the promoted track-2 tone without a level, note-release, or polyphony
      regression.

## Plan 0105 Low-E Reference Candidate Checks

- [x] Windows Release `GuitarAGOfflineRender` builds with fixed pluck-position
      and harmonic-body decay-time controls.
- [x] Omitting both controls reproduces the Plan 0104 pickup-matched downstroke
      WAV byte-for-byte.
- [x] Windows Release VST3 builds with the offline compile definition absent and
      exposes neither research control.
- [x] A 0.10-0.22 position bracket plus a tighter 0.12-0.16 bracket selects 0.14
      from onset-aligned attack measurements.
- [x] A 1.3x/1.5x/1.8x/2.1x body-decay bracket selects 1.5x as the cross-context
      compromise.
- [x] Current and candidate A/Bs use identical MIDI and humbucker/neck settings,
      are 18.9 seconds each, RMS-matched to -18 dBFS, and do not clip.
- [x] Human listening confirms whether the candidate is materially more guitar-
      like rather than only darker and longer.
- [x] Promote nothing and request no next-string capture until that verdict.

## Plan 0106 Legacy Repick De-click Checks

- [x] Measure the exact note-on boundary and the following 5 ms rather than
      assuming the largest discontinuity occurs on the MIDI event sample.
- [x] Direct-layer ablations leave the restart spike unchanged.
- [x] Bracket 1, 3, 5, and 8 ms outgoing-modal-tail transitions and select the
      shortest equally effective value, 1 ms.
- [x] Default/neutral renderer output remains byte-identical with the new flag
      omitted.
- [x] The 1 ms render repeats byte-identically.
- [x] Windows Release VST3 and offline renderer build; the VST3 excludes the
      offline tail storage and flag.
- [x] The 5.85-second control/candidate pair is RMS-matched to -18 dBFS with
      less than 0.001 dB gain difference and no clipping.
- [x] Human listening confirms tick removal without a soft, doubled, or flammed
      legitimate pick attack.

## Plan 0107 High-E Capture Checkpoint Checks

- [x] Inventory generator emits 10 unique items across three phases.
- [x] New high-E IDs are ringing down, ringing up, and down-first alternate only.
- [x] High-E stroke counts are 6, 6, and 12; one approved batch is required and
      a second is optional.
- [x] Every high-E request records the EVH Wolfgang Special, neck humbucker,
      midpoint picking location, likely-.009 string set/condition, and worn
      medium celluloid-style pick with uncertainty preserved.
- [x] Forced regeneration preserves all 43 existing session files byte-for-byte.
- [x] Existing approvals make `high-e-eval-ringing-down` the first incomplete
      inventory item.
- [x] Complete all three high-E items, then stop before A/D/G/B capture for the
      endpoint model comparison.

## Plan 0108 Low/High-E Endpoint Candidate Checks

- [x] All six high-E WAVs validate as 44.1 kHz/24-bit with no clipping or
      dropped-audio flag; each contains the expected 6/6/12 attacks.
- [x] Default and explicit-neutral new renderer controls reproduce the prior
      high-E production render byte-for-byte.
- [x] Candidate low-E down/up/alternate renders reproduce the accepted Plan
      0105/0106 low-E renders byte-for-byte.
- [x] Windows Release `GuitarAGOfflineRender` and `GuitarAG_VST3` build.
- [x] Listening tracks are aligned, mono 44.1 kHz/24-bit, RMS-matched, and do
      not clip.
- [x] Human listening rejects the endpoint candidate because it is dark, large,
      flat, and less convincing than the preferred production high E.

## Plan 0109 Transient-Preserving Endpoint Checks

- [x] Add a separate 18 ms initial-onset spectrum alongside the 60 ms attack.
- [x] New upper-partial decay changes modal persistence rather than start
      amplitude and is smoothly disabled at low E.
- [x] Transient candidate low-E down/up/alternate renders are byte-identical to
      the accepted Plan 0105/0106 files.
- [x] Generate actual-reference versus accepted-candidate low-E listening files.
- [x] Generate real-reference/production/rejected/transient high-E listening
      files in both useful orders.
- [ ] Human judges the low-E accepted candidate against the actual low-E guitar.
- [ ] Human judges whether the transient candidate reduces glassiness without
      repeating Plan 0108's dark/flat failure.
