# Plan 0089 — Stateful Electric-Guitar Realism

## Status

Research and implementation plan prepared on 2026-08-27. No production DSP behavior changes are part of this plan commit.

## Goal

Make Guitar AG modestly but unmistakably more guitar-like by improving the physical causes of attacks, left-hand transitions, and electric-instrument response. Preserve the current `v0.3.1` plugin as the comparison baseline and advance in small A/B-testable milestones.

The immediate target is not a complete scientific guitar simulation. It is a clean DI instrument whose isolated notes no longer expose a synthetic tonal chirp and whose repeated notes, hammer-ons, pull-offs, and fret changes behave like changes to one continuing string rather than unrelated note triggers.

## Baseline Confirmed on Windows

- Canonical Windows checkout: `C:\code\github\guitar-ag`
- GitHub/main baseline: `d6fad1a0ddce650a39595a0c46de1a76676fb587`
- Latest published release: `v0.3.1`
- Visual Studio 2022 x64 Release builds pass for `GuitarAG_VST3` and `GuitarAGOfflineRender` using JUCE at `C:\code\JUCE`.
- Baseline offline render: `build-vs2022-x64/diagnostics/baseline-v031-single-note.wav`
- Guitar-TECHS P1 single-note DI material is available outside the repository under `C:\code\reference-audio\Guitar-TECHS`.

## Architectural Diagnosis

The current class names and storage suggest a waveguide, but the audible `StringVoice` path is now a modal oscillator bank plus independently synthesized contact layers:

- `delayLine` and `secondaryDelayLine` are populated at note start but are not advanced or read during current rendering.
- `renderModalBank()` supplies the sustained note.
- `renderPickTransient()` and `renderContactLayer()` add explicit ring, chirp, grind, burr, impact, and scrape signals.
- `AudioEngine::noteOn()` may infer a legato source, but it releases that source and calls `StringVoice::start()`, which resets modal phases, amplitudes, delay buffers, pickup history, and contact state.

This makes three symptoms structural rather than cosmetic:

1. A pick can sound like a separately audible chirp generator.
2. A slide or finger event can sound like periodic synthetic noise laid over a note.
3. A same-string transition cannot preserve the vibration that existed immediately before the transition.

Commit `3457200` (`KS015 DualPickup`) is an important project-local starting point. Its `StringVoice::renderSample()` advanced two delay lines and read them through finite pickup apertures. Commit `00c8759` promoted the modal cluster after a listening preference at that stage. The proposal is not to revert the plugin; it is to recover the stateful mechanics as an experimental engine and judge them with better excitation, terminations, and reference tests than the original comparison used.

## New Evidence Since the Earlier Realism Pass

### Dynamic contact and nonlinear strings

- Bilbao, Russo, Webb, and Ducceschi's 2024 real-time guitar synthesis work models a string, geometric nonlinearity, smooth fingerboard, point frets, and a dynamic finger collision in one stable real-time system. Its pluck is a finite-duration applied force rather than an additive click: <https://www.pure.ed.ac.uk/ws/portalfiles/portal/470239305/BilbaoEtal2024RealTimeGuitarSynthesis.pdf>
- The 2025 power-balanced SAV work addresses long-term pitch drift under repeated strikes while retaining a real-time, non-iterative update: <https://dafx.de/paper-archive/2025/DAFx25_paper_24.pdf>
- IRCAM's accompanying implementation is GPL-3.0 and useful for evaluation only; Guitar AG must not copy its code into this PolyForm Noncommercial project: <https://github.com/Ircam-RnD/SAV-string-simulations>

### Pick trajectory is not a linear brightness/noise knob

A 2025 robotic picking experiment measured pick depth in 192-micrometre increments. Small trajectory changes affected loudness, harmonic ratios, noisiness, spectral centroid, and spectral flux, with threshold/shelf behaviour that depended on pick material. Past the release threshold, deeper plucks could emphasize lower modes and reduce relative noisiness rather than simply add brightness: <https://vibsys.put.poznan.pl/_journal/2025-36-2/articles/vps_2025205.pdf>

### Electric-guitar termination and pickup identity

