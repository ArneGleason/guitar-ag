# 0073 — Pick Attack Research

## Context

Human DAW audition after EG-071 found the slide-squeak controls musically useful. Chord/strum examples sound convincing, especially when note starts are staggered across strings, but single-note picked attacks still read as synthetic, almost like an older digital/FM attack artifact.

This pass is research and diagnosis only. No DSP or UI behavior is changed here. The first implementation pass that followed this research is tracked in `plans/0074-pick-stroke-plectrum.md`.

## Local Diagnosis

Current `StringVoice::start` treats picking as several coupled but mostly note-start-only ingredients:

- a displacement shape and local pick-kink written into the delay lines;
- high modal attack energy and extra short chirp modes;
- a random `pickTransient` impulse;
- a separate contact layer with `pickContact`, `pickContactRing`, `pickGrind`, `pickCoin`, and `pickHeavy` branches;
- oscillator-like contact carriers driven by `fastContactSin` and nonlinear shaping in `renderContactLayer`.

The likely failure modes for single notes:

- The contact layer can be too tonal and oscillator-like, which exposes a digital "attack patch" quality.
- The modal chirp modes and contact ring can both emphasize a clear synthetic pitch/edge at note onset.
- `randomState` is currently seeded from note number and MIDI channel at note start, so repeated single notes on the same note/channel can reuse the same attack fingerprint.
- The model has no pick direction or alternating-stroke memory, so repeated picked notes do not get the small up/down and grip/angle variation a guitarist naturally produces.
- Strummed chords hide these weaknesses because different notes/strings use different seeds and onset times, making the aggregate gesture feel more physical.

## Research Notes

### Pick/String Interaction

Evangelista and Smith's DAFx pluck-action work frames plucking as a temporary interaction between player and string, not just an impulse. Their model uses a scattering junction, player compliance, and force coupling, and explicitly notes that plucking direction distributes energy into the two transverse string polarizations.

Germain and Evangelista's WASPAA plectrum work models the pick itself from beam mechanics and derives a release condition so the string enters free vibration with a physically plausible state. For Guitar AG, the key idea is not to implement a full beam solver immediately, but to model the pick as a contact-and-release process with compliance and release timing.

Recent robotic-plucker work by Pluta, Jasinski, Tokarczyk, and Grygiel shows that small changes in plectrum trajectory/depth change loudness, timbre, harmonic content, and decay. At too-low depth the string is weakly or oddly excited; beyond that region, deeper attack gets louder, fuller in low frequencies, rougher, and less noisy/inharmonic.

### Plucking Position And Pickup Interaction

Mohamad, Dixon, and Harte show that electric-guitar plucking point and pickup position leave measurable spectral signatures, robust enough to estimate from recorded electric guitar tones. This reinforces that picking sound should not be only a click/noise layer; where and how the string is released changes the sustained partial balance.

### Technique And Subjective Tone

Berklee's guitar-picking guidance treats pick strokes, finger plucks, hammer-ons, pull-offs, and slides as separate "attack devices." It explicitly notes that upstrokes and downstrokes can sound different, and players may either minimize or exploit that difference.

Fender's pick guide is a useful practical baseline: celluloid, Delrin, and nylon are common materials; medium, standard-shaped picks are a reasonable default starting point. That supports using one normal plastic plectrum model first rather than exploding the UI into many material types.

Classical-guitar timbre research by Orio found that plucking technique variations can be recognized through sound analysis. For future finger/nail modes, the important direction is that flesh/nail contact is a different excitation family, not just "softer pick."

## Design Implications

Start with one believable everyday plectrum rather than a giant pick-material menu.

The first implementation pass should likely:

- add a `Pick Stroke` or `Stroke Pattern` choice: `Down`, `Up`, `Alternate`;
- track the next stroke direction in `AudioEngine` or a small player-articulation layer;
- feed direction into `StringVoice::start`;
- preserve the current `Pick Stiffness` and `Pick Texture` controls, but reinterpret them around a normal plectrum:
  - `Pick Stiffness`: compliance/flex and release time;
  - `Pick Texture`: edge/friction/noise amount, not a synthetic oscillator carrier;
