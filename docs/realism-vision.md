# Realism Vision and Research Notes

## Purpose

This document captures the longer-term vision for making Guitar AG feel and sound like a realistic electric guitar instrument while the implementation still proceeds in small, testable milestones.

The key idea is that realism is not only a better string algorithm. It is the combination of:

- a responsive physical/electrical sound model
- a guitar-aware player interpretation layer
- expressive MPE control that can drive both of those layers without flattening performance detail

## Working Thesis

Guitar AG should eventually behave less like a keyboard synthesizer with a plucked preset and more like a virtual guitar being played by a plausible guitarist.

That means the system needs two interacting models:

1. The instrument model: string vibration, pick/finger excitation, fretting, damping, collision, pickup position, pickup width, pickup/circuit coloration, and clean DI output.
2. The player model: string choice, fret choice, hand position, pick direction, legato, slides, bends, vibrato, muting, repeated-note handling, and context-sensitive expressive variation.

The MVP should still stay simple. The value of this document is to keep the larger target visible while each milestone remains small.

## Research Themes

### Digital Waveguide String Models

Digital waveguides remain the most practical starting point for real-time plucked strings. They naturally represent traveling waves, delay length as pitch, pluck position, pickup position, damping, and dispersion-like filtering.

Useful hypotheses:

- Karplus-Strong can be the first audible step, but a richer waveguide should follow quickly.
- Pickup position and pluck position should be modeled as positions along the string, not merely brightness knobs.
- Frequency-dependent damping is likely essential for avoiding the generic plucked-synth sound.
- Fractional delay and smoothed delay changes will matter for bends, vibrato, and MPE pitch movement.

Relevant references:

- Julius O. Smith's digital waveguide work remains foundational background for the string model.
- Sanders and Weiss's guitar synthesis project is older but useful because it explicitly notes that a basic waveguide resembles a plucked string before it resembles a guitar: https://www.ee.columbia.edu/~ronw/dsp/
- The 2021 ICMC paper "Guitar Virtual Instrument using Physical Modelling with Collision Simulation" uses a finite-difference string with damping/stiffness plus a collision system for articulations: https://www.researchgate.net/publication/346562874_Guitar_Virtual_Instrument_using_Physical_Modelling_with_Collision_Simulation

### Pick, Finger, and Collision Interactions

The strongest clue from the research is that realistic guitar tone depends heavily on interactions at contact points. The pick or finger is not only a velocity value. The fretting hand is not only a pitch selector. Frets, fingers, and strings collide, clamp, slide, damp, and inject energy.

Useful hypotheses:

- A pick model should eventually have direction, position, hardness, release time, and scrape/noise components.
- A fretting model should eventually include contact time, clamping strength, fret collision/buzz thresholds, and damping from unused fingers.
- Hammer-ons and pull-offs are different energy-injection events, not just legato note changes.
- Palm muting and left-hand muting should be separate damping mechanisms with different tonal signatures.

Relevant references:

- Evangelista and Eckerholm describe player/string and string/object interactions using scattering junctions inside a digital waveguide, including plucking, harmonics, fret/fingerboard collisions, and touch: https://liu.diva-portal.org/smash/record.jsf?pid=diva2:316228
- Evangelista's DAFx 2011 paper models string-fret interaction with two polarization modes, clamping/friction, and dynamic detuning from string elongation: https://www.dafx.de/paper-archive/details/V4Cf7-bGBHWqxAXcEo5eEQ
- Grimes's PLOS ONE paper models string bending, vibrato, fretting force, and whammy-bar behavior, and is useful for translating expressive pitch gestures into physically plausible string behavior: https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0102088

### Electric Pickup and DI Tone

The pickup is not just a volume tap. It changes harmonic balance through position, finite sensing width, pickup mixing, circuit resonance, and magnetic nonlinearity.

Useful hypotheses:

- The first pickup model can be a position-dependent comb/filter stage after the string.
- Pickup width should low-pass or smooth spatial detail, reducing some high harmonic content.
- Bridge/neck behavior should emerge from pickup position and filtering before it becomes a preset label.
- Nonlinear pickup behavior is probably not the first milestone, but it belongs on the roadmap.

Relevant references:

- Paiva, Pakarinen, and Välimäki summarize pickup position, sensitivity width, pickup mixing, resonant filtering, and magnetic-flux distortion models: https://www.researchgate.net/publication/234034228_Acoustics_and_Modeling_of_Pickups
- Novak et al. model nonlinear behavior of a guitar pickup at low frequencies and discuss physical versus black-box nonlinear approaches: https://www.mdpi.com/2076-3417/7/1/50
- Rau, Abel, James, and Smith's 2021 JASA work on pickup processing and hexaphonic guitar signals is useful background for string-specific pickup processing: https://pubmed.ncbi.nlm.nih.gov/34340520/

### Human Player Interpretation Layer

MIDI and MPE can contain very expressive detail, but a guitar instrument still needs guitar-specific interpretation. The same MIDI pitch may correspond to multiple strings and frets. The same pitch transition may be picked, hammered-on, pulled-off, slid, bent, or re-fretted.

Useful hypotheses:

- Add a future layer between MIDI/MPE routing and voice events: `PerformanceInterpreter`.
- It should convert notes and per-note expression into guitar gestures, not directly into oscillator commands.
- It should be optional and transparent at first, so authored MPE can pass through without unwanted "help."
- It can begin rule-based, then later learn from data or accept explicit string/fret/articulation metadata.

Possible input signals:

- note timing, pitch, velocity, duration
- MPE pitch curves
- per-note pressure and CC74
- overlaps between notes
- string/fret hints from future UI, MIDI channels, or custom metadata
- style controls such as picked, legato, slide-friendly, bend-friendly, muted, or humanized

Possible output gestures:

- string assignment
- fret position
- pick direction and pick position
- pluck force and pick hardness
- hammer-on, pull-off, slide, bend, vibrato, mute, release
- fretting contact time and damping strength

Relevant references:

- "From MIDI to Rich Tablatures" proposes converting simple MIDI melodies into guitar tablature enriched by fingering, articulations, and expressive techniques using optimization and stylistic data: https://smcnetwork.org/smc2024/papers/SMC2024_paper_id54.pdf
- The 2024 electric bass synthesis paper argues that deterministic systems follow MIDI too literally and that human-like performance needs fluctuations and technique controllability: https://link.springer.com/article/10.1186/s13636-024-00327-9
- The Frontiers review on computational expressive music performance is useful background for timing, dynamics, articulation, and performance-rendering models: https://www.frontiersin.org/articles/10.3389/fdigh.2018.00025/full

### Neural and Hybrid Directions

The project should not start as a neural black box. But recent work suggests useful hybrid directions:

- Use physical modeling for real-time controllability and MPE responsiveness.
- Use neural or differentiable models later for parameter fitting, gesture interpretation, or residual/timbre correction.
- Treat neural results as research inspiration unless they can run predictably in a VST3 real-time context.

Relevant references:

- DAFx 2024 DDSP guitar work explores waveform synthesis from string-wise MIDI input and finds that direct prediction of synthesis parameters from MIDI performed best among their tested systems: https://research.aalto.fi/en/publications/ddsp-based-neural-waveform-synthesis-of-polyphinic-guitar-perform/
- The 2024 NeurIPS Differentiable Modal Synthesis paper is not guitar-specific, but it is relevant to learning physically grounded nonlinear string motion from mechanical parameters and pitch: https://proceedings.neurips.cc/paper_files/paper/2024/hash/0232cafe8d1909a01019abe8af32f3e1-Abstract-Conference.html
- Neural electric bass guitar synthesis with attack-sustain technique representation is useful for thinking about plucked-string attacks and sustained tone as separately controllable musical events: https://link.springer.com/article/10.1186/s13636-024-00327-9

## Proposed Long-Term Architecture

```text
MIDI / MPE / future guitar metadata
  -> MIDI and MPE Router
  -> Performance Interpreter
       -> string/fret/articulation gestures
  -> Voice Manager
  -> String Voice
       -> excitation model
       -> waveguide/string model
       -> fret/finger/collision/damping models
  -> Pickup and Tone Model
  -> Output
```

The MVP can skip the `Performance Interpreter` at first. It becomes important once the string model can respond to richer gestures than note-on/note-off.

## Near-Term Experiment Ladder

1. Replace the sine tone with a simple Karplus-Strong string.
2. Add velocity-scaled excitation and note-off damping.
3. Add pluck position and brightness/damping parameters.
4. Add a simple pickup-position filter or comb-like model.
5. Add polyphonic voices with stable voice stealing.
6. Add MPE channel-to-voice routing and independent pitch bend.
7. Add pressure and CC74 mappings to damping, brightness, pluck position, or pickup intensity.
8. Add a basic gesture vocabulary: picked note, muted note, slide, bend, vibrato, hammer-on, pull-off.
9. Add a rule-based performance interpreter for string/fret/articulation choices.
10. Revisit neural/DDSP/differentiable approaches only after the physical controls and test cases are clear.

Early listening notes suggest that Strat-like twang/quack may require pickup-position and pickup-mix modeling, not only changes to the string excitation.

## Evaluation Strategy

Each realism feature should have both a technical test and a listening test.

Technical checks:

- no allocation or blocking on the audio thread
- stable output at 44.1 kHz and 48 kHz
- safe default gain
- smooth pitch and parameter changes
- repeatable behavior under fast MIDI input

Listening checks:

- Does the attack sound picked rather than synthesized?
- Does note-off feel like a hand/finger release rather than a gate?
- Does pitch bend feel like guitar string bending rather than generic oscillator bend?
- Does pickup position change harmonic character in a guitar-like way?
- Does MPE expression preserve independent note identity?
- Does the instrument respond naturally through an external amp sim?

## Open Questions

- Should string/fret metadata be controlled by MIDI channel, future UI settings, or custom MPE mappings?
- How much automatic interpretation should happen by default versus being opt-in?
- What is the minimum useful string model before pickup modeling becomes meaningful?
- Can hammer-ons and pull-offs be modeled without a full left-hand/fret simulation?
- How should authored MPE pitch curves map to physical bend/vibrato gestures without fighting the performer?
- Would a small learned model be useful for choosing articulations from MIDI context while keeping audio synthesis physical?