- McGill/CIRMMT and Yamaha's 2023–2025 electric-guitar project found that fret-position-dependent neck admittance affects decay and can be represented in a digital waveguide. This supports a small per-fret termination/reflection model instead of one global sustain curve: <https://www.cirmmt.org/en/research/projects/yamaha-rnd_guitar-analysis>
- Recent pickup research reports polarization-dependent magnetic force, amplitude-dependent beating, and pickup-height-dependent roughness. These effects belong after a stable two-polarization string and should remain subtle by default: <https://www.yamaha.com/en/tech-design/research/tag/acoustic-device-material/> and <https://ismra2025.org/wp-content/uploads/2025/05/ISMRA-2025-Final-1.3.pdf>
- A 2025 electric-guitar measurement study also cautions that magnetic pickups have their own temporal low-pass response. Contact sound should therefore drive the string and pass through pickup readout, not bypass it as raw foley: <https://www.mdpi.com/1424-8220/25/21/6514>

### Fitting and player interpretation

- Differentiable Modal Synthesis offers a useful offline parameter-fitting/model-reduction tool for physical modal systems. It is not the preferred runtime core for the next plugin milestone: <https://proceedings.neurips.cc/paper_files/paper/2024/hash/0232cafe8d1909a01019abe8af32f3e1-Abstract-Conference.html>
- Fretting-Transformer and recent MIDI-to-rich-tablature work reinforce the value of context-sensitive, playable string/fret choices: <https://arxiv.org/abs/2506.14223> and <https://arxiv.org/abs/2407.09052>
- Guitar-TECHS provides synchronized DI, amplified audio, motion, and MIDI for controlled guitar techniques and is suitable for reference comparisons: <https://arxiv.org/abs/2501.03720>

## Local Baseline Measurement

The current six-note calibration render was compared with real Guitar-TECHS P1 direct-input single notes using the existing analysis scripts.

- Current-render zero-crossing rates were roughly 4–8 times the matched DI examples.
- The render was substantially more periodic/harmonic and less spectrally evolving than the references.
- The attack, early, and late spectral-distance results were all poor rather than showing an attack-only mismatch.

This is consistent with the listening description: narrow high-frequency crossings produce a chirpy edge while a highly deterministic modal bank remains too clean and static underneath. These metrics are diagnostic clues, not optimization targets; previous project work already showed that analyzer improvements can disagree with listening preference.

## Design Principle

Articulations should be changes to a persistent physical string state.

```text
MIDI / MPE
  -> player interpreter (which hand did what, on which string?)
  -> per-string physical state (speaking length, contacts, residual vibration)
  -> contact forces (pick, finger, fret, palm)
  -> two-polarization string propagation
  -> fret/bridge/neck terminations
  -> magnetic pickup aperture + circuit
  -> clean DI output
```

The interpreter may remain heuristic, but it must describe plausible hand actions. It must not replace authored MPE curves with random gestures.

## Milestone 0 — Protect the Baseline and Build the A/B Harness

Deliverables:

- Keep the current engine as `Legacy Modal` for comparison while experiments are underway.
- Add an offline-only or development-only engine selector before exposing a persisted production parameter.
- Render the same deterministic MIDI and settings through both engines.
- Keep a small reference manifest of matched Guitar-TECHS DI notes and techniques; do not commit the large licensed audio archives.
- Add measurements that emphasize attack duration, spectral evolution, beating, decay slope, and repeated-note continuity in addition to whole-note spectral distance.

Exit criteria:

- Both engines build in Release on Windows.
- A/B renders are level matched and repeatable.
- The legacy path is byte-identical when the experimental engine is not selected.

## Milestone 1 — Stateful String and Plectrum Contact

Start from the project-owned two-delay-line implementation at commit `3457200`, then modernize it rather than reverting main.

Implement:

- two persistent polarization states per string
- fractional-delay pitch control with smoothed MPE movement
- frequency-dependent loop loss and mild dispersion
- pickup-position and finite-aperture velocity readout
- a finite-compliance plectrum contact that applies force until release
- pick direction, depth, stiffness, and angle as coupled contact variables
- deterministic but non-cloned per-attack variation
- `repluck()` that injects new force into residual motion without clearing the string