- include the render timeline or a per-voice counter in the attack seed so repeated same-note attacks vary subtly;
- reduce or replace the exposed tonal contact oscillator/ring for normal plastic-pick settings;
- move the perceived attack realism toward a short contact/release envelope that shapes modal energy and polarization, with contact noise as a supporting residue.

## Candidate Control Taxonomy

Keep this small:

- Existing `Pick Stiffness`: soft/flexible to hard/rigid plectrum behavior.
- Existing `Pick Texture`: smooth edge to rougher edge.
- New `Pick Stroke`: `Down`, `Up`, `Alternate`.

Avoid adding finger/nail/plectrum type controls until the ordinary plectrum sounds good on single notes.

## Proposed Phase 1

Implement `StringVoice EG-072 PickStrokePlectrum`:

- Add pick-direction state and an automatable or host-facing `Pick Stroke` choice.
- Make `Alternate` the default for single-note lines.
- Vary note-start random seed with timeline/sample or a monotonic attack counter while keeping renders deterministic.
- Rebalance normal pick contact:
  - lower the steady tonal ring in `pickContactRing`;
  - reduce attack chirp-mode intensity at default settings;
  - shorten and darken the standalone `pickTransient`;
  - emphasize displacement/release shape and modal attack distribution over additive contact tone.
- Keep coin/heavy-pick extremes intact or gated to high `Pick Texture`, but protect the default plastic-pick region from sounding like those regimes.

## Verification Targets

- Repeated single notes no longer have identical attack fingerprints.
- A default medium-plastic pick setting sounds less like a digital/FM attack on isolated notes.
- Strummed chords retain the current convincing staggered engagement.
- `Pick Stroke = Down` and `Pick Stroke = Up` produce subtle but audible differences.
- `Pick Stroke = Alternate` alternates on repeated notes and feels more like a picked line.
- Low `Pick Texture` is clean but not lifeless; high `Pick Texture` adds edge/friction without becoming the old synthetic chirp.
- Existing pick controls remain useful and do not create UI overload.

## Sources

- Evangelista, G. and Smith, J. O. III. "Structurally passive scattering element for modeling guitar pluck action." DAFx-10. https://www.researchgate.net/publication/267301263_Structurally_passive_scattering_element_for_modeling_guitar_pluck_action
- Germain, F. G. and Evangelista, G. "Synthesis of guitar by digital waveguides: Modeling the plectrum in the physical interaction of the player with the instrument." WASPAA 2009. https://www.researchgate.net/publication/224086779_Synthesis_of_guitar_by_digital_waveguides_Modeling_the_plectrum_in_the_physical_interaction_of_the_player_with_the_instrument
- Pluta, M., Jasinski, J., Tokarczyk, D., and Grygiel, J. "The effect of micro-changes in the pluck trajectory on the sound of an acoustic guitar." Vibrations in Physical Systems, 2025. https://vibsys.put.poznan.pl/article/the-effect-of-micro-changes-in-the-pluck-trajectory-on-the-sound-of-an-acoustic-guitar/
- Mohamad, Z., Dixon, S., and Harte, C. "Pickup position and plucking point estimation on an electric guitar via autocorrelation." JASA, 2017. https://www.researchgate.net/publication/321787169_Pickup_position_and_plucking_point_estimation_on_an_electric_guitar_via_autocorrelation
- Orio, N. "The Timbre Space of the Classical Guitar and Its Relationship with the Plucking Techniques." ICMC, 1999. https://quod.lib.umich.edu/i/icmc/bbp2372.1999.427/1/--timbre-space-of-the-classical-guitar-and-its-relationship
- Berklee Online. "Signature Guitar Tone and Picking." https://online.berklee.edu/takenote/signature-tone-and-picking/
- Fender. "How to Choose a Guitar Pick." https://www.fender.com/articles/techniques/take-your-pick