Remove from the experimental default:

- independently audible pick-ring oscillators
- explicit harmonic chirp banks
- broadband contact noise mixed after string/pickup state

The pick-depth mapping should be nonlinear and bounded. It should include a contact/release threshold and should not make every deeper strike brighter and noisier.

Exit criteria:

- Isolated picked notes sound less chirpy through both clean DI monitoring and an external amp sim.
- Repeated same-note attacks are not cloned, yet the offline render remains deterministic.
- A repick audibly preserves some residual phase/energy from the previous strike.
- No allocation, locks, or unbounded iteration occur on the audio thread.
- Six voices stay comfortably real-time at 44.1 and 48 kHz in the Windows plugin.

## Milestone 2 — Persistent Left-Hand State

Separate MIDI-note ownership from the six physical string states. Track, per string:

- open or fretted state
- fret and continuous contact position
- fretting pressure/contact stiffness
- held, approaching, sliding, or lifting finger state
- current speaking length and residual vibration
- right-hand contact independently of the left hand

Implement transitions as physical actions:

- **Hammer-on:** a moving finger contacts and clamps the already vibrating string at a new fret, injecting collision energy without resetting the whole state.
- **Pull-off:** contact releases with a small lateral displacement and damping change, preserving existing vibration.
- **Slide:** move the contact boundary along the string with pressure-dependent friction and fret interactions; avoid a free-running squeak oscillator.
- **Repeated fretted note:** preserve the held left-hand shape while the right hand repicks.
- **Lift/mute:** change damping/contact state rather than selecting an arbitrary noise tail.

Exit criteria:

- Same-string legato is distinguishable from a fresh pick without a synthetic overlay.
- A repeated note does not imply that the fretting finger lifted between MIDI note-off/note-on when context says the shape remained held.
- String bends and authored MPE expression remain per-note and do not fight the interpreter.

## Milestone 3 — Electric Instrument Response

Add only after Milestones 1 and 2 give a convincing string/contact foundation:

- a compact fret-position-dependent neck/termination loss model
- bridge and nut reflection filters
- calibrated pickup position, finite aperture, and electrical resonance
- subtle two-polarization detuning/beating
- optional amplitude-dependent magnetic-pull/pickup-height effect with conservative defaults
- sympathetic coupling only if listening shows a useful improvement per CPU cost

Exit criteria:

- Fretted notes do not share one generic decay envelope.
- Pickup changes alter harmonic nulls and transient smoothing in a physically legible way.
- Clean DI remains suitable for external amp simulations.

## Milestone 4 — Nonlinear Core Shootout

In an offline single-string prototype, independently implement the equations described in the recent SAV papers and compare them with the stateful two-polarization waveguide. Do not copy GPL implementation code.

Judge:

- attack and fret-collision realism
- repeated-strike pitch stability
- MPE pitch-control practicality
- six-string CPU projection
- numerical stability under extreme input
- human listening preference against Guitar-TECHS examples

Only promote the nonlinear core if it clearly wins by ear and fits the plugin's real-time/control requirements. A recent paper is evidence for feasibility, not an automatic architecture mandate.

## What Not to Do Next

- Do not tune the existing chirp/ring/squeak oscillator gains as the main realism strategy.
- Do not begin with a full body, amp, cabinet, or room model.
- Do not replace the physical model with sample playback or an opaque neural generator.
- Do not expose a large new parameter surface before the contact variables have stable musical meanings.
- Do not optimize solely for one spectral-distance score.

## Recommended First Implementation Slice

Create the Milestone 0 engine A/B seam and a single-string offline `StatefulWaveguide` prototype, then implement compliant pick contact and state-preserving repick. Keep fret collision, magnetic nonlinearity, and new UI controls out of that slice.

Human verification should compare four short cases at matched loudness:

1. one isolated low wound-string note
2. one isolated plain-string note
3. eight repeated picks on the same fretted note
4. a pick followed by a same-string hammer-on

The first three are the acceptance gate. The fourth is allowed to expose the missing Milestone 2 behaviour; it establishes the next baseline rather than blocking Milestone 1.
