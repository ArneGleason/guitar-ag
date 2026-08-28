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

Listening feedback:

- The left-hand mute behavior is good enough for now.
- The string tone still reads more nylon-like than steel-like.
- The missing steel/electric quality seems to involve overtone generation or preservation, not only EQ brightness.

## 2026-04-25 — Render vs Guitar-TECHS DI Comparison

Compared the user-rendered `StringVoice KS-003` calibration WAV against Guitar-TECHS P1 direct-input single-note references.

Render:

```text
/Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-ks003-e35bf11-single-note-calibration.wav
```

Reference:

```text
/Users/arnegleason/code/reference-audio/Guitar-TECHS/extracted/P1_singlenotes/audio/directinput/directinput_allsinglenotes.wav
```

Initial observations:

- The render is a 48 kHz stereo WAV, 24 seconds long, peak 0.087955, RMS 0.017134.
- The comparison script matched the six long calibration notes against same-pitch DI note regions.
- Simple zero-crossing rates are in the same rough range for some notes, so the issue is not merely pitch or gross brightness.
- Rough harmonic-balance metrics show the render has far less upper-harmonic energy than the DI reference.
- For example, the render upper-to-low harmonic ratios were around 0.000001 to 0.007, while real DI examples were often much higher.

Hypothesis:

The next sound step should not be a plain high-shelf EQ. Guitar AG likely needs a simple electric pickup/tone model and/or steel-string partial behavior that creates or preserves upper harmonic structure. A pickup-position readout/comb model is the best next small experiment because it can introduce electric-guitar-like harmonic emphasis/notches and may move the sound toward twang/quack.

Risks:

- Current reference matching is still approximate; some same-pitch notes in Guitar-TECHS are much quieter than others.
- The harmonic metrics are dependency-free and coarse. They are useful for direction, not final scoring.

## 2026-04-25 — Simple Pickup/Tone Experiment

Added a fixed post-string `ElectricGuitarTone` stage.

Current behavior:

- Mixed string voices feed a single tone stage before output.
- The tone stage uses a short delay/comb readout as a crude bridge-ish pickup-position approximation.
- It adds a string-velocity-style difference component.
- It adds small high-pass/presence shaping and a little body/tone smoothing.
- The visible model label is now `StringVoice KS-004 Pickup-001`.

Initial values:

- nominal pickup position: 0.18 of low-E string length for the fixed delay estimate
- delayed sample subtraction: 0.72
- presence coefficient: 0.42
- high-pass coefficient: 0.14
- body coefficient: 0.18
- final tone gain: 0.72

Expected sound:

This should move the tone away from smooth nylon-like pluck and toward electric DI brightness/twang. It may be too thin or phasey; the next calibration render should decide whether it is warmer or colder.

Calibration feedback:

- `StringVoice KS-004 Pickup-001` increased upper-harmonic measurements compared with KS-003.
- It also reduced overall level/body and still remained far below Guitar-TECHS DI upper-harmonic ratios.
- The fixed post-mix comb is probably too crude because all notes share one pickup delay instead of using a note/string-length-specific readout.

## 2026-04-25 — Per-Voice Pickup Readout Experiment

Moved the first pickup-position readout into each `StringVoice`.

Current behavior:

- Each voice computes a pickup read offset from its own delay length.
- The offset is currently about 0.18 of the modeled string length.
- The voice output blends a displacement-like pickup sample with a velocity-like difference component.
- `ElectricGuitarTone` no longer performs a fixed pickup comb. It now provides lighter post-mix high-pass, presence, body, and gain conditioning.
- The visible model label is now `StringVoice KS-005 Pickup-Voice`.

Initial values:

- per-voice pickup offset: 0.18 of delay length
- pickup sample coefficient: 0.82
- pickup velocity coefficient: 0.55
- voice output gain: 0.42
- post-mix tone gain: 0.95

Expected sound:

This should keep more note-dependent electric/pickup character than KS004, with less global thinning from the fixed post-mix comb. It may still lack steel-string bite because the string/exciter itself remains a simple Karplus-Strong model.

Calibration feedback:

- The KS005 render restored level/body compared with KS004. Whole-render RMS is about 0.0169, close to KS003 at about 0.0171, while KS004 was about 0.0051.
- KS005 peak is about 0.0986, higher than KS003 at about 0.0880 and KS004 at about 0.0654.
- The upper-to-low harmonic metric mostly returned near KS003 rather than preserving the KS004 increase. For example, E2 was 0.0036 in KS005 vs 0.0041 in KS003 and 0.0182 in KS004.
- This suggests the per-voice readout is a better structure, but the current readout/blend is not enough to add steel-string/electric bite.
- The next sound experiment should likely change the string/exciter partial content or add a controlled nonlinear/contact component rather than only moving pickup filtering around.

## 2026-04-25 — Steel Exciter Harmonics Experiment

Added a small steel/pick-contact experiment inside `StringVoice`.

Current behavior:

- The initial delay line now includes a small deterministic higher-partial component at the 5th, 7th, 11th, and 13th harmonics.
- The note start includes a short high-passed noise burst intended to read more like pick contact/scrape than generic broadband noise.
- The feedback path includes a very small soft-clipped slope term to preserve or create more upper partials.
- The per-voice pickup readout from KS005 remains in place.
- The visible model label is now `StringVoice KS-006 SteelExcite`.

Initial values:

- steel partial amount: 0.010 to 0.050, scaled by velocity
- pick contact amount: 0.006 to 0.036, scaled by velocity
- pick contact duration: about 14 to 26 ms
- feedback blend: 0.58 current sample, 0.42 previous sample
- slope drive: `tanh(slope * 2.8) * 0.018`

Expected sound:

This should add more bite, scrape, and upper partials than KS005 while keeping the cleaner per-voice pickup structure. The risk is that the contact burst or slope term may become fizzy or too synthetic, especially at high velocity.

Calibration feedback:

- KS006 increased the mean upper-to-low harmonic metric from KS005's about 0.0028 to about 0.0044, so the steel exciter moved in the intended direction.
- KS006 is still far below the Guitar-TECHS DI references on upper-harmonic energy. The reference notes often show much stronger upper-harmonic energy immediately after the attack.
- Time-varying analysis suggests the real DI notes are less static than the current KS voice. Reference harmonic phase/pitch drift across harmonics 2-8 averaged about 12.4 cents standard deviation, while KS006 averaged about 1.0 cent.
- The reference also shows stronger moving upper-harmonic/resonance behavior: early upper-to-low energy is much higher and often collapses or shifts over the note, while KS006 decays more smoothly and predictably.
- This points toward adding time-varying string behavior such as tension/pitch settling, dispersion/inharmonicity, moving contact damping, or a short attack-only contact/resonance layer rather than only adding static partials.

## 2026-04-25 — Phase Settling Experiment

Added one isolated time-varying string property inside `StringVoice`.

Current behavior:

- The KS006 exciter, pickup readout, and tone stage are unchanged.
- The delay-line feedback path now passes through a first-order all-pass stage.
- The all-pass coefficient starts lower and settles upward during the first few hundred milliseconds of each note.
- This should create harmonic-dependent phase movement without directly adding brightness.
- The visible model label is now `StringVoice KS-007 PhaseSettle`.

Initial values:

- all-pass coefficient start: about 0.030 to 0.045, depending on velocity
- all-pass coefficient target: about 0.120 to 0.150, depending on velocity
- settling time: about 260 to 380 ms

Expected sound:

This should subtly destabilize the otherwise static KS loop, especially near the attack. It may read as more string-like tension settling, or it may simply detune/blur the note if the coefficient movement is too strong.

Calibration feedback:

- KS007 successfully increased harmonic phase/pitch drift. Mean phase-drift standard deviation across harmonics 2-8 rose from about 1.0 cents in KS006 to about 3.5 cents in KS007; the Guitar-TECHS DI reference averaged about 12.4 cents.
- Mean absolute phase drift moved close to the reference average: KS006 was about 2.5 cents, KS007 about 8.7 cents, and the reference about 9.4 cents.
- The isolated phase-settling change did not improve brightness. Mean upper-to-low harmonic energy fell from about 0.0044 in KS006 to about 0.0017 in KS007.
- Moving upper-harmonic emphasis also became more static by the current metric. The strongest-harmonic shift standard deviation fell from about 0.57 in KS006 to about 0.06 in KS007, while the reference averaged about 1.04.
- Conclusion: the all-pass phase-settling direction is useful, but the current implementation should not be treated as a tone improvement by itself. It likely needs to be paired later with a separate amplitude/resonance or harmonic-dependent damping model.

## 2026-04-25 — Moving Resonance Experiment

Added one isolated time-varying amplitude/resonance property inside `StringVoice`.

Current behavior:

- The KS007 all-pass phase-settling stage is not included in this build.
- The KS006 steel exciter, pickup readout, and tone stage are otherwise preserved.
- Three short-lived resonators are configured at the 5th, 7th, and 11th harmonics of the active note.
- Resonance weighting starts higher on the 11th harmonic and shifts toward the 5th harmonic over the first few hundred milliseconds.
- The resonance layer is lightly fed back into the delay line and also added to the pickup readout.
- The visible model label is now `StringVoice KS-008 MovingRes`.

Initial values:

- resonance harmonics: 5, 7, 11
- resonance radii: 0.9895, 0.9880, 0.9860
- resonance envelope: 0.012 to 0.057, scaled by velocity
- resonance movement time: about 360 to 520 ms

Expected sound:

This should test whether a changing upper-harmonic emphasis moves the sound closer to the DI reference. It may sound more alive, or it may become metallic/ringy if the resonators are too narrow or too prominent.

Calibration feedback:

- KS008 preserved the KS006 level and broad upper-harmonic balance better than KS007. Mean upper-to-low harmonic energy was about 0.0043, close to KS006 at about 0.0044 and above KS007 at about 0.0017.
- The high "air" band improved. Early air-to-low rose from about 0.00057 in KS006 to about 0.00104 in KS008; the DI reference average is about 0.00825.
- The h4-h12 relative spread moved very close to the reference average. KS008 averaged about 34.49 dB, while the reference averaged about 34.50 dB. KS006 was about 37.97 dB and KS007 about 47.48 dB.
- The strongest-harmonic shift metric did not improve beyond KS006. KS008 averaged about 0.47 vs KS006 at about 0.57 and the reference at about 1.04.
- As expected for this isolated test, phase drift stayed close to KS006 and far below KS007/reference. KS008 phase-drift standard deviation averaged about 1.0 cents.
- Conclusion: moving resonance is useful for spectral shape/air and should be kept as a candidate. It does not replace the phase-settling idea, and the resonance movement probably needs either stronger frequency movement or later recombination with phase settling.

## 2026-04-25 — Harmonic Damping Experiment

Added one isolated harmonic-dependent damping property inside `StringVoice`.

Current behavior:

- The KS008 moving resonance layer remains in place.
- The KS007 all-pass phase-settling stage is still not included.
- The delay-line feedback sample is split into low and high components with a one-pole filter.
- The high component starts with nearly neutral feedback gain and gradually damps more strongly over the first few hundred milliseconds.
- The visible model label is now `StringVoice KS-009 HarmDamp`.

Initial values:

- high feedback gain start: 0.9995
- high feedback gain target: about 0.980 to 0.984, depending on velocity
- transition time: about 420 to 620 ms
- split lowpass coefficient: 0.18

Expected sound:

This should keep the attack relatively bright while making the sustain shed high-frequency energy faster. It may make the tone more guitar-like if the decay contrast is useful, or it may simply dull the note if the target high feedback gain is too low.

Calibration feedback:

- KS009 did create stronger time-varying damping. Mean attack-to-sustain rose from about 1.34 in KS008 to about 1.36, and late-to-sustain fell from about 0.64 to about 0.61.
- Strongest upper-harmonic shift moved toward the DI reference: KS008 averaged about 0.47, KS009 about 0.75, and the reference about 1.04.
- High-harmonic decay became too strong by the current h8 metric. KS008 averaged about -14.8 dB, close to the reference average of about -14.6 dB, while KS009 fell to about -25.7 dB.
- Upper-to-low harmonic energy fell modestly from about 0.0043 in KS008 to about 0.0040 in KS009, and late upper-to-low fell from about 0.0033 to about 0.0020.
- The high-air improvement from KS008 mostly survived at the attack, but mid-air energy dropped from about 0.00057 to about 0.00043.
- Conclusion: harmonic-dependent damping is a useful property, especially for moving harmonic emphasis, but the current high-feedback target is too aggressive. A later version should use a gentler high-frequency loss or a more selective band-specific decay.

## 2026-04-25 — Gentle Harmonic Damping Retune

Retuned the KS009 harmonic damping experiment.

Current behavior:

- The KS008 moving resonance layer remains in place.
- The KS009 harmonic damping architecture remains in place.
- The high feedback target is much closer to neutral.
- The damping transition is slower.
- The split filter is more selective, so less midrange is treated as the high component.
- The moving resonance contribution is added after the damping tilt instead of being damped by it.
- The visible model label is now `StringVoice KS-010 GentleDamp`.

Initial values:

- high feedback gain start: 0.9995
- high feedback gain target: about 0.992 to 0.9935, depending on velocity
- transition time: about 550 to 800 ms
- split lowpass coefficient: 0.30

Expected sound:

This should preserve more of KS008's upper harmonic and air behavior while keeping some of KS009's improved dynamic decay/moving-emphasis behavior. It is a retune, not a new modeling feature.

Calibration feedback:

- KS010 restored most of the KS008 energy that KS009 lost. Whole-file RMS was about 0.01676, close to KS008 at about 0.01678.
- Mean upper-to-low harmonic energy returned to about 0.00424, close to KS008 at about 0.00429 and above KS009 at about 0.00403.
- Mean h8 decay was about -16.7 dB, between KS008 at about -14.8 dB and KS009 at about -25.7 dB. This is a better compromise than KS009, though still note-dependent.
- Strongest upper-harmonic shift retained a small improvement over KS008: KS010 averaged about 0.52, KS008 about 0.47, KS009 about 0.75, and the reference about 1.04.
- Grouping the calibration notes roughly as lower/wound-side notes E2-A2-D3 vs higher/plain-side notes G3-B3-E4 supports the listening feedback: the lower notes are much farther from the reference in early/mid air and phase drift.
- The DI reference low-note group has far more early/mid upper and air energy than the render. For example, reference low-note early air-to-low averaged about 0.0164, while KS010 averaged about 0.00115.
- The high/plain-side render is still imperfect, but it is closer in broad spectral shape than the lower/wound-side notes.
- Conclusion: KS010 is the better damping retune. The next experiment should probably add string-class-aware behavior, especially a wound-string interaction layer for lower notes, rather than another global damping adjustment.

## 2026-04-25 — Wound Open-String Experiment

Added open-string class behavior inside `StringVoice`.

Current behavior:

- The calibration notes are treated as open guitar strings.
- E2, A2, and D3 are treated as wound strings.
- G3, B3, and E4 are treated as plain strings.
- The KS010 path remains the base path for all strings.
- Wound strings receive a small excitation boost and an extra decaying interaction layer.
- The wound interaction layer combines high-passed noise, slow roughness, string motion, and pick contact.
- The wound interaction is injected lightly into the string loop and more directly into the pickup readout.
- The visible model label is now `StringVoice KS-011 WoundOpen`.

Initial values:

- wound excitation scale: 1.28
- wound interaction envelope: 0.006 to 0.028, scaled by velocity
- wound interaction duration: about 420 to 600 ms
- wound loop injection: 0.22
- wound pickup readout mix: 0.88

Expected sound:

This should add more scrape/air and physical roughness to the lower open strings while leaving the higher plain strings close to KS010. The main risk is adding static fizz or noisy attack that does not track string motion naturally.

Calibration feedback:

- KS011 changed the intended note group only. E2, A2, and D3 gained upper/air energy, while G3, B3, and E4 stayed effectively unchanged from KS010.
- Whole-render level stayed stable. KS011 RMS was about 0.01676, close to KS010 at about 0.01676.
- The lower/wound group upper-to-low metric rose from about 0.00607 in KS010 to about 0.00708 in KS011.
- The lower/wound early air-to-low metric rose from about 0.00115 in KS010 to about 0.00150 in KS011, but the Guitar-TECHS DI lower/wound group is still much higher at about 0.0164.
- The lower/wound strongest upper-harmonic shift improved from about 0.46 in KS010 to about 0.64 in KS011, closer to the DI reference around 1.06.
- Lower/wound phase drift did not materially improve. KS011 stayed near 0.06 cents by the current metric, while the DI reference lower/wound group is much more irregular.
- Conclusion: the wound-string interaction layer is a useful structure, but this first version mostly adds controlled amplitude/noise energy. The next lower-string experiment should target wound-only phase/dispersion or motion-coupled roughness rather than a global brightness change.

## 2026-04-25 — Wound Phase Experiment

Added a wound-only phase/dispersion stage inside `StringVoice`.

Current behavior:

- KS011 remains the base path.
- E2, A2, and D3 now pass their feedback signal through a gentle first-order all-pass stage.
- The all-pass coefficient moves over the first few hundred milliseconds of each wound-string note.
- A small decaying modulation, driven by slow noise and string motion, perturbs the coefficient.
- G3, B3, and E4 are intended to stay on the KS011/KS010 plain-string path.
- The visible model label is now `StringVoice KS-012 WoundPhase`.

Initial values:

- wound all-pass coefficient start: about 0.032 to 0.044, depending on velocity
- wound all-pass coefficient target: about 0.088 to 0.106, depending on velocity
- coefficient movement time: about 340 to 560 ms
- decaying coefficient modulation: about 0.018 to 0.042, scaled by velocity
- coefficient clamp: 0.015 to 0.135

Expected sound:

This should test whether the lower wound strings can gain more harmonic phase irregularity and evolving string behavior without a global tone change. The main risk is the same family as KS007: too much all-pass movement can blur or dull upper harmonics, so this version is wound-only and gentler.

Calibration feedback:

- KS012 kept the intended isolation: G3, B3, and E4 stayed essentially unchanged from KS011.
- The wound-string phase metrics moved, but not in the most useful shape. Lower/wound phase-drift mean absolute error rose from about 0.96 cents in KS011 to about 5.66 cents in KS012, closer to the DI reference around 12.16 cents.
- Lower/wound phase-drift standard deviation only rose from about 0.06 cents to about 0.21 cents, still far below the DI reference around 16.06 cents.
- The cost was significant loss of wound-string air and upper content. Lower/wound upper-to-low fell from about 0.00708 in KS011 to about 0.00372 in KS012, and early air-to-low fell from about 0.00150 to about 0.00084.
- The lower/wound strongest upper-harmonic shift metric collapsed from about 0.64 in KS011 to 0.00 in KS012, while the DI reference is around 1.06.
- Conclusion: this all-pass placement is useful as a diagnostic but probably not a good tone direction as implemented. It creates broad phase offset/blur more than lively wound-string irregularity. The next pass should likely return to the KS011/KS010 base and target motion-coupled roughness or moving partial behavior without putting a strong all-pass directly in the core feedback path.

## 2026-04-25 — Wound Motion Experiment

Added a wound-only moving partial/roughness layer inside `StringVoice`.

Current behavior:

- KS011 is treated as the practical base.
- The KS012 wound all-pass feedback stage is removed.
- E2, A2, and D3 receive a separate wound motion layer with four broad resonances.
- The wound motion layer is driven by string slope, pick contact, wound interaction output, high-passed noise, and slow roughness.
- The resonance weights move from higher partials toward lower/mid partials over the first few hundred milliseconds.
- The wound motion output feeds the pickup readout strongly, the moving-resonance drive lightly, and the string loop very lightly.
- G3, B3, and E4 are intended to stay close to KS011/KS010.
- The visible model label is now `StringVoice KS-013 WoundMotion`.

Initial values:

- wound motion resonances: about 5.65x, 7.90x, 10.85x, and 13.75x note frequency
- resonance radii: 0.9875, 0.9868, 0.9858, and 0.9845
- motion envelope: about 0.010 to 0.044, scaled by velocity
- motion decay: about 0.99978 to 0.99982
- motion movement time: about 300 to 500 ms
- wound motion loop injection: 0.055
- wound motion pickup readout mix: 0.96

Expected sound:

This should restore the KS011 lower-string air/upper energy while adding more evolving wound-string character than a static roughness layer. The main risk is metallic ringing or fizzy non-string noise if the moving partial layer is too strong.

Calibration feedback:

- KS013 correctly isolated the change to E2, A2, and D3; the plain-string notes stayed essentially unchanged.
- It restored the air/upper energy that KS012 lost. Lower/wound upper-to-low was about 0.00673, near KS011 at about 0.00708, and early air-to-low rose to about 0.00276 from KS011's about 0.00150.
- The lower/wound strongest upper-harmonic shift metric improved to about 0.93, close to the DI reference around 1.06.
- Subjectively, the lower strings sounded too synthetic because the added layer read as a short white-noise hiss/envelope after the attack, around a few hundred milliseconds long.
- The likely cause is the high-passed noise component in the wound-motion drive plus a relatively fast motion-envelope decay. The metric moved in a useful direction, but the perceptual material is wrong: real guitar wound-string air should read more like metallic/string-coupled ring than white-noise hiss.
- Conclusion: keep the moving-partial architecture as promising, but strongly reduce stochastic noise drive and retune toward lower-level, longer metallic resonances.

## 2026-04-25 — Wound Ring Retune

Retuned the KS013 wound-motion experiment.

Current behavior:

- KS014 keeps the separate wound-only moving-partial layer.
- The high-passed noise contribution inside the wound-motion drive is reduced sharply.
- The slow random texture contribution is reduced.
- The motion drive is biased more toward string slope and wound interaction output.
- The wound-motion resonators are longer-lived and slightly lower in output level.
- Coupling into the main loop and pickup readout is reduced.
- The visible model label is now `StringVoice KS-014 WoundRing`.

Initial values:

- wound motion resonances: about 5.90x, 7.95x, 10.85x, and 13.65x note frequency
- resonance radii: 0.9935, 0.9928, 0.9918, and 0.9905
- motion envelope: about 0.006 to 0.026, scaled by velocity
- motion decay: about 0.999925 to 0.99994
- high-passed noise drive: 0.055, down from 0.35
- slow texture drive: 0.10, down from 0.28
- wound motion loop injection: 0.018, down from 0.055
- wound motion pickup readout mix: 0.52, down from 0.96

Expected sound:

This should trade some of KS013's spectral-air gain for a more metallic, string-coupled lower-string tail with less obvious white-noise hiss. The main risk is undershooting and returning too close to KS011.

Calibration feedback:

- KS014 reduced the KS013 air boost but did not create a qualitatively more realistic open-string sound.
- The lower/wound early air-to-low metric fell from about 0.00276 in KS013 to about 0.00206 in KS014, still above KS011 at about 0.00150.
- The lower/wound strongest upper-harmonic shift fell from about 0.93 in KS013 to about 0.77 in KS014, still above KS011 at about 0.64.
- Subjectively, the recent wound-layer releases do not feel meaningfully closer to a realistic open electric guitar string.
- Conclusion: the additive wound-noise/ring branch is probably the wrong level of model. The next experiment should change the core string/pickup representation rather than continuing to decorate the existing one-polarization Karplus-Strong loop.

## 2026-04-25 — Research-Informed Course Correction

The next open-string experiment should be bolder than another wound-layer retune.

Useful research signals:

- Parametric electric guitar synthesis work models two string polarizations, loop filters, inharmonicity, pickup position/width, and pickup nonlinearity as connected parts of the instrument rather than isolated effects.
- Pickup modeling research emphasizes that pickup position, finite aperture/sensitivity width, resonant electrical filtering, pickup mixing, and magnetic nonlinearity all shape electric guitar DI tone.
- Player/string interaction work points at scattering/contact models for pluck, touch, frets, and collisions rather than treating noise as a main realism source.
- Piano and other stiff-string models reinforce that inharmonicity, dual-polarization/coupled modes, and body/bridge coupling can make the sound feel physical before any explicit "noise" layer is added.

Working hypothesis:

The current model sounds synthetic because it has one main string motion, a crude pickup readout, and additive layers trying to impersonate missing physics. A better next experiment is a restrained architecture reset:

- two polarization delay paths per voice
- different damping/dispersion per polarization
- weak cross-coupling between polarizations
- pick excitation that launches both polarizations with a slight time/phase difference
- pickup readout based on string velocity at a position, with a small finite-width aperture
- optional pickup/circuit resonance after the physical readout
- no dedicated white-noise wound tail

Expected sound:

This may initially be less bright than KS013, but it should be a better physical base: more naturally metallic, more pickup-like, and less dependent on hiss/noise tricks.

## 2026-04-25 — Dual Polarization Pickup Experiment

Implemented the first version of the course-correction experiment inside `StringVoice`.

Current behavior:

- Each voice now has two delay paths representing two simplified string polarizations.
- The two polarizations are excited differently at note start.
- The secondary polarization has its own damping state and high-frequency feedback target.
- The two paths are weakly cross-coupled in the feedback loop.
- The old wound noise/ring processing branch is removed from the audible path.
- Pickup readout now uses velocity from a small finite-width aperture rather than one raw displacement sample plus additive wound layers.
- The pickup reads the two polarizations from slightly different positions.
- E2, A2, and D3 still get slightly different coupling/damping through the open-wound-string classification, but they no longer receive a dedicated hiss/ring layer.
- The visible model label is now `StringVoice KS-015 DualPickup`.

Initial values:

- primary pickup position: 0.18 of string length
- secondary pickup position: 0.205 of string length
- pickup aperture: 0.018 of string length
- primary displacement excitation: 0.70 * velocity
- secondary displacement excitation: about 0.28 to 0.34 * velocity
- primary loop blend: 0.58 current, 0.42 previous
- secondary loop blend: 0.53 current, 0.47 previous
- cross-coupling: about 0.010 for plain strings, 0.015 for wound strings
- contact noise is now short and low-level, intended as incidental pick contact only

Expected sound:

This is a deliberate jump away from the KS011-KS014 local search. It may score worse on some previous metrics, but it should test whether two-polarization motion plus physical pickup velocity/aperture readout produces a less synthetic open-string base.

## 2026-04-25 — Hybrid Modal / Pickup Resonance Experiment

Tested a more radical hypothesis after KS015 still sounded too synthetic: convincing electric-guitar DI may depend less on a single Karplus-Strong loop sounding "stringy" and more on the combined behavior of transverse string motion, sparse inharmonic side modes, pickup aperture/position, and pickup/cable resonance.

Current behavior:

- `StringVoice` keeps the KS015 two-polarization delay-loop core as the main string motion.
- Each voice also initializes a quiet modal residual bank.
- The modal residual uses harmonic modes plus sparse, slightly inharmonic side modes, with extra side modes on E2/A2/D3 treated as wound open strings.
- The modal residual is mixed into pickup readout at low level instead of replacing the waveguide loop.
- `ElectricGuitarTone` now includes a simple fixed pickup/cable-style resonator around 3.1 kHz, plus stronger presence shaping.
- The visible model label is now `StringVoice KS-016 HybridModal Pickup-002`.

Offline experiment notes:

- Pure modal replacement was rejected for now. It became too clean, too harmonically organized, and farther from the DI references by log-spectral distance.
- Modal residual on top of the KS015 loop was also modest by itself; it did not explain the missing electric character.
- Adding a simple pickup/cable resonant output stage improved the KS015 perceptual distance from about 30.16 dB to about 29.34 dB in the current analyzer.
- This is not yet a full pickup model and should be judged by listening, especially for twang/quack and lower-string metal character.

Expected sound:

This candidate should sound more electrically voiced than KS015, with more upper-mid pickup emphasis. It may still lack realistic open-string complexity; if so, the next bigger hypothesis should focus on coupling the pick/string contact into the loop over time rather than adding independent residual layers.

## 2026-04-25 — Modal Cluster Becomes the Active Branch

Listening feedback overturned the initial analyzer conclusion. The offline render:

```text
/Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-offline-ks016-modalcluster-dirty-single-note-calibration.wav
```

was judged much more like a real electric guitar string than the later hybrid/pickup installed build, even though the current perceptual analyzer scored it worse.

Current behavior:

- The active branch is now `StringVoice KS-017 ModalCluster`.
- The rendered audio is byte-for-byte identical to the previous `KS016 ModalCluster` dirty offline render when using the same calibration command.
- The voice uses explicit modal oscillators as the primary sound source rather than the previous delay-loop waveguide output.
- Wound open strings receive sparse inharmonic side clusters.
- `ElectricGuitarTone` has been returned to the simpler post-mix conditioning stage used during the original modal-cluster render.

Why the analyzer may be wrong here:

- The log-spectral distance rewards matching a particular reference take, not necessarily sounding like the same class of physical object.
- The modal cluster has clearer discrete ringing partials and metallic string identity, but those same high-Q partials can look like "wrong bins" in a strict spectral-distance metric.
- The current reference matching still includes large per-note/reference variation; some selected DI notes have unusual harmonic ratios, especially for A2/D3.
- The current metrics do not directly score "metal string objectness" or listener-recognizable pick/string behavior.

Follow-up:

Use `KS-017 ModalCluster` as the next listening baseline. The next analysis pass should try to explain why it sounds better by adding or emphasizing metrics for modal partial stability, inharmonic side-mode spacing, attack partial onset, and decay shape rather than optimizing only full-spectrum distance.

## 2026-04-25 — Pick Regime Velocity Experiment

Listening feedback on KS017: the ringing string core is much closer to an electric guitar string, but MIDI velocity only changes loudness. Real guitar picking changes the excitation regime: a gentle pluck and an aggressive strike have different contact width, release speed, partial balance, and short-lived attack content.

Current behavior:

- The visible model label is now `StringVoice KS-018 PickRegimes`.
- The KS017 modal-cluster structure remains the main sound source.
- Velocity now changes pluck position, moving harder strikes closer to the bridge.
- Velocity changes a synthetic contact width: gentle notes filter upper modes more, hard notes allow narrower, sharper excitation.
- Velocity changes harmonic tilt so hard strikes excite upper partials and air-band modes more strongly.
- Velocity increases inharmonic side-mode and wound-side cluster strength.
- High velocities add short-lived upper attack modes intended to resemble a sharper pick-release event.

Initial values:

- velocity ladder test: velocities 25, 45, 65, 85, 110, 127
- pluck position: about 0.24 at gentle velocity to 0.13 at hard velocity
- contact width: about 0.105 at gentle velocity to 0.020 at hard velocity
- modal tilt exponent: about 1.05/1.12 at gentle velocity to 0.56/0.68 at hard velocity for wound/plain strings

Expected sound:

Low velocities should keep the soft, old-string KS017 character. High velocities should sound brighter, more metallic, less polite, and more like the string is being shocked by the pick. This pass may be too extreme; the important listening question is whether velocity now changes playing character at all.

## 2026-04-25 — Hard Pick Regime Retune

Listening feedback on KS018: the dynamics became much more convincing, but the whole velocity range still felt weighted toward soft picking. Even the loudest notes sounded like medium-soft/casual pick strokes, and the lower half of the velocity range collapsed into extremely light touch.

Current behavior:

- The visible model label is now `StringVoice KS-019 HardPickRegimes`.
- The velocity-to-strike curve is shifted upward so mid velocities reach the prior light-pick region sooner.
- High velocities get an extra `hardStrike` region that boosts upper modal attack and inharmonic side clusters.
- Hard strikes move closer to the bridge than KS018.
- Contact width narrows more at high velocity, allowing more high-order modes.
- Overall modal gain is raised to address the quietness of KS018.

Offline analysis:

- KS018 velocity 25 to 127 raised upper-to-low harmonic balance by roughly 5x to 7x.
- KS019 raises upper-to-low harmonic balance by roughly 11x to 13x.
- KS019 attack/sustain now increases from low to high velocity, whereas KS018's attack/sustain ratio fell.
- The air-band increase is very large on wound strings, so KS019 may overshoot into clang/fizz. That is acceptable for this audition because the question is whether truly aggressive picking is now available.

Expected sound:

KS019 should make the top velocities much more forceful than KS018. If it is too bright or clangy, the next pass should keep the shifted velocity curve but reduce the air-band side modes rather than returning to the compressed soft-pick behavior.

## 2026-04-25 — First Fretboard Mapper

The sound model from KS019 is preserved, but wound/plain selection now comes from a simple guitar fretboard assignment rather than exact open-string MIDI notes.

Current behavior:

- The visible model label is now `StringVoice KS-020 FretboardMap`.
- Standard tuning is assumed: E2, A2, D3, G3, B3, E4.
- Strings 0-2 are treated as wound; strings 3-5 are treated as plain.
- `AudioEngine` asks `FretboardMapper` for a string/fret assignment at note-on.
- `StringVoice` receives the assignment and uses the assigned string class for modal stiffness, decay, side clusters, and wound-string modes.
- Active strings are marked occupied so chord notes prefer different strings where possible.
- The mapper starts around first/second position and has a simple position memory. It follows high notes upward faster than it returns downward, so a phrase that jumps up the neck does not immediately snap back to open position.

Initial heuristic:

- Valid candidates are all string/fret locations between fret 0 and 24 for the MIDI pitch.
- Candidate score favors the current hand position, open/first-position notes, and slightly lower-pitched strings as tie breakers.
- Occupied strings receive a large penalty.
- Out-of-range notes remain playable by clamping the assignment to low E open or high E fret 24, while pitch still follows the incoming MIDI note.

Expected sound:

Non-open notes that would naturally be played on the low E, A, or D strings can now inherit wound-string behavior. This is not yet a full guitarist model, but it should address the immediate issue where only MIDI notes 40, 45, and 50 sounded like wound strings.

## 2026-04-25 — Sustain and Velocity Ceiling Retune

Listening feedback on KS020: the fretboard mapper feels good, but held notes decay too quickly for a clean DI electric guitar, and the top of the velocity range should feel closer to the previous 67-68% strike intensity.

Current behavior:

- The visible model label is now `StringVoice KS-021 SustainVelCeil`.
- Full incoming MIDI velocity still drives note level.
- Strike character is capped at an internal velocity of about 0.68, so the most aggressive modal/side-cluster behavior no longer keeps increasing through the final third of the MIDI velocity range.
- Main modal decay times are lengthened for both wound and plain strings.
- High harmonics now decay less aggressively, especially during the held portion of the note.
- Inharmonic side clusters and wound-string modes sustain longer.

Initial values:

- strike-character velocity ceiling: 0.68
- main modal decay base: wound 8.2 seconds, plain 6.4 seconds
- harmonic decay curvature: wound 0.0065, plain 0.0090
- side-cluster decay: 0.95x main modal decay, up from 0.82x
- wound-string extra mode decay: 0.72 seconds + 0.055 seconds per harmonic index

Expected sound:

The loudest notes should keep the useful KS019/KS020 body and dynamics, but with the hard-pick edge restrained to the region that sounded better by ear. Held notes should speak for longer before disappearing, especially through the middle of the decay, without changing the fretboard mapper behavior.

## 2026-04-26 — Wound Taper and Softer Pick Attack

Listening feedback on KS021:

- The fretboard mapper is good.
- The wound-string character should taper by string: low E full strength, A about two-thirds, D about one-third.
- The current attack reads more like a hammer/fret impact than a pick.
- The velocity aggression range is still too wide; the old 68% strike feel should become the new 100% strike feel, with the whole range scaled down rather than only capped at the top.

Current behavior:

- The visible model label is now `StringVoice KS-022 WoundTaperPick`.
- `FretboardAssignment` now carries `woundAmount` in addition to a wound/plain flag.
- Standard tuning wound amounts are low E 1.00, A 0.67, D 0.33, G/B/high E 0.00.
- Wound amount now scales stiffness, damping curvature, side-mode spread/level, horizontal motion, extra winding modes, and related brightness behavior.
- Full MIDI velocity is remapped into the previous 0.05 to 0.68 strike-character range.
- The short pick transient and attack chirp modes are reduced.
- The modal output has a very short 2.5 ms onset ramp to soften the impact-like first samples.

Expected sound:

The low E should retain the liked wound-string behavior. A should still be wound, but lighter; D should be more subtle again. The top velocity should now feel closer to the previous 67-68% pick aggression, and the note start should be less hammer-like.

## 2026-04-26 — Clearer DI and Built-In Velocity Curve

Listening feedback on KS022:

- The wound progression from low E to D is improved.
- The dynamic range is improved, but the useful bottom and top ranges should be compressed into smaller MIDI-velocity regions.
- The tone sounds musically nice but like a warm, papery speaker or cone resonance is in the path.
- The clean DI target should feel less veiled, with clearer translated highs and less warm mid coloration.

Current behavior:

- The visible model label is now `StringVoice KS-023 ClearDI`.
- A piecewise velocity curve is now applied inside `StringVoice`.
- MIDI velocity 0.10 maps to internal velocity 0.30, compressing the old 0-30 range into roughly the first 10% of input.
- MIDI velocity 0.90 maps to internal velocity 0.80, so the old 80-100 region is pushed into roughly the 90-100 input range.
- The pickup aperture is narrowed, reducing pickup-width smoothing of higher harmonics.
- The post-mix `ElectricGuitarTone` stage now uses less low-passed body, more dry signal, and a little more presence.

Expected sound:

The playing response should feel more usable across authored MIDI velocities, with fewer notes falling into extreme soft or extreme hard regions. The output should be less warm/papery and more like a direct electric signal, without simply adding a broad treble boost.

## 2026-04-26 — Longer Held-Note Tail Sustain

Listening feedback on KS023:

- The clearer DI pass removed the papery speaker-cone coloration.
- Long held notes still die too quickly; after about five seconds they are nearly gone.
- The desired behavior is not compressor gain riding, but a more physical sustain where the note decays at first and then the remaining vibration hangs on longer.

Current behavior:

- The visible model label is now `StringVoice KS-024 TailSustain`.
- Each modal component now has a tail damping scale.
- Normal modal decay is used at the start of the note.
- Starting around 0.55 seconds, the modal damping gradually relaxes over about 1.6 seconds.
- Low/core modes receive the strongest late-tail damping reduction.
- Side modes and wound-string modes receive a gentler version of the same tail sustain.
- Attack chirp modes use the default short decay and are not extended.
- The voice energy cutoff is lowered so quiet sustained tails are not reset too early.

Expected sound:

Held notes should still decay naturally from the attack, but the later tail should stay audible for longer. This is meant to feel like lower physical damping in the vibrating string, not like a compressor pulling up the tail.

## 2026-04-26 — Automatable Sustain Parameter

The KS024 tail-sustain behavior is now exposed as a VST parameter.

Current behavior:

- The visible model label is now `StringVoice KS-025 SustainParam`.
- The parameter is named `Sustain`.
- Range is 0% to 100%.
- Default is 100%, preserving the KS024 behavior that sounded good by default.
- Lower values reduce the late-tail damping relaxation and raise the quiet-tail cutoff back toward the pre-KS024 behavior.
- The parameter is smoothed inside `AudioEngine` before reaching active voices.
- The plugin reports a nonzero tail length to the host.

Expected sound:

100% should match the long, supernatural KS024-style sustain. Lower values should move toward a more natural short sustain without compressor artifacts.

## 2026-04-26 — Pick Stiffness and Texture Parameters

The current pick/excitation layer now has two VST parameters.

Current behavior:

- The visible model label is now `StringVoice KS-026 PickMaterial`.
- Added `Pick Stiffness`, default 50%.
- Added `Pick Texture`, default 50%.
- 50%/50% is intended to preserve the KS025 pick feel.
- Stiffness affects contact width, onset ramp time, pick-edge transient, steel partial excitation, and attack-mode strength.
- Texture affects scrape/contact noise, horizontal excitation, and contact duration/decay.
- These values are sampled at note start, so changing them affects new plucks rather than reshaping already-ringing notes.

Expected sound:

Lower stiffness should feel softer and rounder. Higher stiffness should feel sharper and more snappy. Lower texture should approach a cleaner/slipperier release. Higher texture should add more pick-contact scrape and friction detail without changing the sustain model.

## 2026-04-26 — Pick Range Retune

The pick/excitation layer has been retuned after listening feedback on the first parameterized version.

Current behavior:

- The visible model label is now `StringVoice KS-027 PickRange`.
- `Pick Stiffness` keeps the useful hard-pick range but extends the soft end with a longer onset ramp, wider contact, and lower pick-edge/partial gains.
- `Pick Texture` now has a stronger perceptual range: 0% is effectively frictionless/no texture, 50% is close to the previous default, and 100% adds an intentionally audible scrape/contact component.
- The existing pick-contact envelope is now rendered into the output as a short high-passed scratch term; previously it was initialized but not mixed directly into `renderSample`.
- Texture still affects new note starts rather than reshaping already-ringing notes.

Expected sound:

The lowest stiffness should feel more flexible than KS-026. Texture should be much easier to hear at the extremes, with high values moving toward a rougher pick/coin-like contact and low values sounding cleaner.

## 2026-04-26 — Texture Audibility Fix

The pick texture path has been made intentionally obvious for diagnosis.

Current behavior:

- The visible model label is now `StringVoice KS-028 TextureDebug`.
- New note starts use the target pick-stiffness and pick-texture values immediately rather than the smoothed current values.
- Pick texture now renders a separate short contact component after the attack ramp so it is not masked by the modal onset fade.
- The texture contact includes high-passed scratch plus a short metallic ring component, aiming to make 100% texture unmistakably rough/coin-like.
- The offline renderer accepts `--sustain`, `--pick-stiffness`, and `--pick-texture` for A/B renders.

Expected sound:

At 0% texture, the attack should be clean/slippery. At 100% texture, the pick contact should be obviously scratchier and more metallic. This pass intentionally prioritizes audibility over final musical balance.

## 2026-04-26 — Stick-Slip Pick Texture

The upper half of `Pick Texture` is now treated as a different contact regime rather than a louder copy of the 50% pick snap.

Current behavior:

- The visible model label is now `StringVoice KS-029 StickSlipTexture`.
- 0% to 50% texture keeps the smoother plastic-pick snap range, with 50% remaining the anchor point.
- Above 50%, the smooth snap component is held back instead of simply getting louder.
- The extra high-texture behavior is an irregular stick-slip/grind layer: small interrupted impulses with a short metallic contact resonance.
- This is still an excitation approximation, not a full plectrum/string contact solver.

Expected sound:

50% should stay close to the useful smooth-plastic pick sound. Higher texture should move toward gritty surface drag or coin-like contact rather than a louder white-noise/snare-like snap.

## 2026-04-26 — Coin Pick Texture Extension

The top of the `Pick Texture` range now has a dedicated coin-like contact layer.

Current behavior:

- The visible model label is now `StringVoice KS-030 CoinTexture`.
- The KS-029 texture range is compressed so its previous 100% behavior is reached at about 80%.
- The 80% to 100% range adds a separate coin-edge layer only in that top segment.
- The coin layer uses faster irregular impulses plus a bumpy, ridged metallic phase function to suggest friction and uneven surface scrape.
- The goal is a Brian May/coin-pick style extreme, not a normal plastic pick.

Expected sound:

Below 80%, the texture control should preserve the useful KS-029 behavior. From 80% to 100%, the sound should move into a more overt gritty grind rather than only becoming brighter or louder.

## 2026-04-26 — Heavy Coin Grind Top-End

The top 5% of `Pick Texture` has been retuned after listening feedback that KS-030 became chirpy and bursty above about 95%.

Current behavior:

- The visible model label is now `StringVoice KS-031 HeavyCoinGrind`.
- The 80% to 95% coin-pick range remains mostly the KS-030 behavior.
- From 95% to 100%, the fast coin impulse layer is reduced instead of intensified.
- A darker heavy-contact layer fades in: low-frequency ridged friction, smoothed pressure variation, and mild choking of the freely ringing modal output.
- The intent is a coin that stays in heavier contact with the string rather than releasing bright chirps.

Expected sound:

Up to about 95% should remain useful as before. The last 5% should move darker and grindier, with fewer white-noise bursts and less artificial chirp.

## 2026-04-26 — Friction Drag Top-End

The last 5% of `Pick Texture` has been reworked again after KS-031 sounded like it rolled off the texture instead of adding a harsher grind.

Current behavior:

- The visible model label is now `StringVoice KS-032 FrictionDrag`.
- The 95% coin texture is preserved much more strongly when entering the 95% to 100% zone.
- The heavy-contact choke is reduced so the top end does not simply damp the texture away.
- The new extreme layer is a dense ridged-drag signal: smoothed friction pressure multiplied by a tooth-like carrier, rather than random white-noise bursts.
- The intent is a continuous coin-edge scrape/grind through the pick-contact duration.

Expected sound:

95% should still sound close to the useful coin texture. 100% should add a darker but harsher continuous grind, not a chirpy burst and not a simple rolloff.

## 2026-04-26 — Palm Mute Parameter

The string voice now has a global palm mute control.

Current behavior:

- The visible model label is now `StringVoice KS-033 PalmMute`.
- Added a `Palm Mute` VST parameter, default 0%.
- The parameter is smoothed in `AudioEngine` and applied to every voice every sample, so it can damp notes that are already ringing.
- Palm mute adds extra modal damping across the voice and slightly reduces freely ringing modal output at high values.
- Pick/contact output is left comparatively present, so high mute settings should sound like a strong palm mute: mostly pick/contact with very short string resonance.
- The offline renderer accepts `--palm-mute`.

Expected sound:

0% should match KS-032. Low values should lightly shorten sustain. Mid values should sound like playable palm muting. 100% should be close to a deadened string with a fast, percussive decay rather than a hard mute gate.

## 2026-04-26 — Palm Mute Curve

The palm mute response has been retuned after listening feedback.

Current behavior:

- The visible model label is now `StringVoice KS-034 PalmMuteCurve`.
- The mute sound model is unchanged from KS-033.
- The visible 0% to 50% slider range maps to the previous 0% to 25% mute range.
- The visible 50% to 100% slider range maps to the previous 25% to 100% mute range.

Expected sound:

The light palm-mute range should be easier to dial in. 50% should now resemble the previous 25% setting, while 100% remains the same full mute.

## 2026-04-26 — Harmonic Touch Parameter

Added a first note-start harmonic-touch articulation.

Current behavior:

- The visible model label is now `StringVoice KS-035 HarmonicTouch`.
- Added a `Harmonic Touch` VST parameter, default 0%.
- 0% to 25% leaves normal playing unchanged.
- 25% to 50% selects the 1/4 string-length harmonic family, matching the 5th/24th fret natural-harmonic region.
- 50% to 75% selects the 1/3 string-length harmonic family, matching the 7th/19th fret region.
- 75% to 100% selects the 1/2 string-length midpoint harmonic, matching the 12th fret octave harmonic.
- The top of each band is the most accurate node touch. Lower values in the band leak more normal-string partials, approximating a less precise touch.
- The implementation is a modal excitation mask at note start, not a continuous touch damper after the note has started.
- The offline renderer accepts `--harmonic-touch`.

Expected sound:

100% should produce the strongest and clearest octave harmonic. 75% should produce a quieter octave-plus-fifth harmonic. 50% should produce a still quieter two-octave harmonic. Values near the lower edge of each active band should sound less pure and more mixed with the normal string voice.

## 2026-04-26 — Harmonic Gesture Retune

Harmonic touch behavior was adjusted after listening feedback.

Current behavior:

- The visible model label is now `StringVoice KS-036 HarmonicGesture`.
- When `Harmonic Touch` is in an active harmonic band, the voice ignores the user-facing `Pick Stiffness` and `Pick Texture` values for that note start.
- Active harmonics use a soft, low-friction, no-texture excitation to approximate the index-finger touch plus thumb strike gesture.
- Normal non-harmonic notes still use the pick controls exactly as before.
- The harmonic slider UI now shows `1/4`, `1/3`, and `1/2` markers at the most accurate point for each harmonic family.
- The palm mute response was retuned again: 0% to 50% now covers the previous 0% to 25% visible mute range, and 50% to 100% compresses the rest of the mute range.

Expected sound:

Harmonics should no longer inherit coin/texture/stiff-pick artifacts from the normal pick controls. Palm mute should offer more fine control for barely-touching mutes while preserving the full-mute endpoint.

## 2026-04-26 — String Age Parameter

Added a first old-string/new-string control.

Current behavior:

- The visible model label is now `StringVoice EG-037 StringAge`.
- The visible model series has changed from `KS` to `EG` because the voice is now a broader electric-guitar model rather than a simple Karplus-Strong experiment.
- Added a `String Age` VST parameter, default 0%.
- 0% represents clean/new strings.
- 100% represents older strings that are still usable: duller, less lively, and slightly less sustaining, but not dead.
- Higher age reduces upper-partial excitation, pick/contact brightness, moving resonance energy, and high-mode sustain.
- The offline renderer accepts `--string-age`.

Expected sound:

Increasing `String Age` should darken the string at the source, especially in upper harmonics, and slightly shorten the ring. It should not sound like a drastic low-pass filter or broken strings.

## 2026-04-26 — Pickup Model and Position

Added the first controllable pickup model.

Current behavior:

- The visible model label is now `StringVoice EG-038 PickupModel`.
- Added a `Pickup Model` choice parameter:
  - `Single Coil`
  - `Humbucker`
  - `Singles OOP`
- Added a `Pickup Position` parameter, default 39%.
- The pickup position maps from a bridge-side readout at about 0.055 of the speaking length to a neck-side readout at about 0.335 of the speaking length.
- The UI marks 1/6, 1/5, 1/4, and 1/3 string-length positions as harmonic landmarks.
- `Single Coil` uses a narrow single readout.
- `Humbucker` uses two nearby readouts summed together with a wider aperture and gentle high-partial smoothing.
- `Singles OOP` uses the difference between two wider-spaced single-coil readouts for a thinner, notched, neck/middle-style out-of-phase response.
- The offline renderer accepts `--pickup-position` and `--pickup-model`.

Expected sound:

Bridge-side pickup positions should be brighter and lower in fundamental weight. Neck-side positions should become warmer and fuller, with audible harmonic peaks/nulls rather than a plain EQ sweep. Humbucker should be thicker and smoother than single coil. Singles OOP should be thinner and more nasal.

## 2026-04-26 — Bridge Intonation

Added the first setup imperfection control.

Current behavior:

- The visible model label is now `StringVoice EG-039 BridgeIntonation`.
- Added a `Bridge Intonation` VST parameter, default 0%.
- 0% is perfect bridge intonation.
- 100% applies a deterministic, per-string saddle-offset error that should be noticeably imperfect but not wildly unusable.
- Open strings are unaffected.
- Fretted notes drift more as the assigned fret gets higher, using the current fretboard mapper's string/fret assignment.
- The offline renderer accepts `--bridge-intonation`.

Expected sound:

Low-position playing should remain close to normal. Higher fretted notes should reveal more pitch disagreement between strings as the control increases.

## 2026-04-26 — Fret Pressure and Parameter Info

Added the first fretting-performance pitch imperfection control and lightweight in-plugin help.

Current behavior:

- The visible model label is now `StringVoice EG-040 FretPressure`.
- Added a `Fret Pressure` VST parameter, default 0%.
- 0% adds no fretting-pressure pitch error.
- 100% models over-gripping or landing too far behind the fret by bending fretted notes sharp.
- Open strings are unaffected.
- The sharp offset scales with the current fretboard assignment: lower strings receive more shift than higher strings, and higher frets receive more shift than lower frets.
- The current implementation samples `Fret Pressure` at note start, like `Bridge Intonation`; automation affects newly struck notes, not already-ringing voices.
- Added small info buttons and marker tooltips in the editor so parameter ranges, harmonic bands, pickup landmarks, and project intent are visible without filling the UI with explanatory text.
- The offline renderer accepts `--fret-pressure`.

Expected sound:

At low settings the control should add a little human pitch sharpness to fretted notes. At high settings, fretted notes should become noticeably over-gripped, especially on lower strings and higher frets, while open strings stay stable.

## 2026-04-26 — UI Popovers and Stronger Fret Pressure

Polished the parameter help and widened the practical range of `Fret Pressure`.

Current behavior:

- The visible model label is now `StringVoice EG-041 UIPopovers`.
- Info buttons now open click-to-dismiss JUCE callout popovers instead of relying on hover tooltips.
- The editor is slightly wider, uses a cleaner header band, and gives labels more room.
- `Fret Pressure` still leaves open strings untouched, but 100% now produces a more obvious sharp bend on fretted notes.
- The pressure curve remains strongest on lower strings and higher frets.

Expected sound:

The top of the `Fret Pressure` range should be clearly audible as over-gripped fretted sharpness rather than only a barely detectable drift. Lower settings should still be useful for subtle human pitch error.

## 2026-04-26 — Lookahead Finger Noise

Added the first playback-oriented human finger-noise layer.

Current behavior:

- The visible model label is now `StringVoice EG-042 LookaheadNoise`.
- Added a `Lookahead` VST parameter with `Off`, `150 ms`, and `250 ms`.
- `Off` remains the default for live playability and does not delay MIDI note handling.
- `150 ms` and `250 ms` report matching plugin latency with JUCE `setLatencySamples`.
- When lookahead is enabled, note-on and note-off messages are delayed internally by the selected amount.
- Added a `Finger Noise` VST parameter, default 0%.
- With lookahead enabled, incoming note-on events trigger a short filtered approach scrape before the delayed note speaks.
- Incoming note-off events trigger a short release/fret-contact noise before the delayed note release.
- The first noise model uses fixed-size scheduling and fixed-size noise voices to avoid audio-thread allocation.
- The noise layer is stronger on fretted notes, lower strings, and wound strings.
- The offline renderer accepts `--lookahead-ms` and `--finger-noise`.

Expected sound:

With `Lookahead` off, playback should remain immediate and finger noise should effectively stay out of the way. With lookahead on and `Finger Noise` raised, rendered playback should include short pre-note and pre-release motion noises. In a DAW with plugin delay compensation, the delayed note should line up with the MIDI grid while the finger noise appears just before the note.

## 2026-04-26 — Finger Vibrato

Added a first fretting-hand pitch-vibrato control set.

Current behavior:

- The visible model label is now `StringVoice EG-043 FingerVibrato`.
- Added a separate `Vibrato` editor section.
- Added `Vibrato Speed`, in Hz.
- Added `Vibrato Depth`, in cents.
- Added `Vibrato Delay`, in milliseconds.
- The vibrato delay waits for the chosen time after note start, then ramps in over the same duration.
- At 0 ms delay, vibrato starts immediately.
- Added `Mod Wheel To Speed` and `Mod Wheel To Depth` toggles.
- When enabled, MIDI CC1 adds to the selected baseline vibrato parameter instead of replacing it.
- The implementation modulates per-voice modal oscillator phase increments, so each active voice has its own vibrato phase.
- This is global MIDI/mod-wheel vibrato for now, not MPE per-note pitch bend.
- The offline renderer accepts `--vibrato-speed`, `--vibrato-depth`, and `--vibrato-delay-ms`.
- The footer/version text now draws in a fixed reserved strip to avoid overlap.

Expected sound:

Depth at 0 cents should be unchanged. Raising depth should produce fretting-hand pitch wobble. Delay should keep the note straight at first, then bring vibrato in gradually. Enabling mod-wheel depth and/or speed should allow CC1 to intensify the vibrato from the chosen baseline settings.

## 2026-04-26 — Whammy Pitch Wheel

Added a first global pitch-wheel bend layer that behaves like a simplified tremolo/vibrato-arm gesture.

Current behavior:

- The visible model label is now `StringVoice EG-044 WhammyBend`.
- The build/version identity moved from the crowded footer into the editor header.
- Added a separate `Whammy` editor section.
- Added `Pitch Wheel Whammy`, which enables or disables pitch-wheel control.
- Added `Whammy Up Range`, in semitones, defaulting to +6.
- Added `Whammy Down Range`, in semitones, defaulting to -12.
- Added `Whammy String Spread`, which makes strings respond by slightly different intervals under larger pitch-wheel moves.
- Pitch wheel is still global MIDI pitch behavior, not MPE per-note bend.
- The simplified spread model is inspired by tremolo-arm physics: bridge motion changes string tension, and wound/unwound string construction can produce different pitch intervals for the same bridge motion.
- The offline renderer accepts `--pitch-wheel`, `--whammy-up`, `--whammy-down`, and `--whammy-spread`.

Expected sound:

With `Pitch Wheel Whammy` enabled, the MIDI pitch wheel should bend all active notes together. With spread at 0%, bends should behave like a clean global pitch shifter. Raising spread should make wider bends sound less perfectly parallel across strings, especially in chords, while remaining subtle near the default.

## 2026-04-26 — Per-Note Aftertouch Bend

Added a first key/poly aftertouch pitch-bend mapping.

Current behavior:

- The visible model label is now `StringVoice EG-045 AftertouchBend`.
- Added `Aftertouch Bend` to the `Performance` editor section.
- The parameter range is -12 to +12 semitones, defaulting to +2 semitones.
- 0 semitones effectively disables the mapping.
- Incoming MIDI key/poly aftertouch is routed by note number and channel to matching active voices.
- Full aftertouch pressure bends by the selected range; partial pressure scales proportionally.
- Each voice smooths its own aftertouch pressure amount to avoid zippery pitch movement.
- Channel pressure is intentionally not mapped yet.
- This is a small pre-MPE feature. Full MPE pressure routing remains a separate later milestone.
- The offline renderer accepts `--aftertouch-bend` and `--aftertouch` for quick diagnostics.

Expected sound:

With the default +2 semitone range, increasing key/poly aftertouch on a held note should smoothly bend only that note upward. Chords should reveal whether a host or controller is truly sending per-note aftertouch: pressure on one note should not move the others.

## 2026-04-26 — MPE Per-Channel Pitch Bend

Added the first true MPE pitch-bend routing milestone.

Current behavior:

- The visible model label is now `StringVoice EG-046 MPEBend`.
- Added an `MPE` editor section.
- Added `MPE Mode`, default Off.
- Added `MPE Bend Range`, defaulting to ±48 semitones.
- At this milestone, when `MPE Mode` is enabled, pitch wheel messages are routed by MIDI channel to active voices on that same channel.
- When `MPE Mode` is disabled, pitch wheel retains the existing global whammy behavior.
- The MPE pitch amount is smoothed per voice.
- If a DAW sends multiple notes on one channel, those notes bend together; independent bends require separate MPE member channels.
- The offline renderer accepts `--mpe-mode` and `--mpe-bend-range`.
- Later `EG-054 MPEWhammy` preserves this behavior for member channels 2-16 while reserving lower-zone channel 1 pitch wheel for global whammy.

Expected sound:

With Bitwig set to the same 48-semitone MPE bend range, drawing a two-semitone note expression bend should produce a two-semitone bend. In a chord where Bitwig assigns separate channels, bending one note should leave the other notes stable.

## 2026-05-02 — MPE Pressure And CC74 Expression

Added the next MPE expression routing milestone.

Current behavior:

- The visible model label is now `StringVoice EG-047 MPEExpr`.
- Added `MPE Pressure Amount`, defaulting to 65%.
- Added `MPE CC74 Amount`, defaulting to 65%.
- Channel pressure is routed by MIDI channel to active voices on that channel.
- CC74/timbre is routed by MIDI channel to active voices on that channel.
- Pressure adds a conservative held-note lift: more sustain, slightly more level, and more upper-mode intensity.
- CC74 leans the held voice brighter/more bridge-like by emphasizing upper modes and their decay.
- Key/poly aftertouch bend remains separate and still uses the `Aftertouch Bend` parameter.
- The offline renderer accepts `--mpe-pressure-amount`, `--mpe-cc74-amount`, `--channel-pressure`, and `--cc74`.

Expected sound:

With notes on separate MPE member channels, pressure or CC74 applied to one note should change only that voice. The mapping is deliberately musical rather than dramatic; it should add expression without making the base patch jump when no expression data is sent.

## 2026-05-02 — Player Articulation First Pass

Added a first automatic player-articulation milestone.

Current behavior:

- The visible model label is now `StringVoice EG-048 PlayerArtic`.
- Added `Legato Articulation`, defaulting to 0%.
- 0% preserves the existing picked-note path.
- Above 20%, recent same-string descending candidates can become pull-offs.
- Above 30%, recent same-string ascending candidates can become hammer-ons.
- Above 70%, larger same-string high-fret ascending moves can become right-hand taps.
- The decision layer uses a small fixed-size recent-note memory and deterministic probability, so repeated renders are stable.
- The fretboard mapper can now prefer a requested same string for eligible legato gestures.
- Hammer-ons, pull-offs, and taps use distinct excitation profiles rather than only changing velocity.
- Non-picked gestures suppress pick scrape/material layers and stay below picked-note energy at the DI level.
- Hammer-ons add a short fret-impact envelope.
- Pull-offs add a lateral release/snap envelope.
- Right-hand taps add a sharper, brighter fret-impact envelope with a higher energy ceiling than left-hand hammer-ons.
- This first pass still starts a fresh voice for the destination note; it does not yet preserve and retune a live same-string delay/modal state.
- The offline renderer accepts `--legato-articulation`.

Expected sound:

At 0%, the feature-audition MIDI should sound effectively unchanged. At 100%, same-string scalar phrases should lose some pick attack and level, descending moves should get softer/grabbier pull-off starts, and large high-register ascending moves should acquire a sharper tap-like contact.

## 2026-05-02 — Amp Feedback First Pass

Added a first one-knob speaker-feedback approximation.

Current behavior:

- The visible model label is now `StringVoice EG-049 AmpFeedback`.
- Added `Amp Feedback`, defaulting to 0%.
- The control lives on the Performance page because it acts like the player moving closer to a loud amp/speaker.
- The implementation is not an amp, cab, microphone, or room model.
- Low values mostly lift late modal decay for a subtle resonant sustain effect.
- Higher values bias energy into harmonically related modal components.
- The top of the range adds a controlled harmonic howl component while clamping modal energy to avoid runaway clipping.
- Feedback is injected into active string voices before the pickup/tone stage, so pickup position can still color the resulting harmonic balance.
- The offline renderer accepts `--amp-feedback`.

Expected sound:

At 0%, existing picked and legato behavior should be unchanged. At moderate settings, held notes should sustain more readily and feel closer to a loud rig. At 100%, long held notes should lean into harmonic overtones and controlled howl without acting like a full distortion or feedback-loop effect.

## 2026-05-02 — Feedback Loop Resonator Pass

Reworked the high-feedback behavior so it can produce a more realistic winner/takeover effect.

Current behavior:

- The visible model label is now `StringVoice EG-050 FeedbackLoop`.
- The public control remains `Amp Feedback`.
- Low slider values still use the EG-049 local sustain behavior.
- The upper range now wakes up a small global feedback loop in `AudioEngine`.
- The loop listens after pickup/tone shaping and runs eight fixed resonant bands.
- Each band tracks a slow envelope; the current dominant band wins with a little hysteresis.
- The loop sends its dominant frequency, amount, and phase-like signal back into active string voices.
- `StringVoice` couples that loop mainly into modal components near the winning absolute frequency rather than all harmonics equally.
- The older per-note harmonic lift is reduced as the global loop takes over, so 100% feedback is less even across strings/modes.
- There is still no full room, amp, cabinet, microphone, or guitar-body model.

Expected sound:

Compared with EG-049, high `Amp Feedback` should feel less like every note gets evenly sustained and more like one amp/speaker resonance starts to dominate. Long notes should be the clearest test: one upper resonance should begin to hold or bloom, and changing notes should give the loop a chance to hand off to another band.

## 2026-05-03 — Six-String Voice Cap

Changed voice allocation to behave more like a physical six-string guitar and to bound the high-feedback CPU case.

Current behavior:

- The visible model label is now `StringVoice EG-051 SixStringCap`.
- The core `AudioEngine` voice pool is now six `StringVoice` instances.
- Each new note is still assigned to a standard-tuned string/fret by `FretboardMapper`.
- If the assigned physical string is still ringing, the existing voice for that string is reused for the new note.
- If no voice is active on the assigned string, the allocator uses an inactive voice, then falls back to the existing round-robin steal path.
- This preserves the current sound model inside each `StringVoice`; the change is allocation behavior, not modal tone generation.

Expected behavior:

A six-note guitar-range chord should ring without stealing. A seventh simultaneous note or a repeated note on the same assigned string should replace one physical string voice instead of allowing more than six string voices to accumulate. This should especially reduce the high `Amp Feedback` case where released strings previously stayed active until all eight old voices were running.

Future exception:

The six-voice cap applies to physical strings. A later pickup/body microphonic model may use a seventh auxiliary source for body taps, handling noise, or pickup vibration, but that source should be explicitly non-string and mixed as part of the pickup/body DI path.

## 2026-05-03 — Passive Modal Fast Path

Optimized the existing EG-051 model without intentionally changing its sound.

Current behavior:

- The visible model label is now `StringVoice EG-052 PassiveFast`.
- Each note tracks how many modal slots were actually configured and renders only that active modal range.
- Per-mode constants such as mode position, high/low expression weights, palm-mute weighting, and modal frequency are computed at note start instead of every sample.
- When pitch ratio is exactly neutral, the voice reuses precomputed modal sine/cosine phase steps instead of recalculating them every sample.
- When vibrato, whammy, aftertouch bend, MPE bend, or amp feedback are exactly inactive, their ratio/feedback math is bypassed.

Expected behavior:

This pass should be nearly or fully transparent for normal playback. It does not reduce modal count, shorten tails, add amplitude cutoffs, or change the six-string allocation rule. If a render differs, the expected difference should come from floating-point operation ordering rather than a deliberate tone change.

## 2026-05-03 — Spaced Single-Coil Out Of Phase Pickup

Revised the third pickup model so it behaves more like two spaced single-coil pickups out of phase.

Current behavior:

- The visible model label is now `StringVoice EG-053 SinglesOOP`.
- The plugin version is now `0.2.2`.
- The third pickup choice is now labeled `Singles OOP`.
- `Single Coil` and `Humbucker` are unchanged.
- `Singles OOP` uses two single-coil-width readouts spaced by 0.086 of the speaking string length.
- The `Pickup Position` control moves the pair together while preserving that spacing.
- The pair is constrained so the bridge-side and neck-side readouts stay equally spaced inside the existing pickup-position range.
- The pair subtracts the bridge-side readout from the neck-side readout, creating a wider-spaced phase-cancellation pattern than the old nearby-coil humbucker OOP model.

Expected sound:

Compared with the old third pickup model, `Singles OOP` should be less like a tiny differential humbucker notch and more like a nasal two-pickup out-of-phase sound. It should stay thin and notched, but the notches should move from wider pickup spacing rather than mostly cancelling nearly identical coil positions.

## 2026-05-03 — MPE Master Channel Whammy

Allowed the whammy pitch wheel to keep working while `MPE Mode` is enabled.

Current behavior:

- The visible model label is now `StringVoice EG-054 MPEWhammy`.
- The plugin version is now `0.2.3`.
- In normal MIDI mode, pitch wheel still feeds the global whammy path.
- In MPE mode, pitch wheel on lower-zone channel 1 feeds the global whammy path.
- In MPE mode, pitch wheel on member channels 2-16 still feeds per-channel MPE pitch bend.
- The whammy amount is still shaped by `Whammy Up Range`, `Whammy Down Range`, and `Whammy String Spread`.

Expected behavior:

MPE note-expression pitch bends should remain independent by member channel, while a DAW or controller that sends pitch wheel on channel 1 can still move all active strings like a tremolo arm. Upper-zone MPE master-channel behavior is still not modeled.

UI notes:

The in-plugin info popovers now use a simple first sentence followed by a `Technical:` detail line where useful. This is meant to keep controls approachable while still exposing the model behavior for users who want to understand what each parameter is doing.

## 2026-05-03 — Feedback String Focus And Clipped Return

Retuned the dominant-band feedback loop so it can take over one physical string instead of evenly reinforcing every active string.

Current behavior:

- The visible model label is now `StringVoice EG-055 FeedbackFocus`.
- The plugin version is now `0.2.4`.
- `AudioEngine` now tracks a dominant feedback string in addition to the dominant feedback band.
- Each active `StringVoice` reports a cheap coupling score against the current loop frequency using its configured modal frequencies and modal energy.
- As the loop amount rises, the focused string receives a stronger loop return while other strings receive less.
- Local all-string feedback sustain is reduced as the focused loop takes over, so high feedback should lean toward one runaway note/harmonic instead of lifting everything evenly.
- Added `Feedback Return Distorted`, exposed in the UI as `Distorted Return`, which clips the signal that feeds the feedback resonator bank without adding amp/cab processing to the main DI output.
- The offline renderer accepts `--feedback-return-distorted`.

Expected behavior:

At low settings, `Amp Feedback` should still mostly feel like extra loud-rig sustain. At high settings, held notes and chords should more clearly develop a dominant string/harmonic that can take over and, depending on the loop band and string energy, hand off to another string. `Distorted Return` should make that loop react more like a clipped high-gain amp return while keeping the plugin output clean DI-style.

## 2026-05-03 — Feedback Bloom After Note Attacks

Listening in Bitwig showed that the focused feedback worked better when the `Amp Feedback` slider was held near zero during the note attack, then raised during the sustain. EG-056 automates that behavior with a MIDI-note-on bloom envelope.

Current behavior:

- The visible model label is now `StringVoice EG-056 FeedbackBloom`.
- The plugin version is now `0.2.5`.
- New note-ons temporarily duck the effective amp-feedback amount before it reaches the string voices and feedback resonator loop.
- Picked notes duck the loop most. Right-hand taps, hammer-ons, and pull-offs duck it less so legato gestures can remain connected.
- Notes in the same short attack cluster share the duck event, so a strummed or block chord does not repeatedly flush the feedback loop for every string.
- The ducked feedback blooms back toward the slider setting over roughly 0.7 to 1.5 seconds, depending on the feedback amount.
- When the effective amount is near zero, the resonator bank no longer listens to the dry attack at full input strength.
- A fresh attack lightly reduces the previous loop state and clears the dominant string focus, giving the new note or chord a chance to become the next feedback winner.

Expected behavior:

At high `Amp Feedback` settings, picked attacks should stay more like clean guitar attacks instead of instantly exciting every string or harmonic. The feedback should arrive later in the sustain, making it easier for one harmonic or string to take over after the chord has settled. Fast note streams should also release more naturally because instant feedback is no longer extending every new attack.

Render check:

- `tests/midi/guitar-ag-feature-audition.mid`, `Amp Feedback` 100%, clean return: 98.146 seconds rendered in 10.067 seconds, 9.749x realtime, average 2.487 active string voices, max 6.
- Same MIDI with `Distorted Return` on: 98.146 seconds rendered in 10.157 seconds, 9.663x realtime, average 2.477 active string voices, max 6.
- `tests/midi/guitar-ag-player-articulation-audition.mid`, `Amp Feedback` 100%, clean return: 61.223 seconds rendered in 2.568 seconds, 23.841x realtime, average 1.164 active string voices, max 3.

## 2026-05-03 — Distorted Feedback Return Default

After auditioning EG-056 in Bitwig, the clipped feedback return sounded more natural than the clean return. The clean return exposed a small early chirp as the feedback loop hunted across harmonics; the clipped return provided denser amp-like excitation and masked that band-hunt.

Current behavior:

- The visible model label is now `StringVoice EG-057 FeedbackBloom`.
- The plugin version is now `0.2.6`.
- `Feedback Return Distorted`, shown as `Distorted Return`, now defaults on for new plugin instances.
- The offline renderer also defaults `--feedback-return-distorted` to `1`.
- Turning `Distorted Return` off is still available as a cleaner diagnostic or alternate feedback flavor.

Expected behavior:

The default `Amp Feedback` sound should now use the more natural clipped return while the main instrument output remains clean DI-style. Existing saved DAW projects may preserve their stored switch value; new instances should open with `Distorted Return` enabled.

## 2026-05-09 — Lookahead Expression Sync And Normal MIDI Expression Fallback

Reviewer feedback found that lookahead delayed notes and key/poly aftertouch while applying pitch wheel, mod wheel, channel pressure, and CC74 immediately. This could desync expression from delayed note playback at `150 ms` or `250 ms` lookahead.

Current behavior:

- With lookahead enabled, note on/off, key/poly aftertouch, pitch wheel, mod wheel, channel pressure, and CC74 are scheduled through the same fixed MIDI delay queue.
- Finger approach/release noises still trigger immediately from incoming note on/off so they can precede the delayed note event.
- MPE mode changes now clear per-channel pitch bend, pressure, and CC74 state.
- In normal MIDI mode, channel pressure and CC74 apply globally to all active voices and seed new voices with the current global expression value.

## 2026-05-09 — EG-058 Pitch Control-Rate Cache

Optimized the pitch-modulation path identified by reviewer feedback.

Current behavior:

- The visible model label is now `StringVoice EG-058 PitchControlRate`.
- Each `StringVoice` still smooths aftertouch and MPE pitch values every sample.
- Expensive pitch ratios for vibrato, whammy, aftertouch bend, and MPE bend are recalculated every 4 samples instead of every sample.
- When pitch modulation is active, adjusted modal phase-step sine/cosine values are cached every 4 samples instead of being recomputed inside every modal slot on every sample.
- Neutral renders with no pitch modulation remain byte-identical against the pre-optimization baseline in the player-articulation smoke renders, including `Amp Feedback` at 100%.

Measured offline render checks:

- Player-articulation MIDI, feedback 0%, before/after: 37.636x to 43.545x realtime in one local run; WAVs were byte-identical.
- Player-articulation MIDI, feedback 100%, before/after: 17.726x to 17.702x realtime in one local run; WAVs were byte-identical.
- Feature-audition MIDI with MPE enabled, before/after: 31.692x to 37.858x realtime in one local run.

The MPE feature render changes at waveform level because pitch modulation is now control-rate; the 4-sample interval measured about 0.74% relative RMS difference against the previous per-sample render. Manual listening and DAW MPE checks should decide whether this is perceptually transparent enough.

## 2026-05-09 — EG-059 Feedback Weight Cache

Optimized the high-feedback modal branch after Antigravity verified EG-058 and cleared the next pass.

Current behavior:

- The visible model label is now `StringVoice EG-059 FeedbackWeightCache`.
- Each `StringVoice` caches per-mode feedback harmonic weights and loop-lock weights at an 8-sample control interval when feedback is active.
- The cache moves expensive harmonic-lock and loop-lock calculations, including `round`, `exp`, and `log2`, out of the per-mode/per-sample inner loop.
- Per-sample energy gates, feedback rise, release scaling, loop signal scaling, modal decay, and feedback injection remain per-sample.
- The exact global feedback-loop `std::tanh` saturation remains unchanged.
- A first attempted global feedback scalar cache was measured and discarded because it did not improve local high-feedback render speed.

Measured offline render checks:

- Player-articulation MIDI, `Amp Feedback` 100%, before/after: 17.768x to 29.543x realtime in one local run.
- Feature-audition MIDI with MPE enabled and `Amp Feedback` 100%, before/after: 9.315x to 19.648x realtime in one local run.
- Player-articulation MIDI, `Amp Feedback` 0%, sample data stayed identical against the EG-058 no-feedback render.
- High-feedback render sample differences were small in the local WAV comparison: about 0.0015% relative RMS for player-articulation and 0.0199% relative RMS for feature-audition.

Manual listening should confirm that high-feedback bloom, string focus, and harmonic takeover still feel natural.

## 2026-05-09 — EG-060 Contact Trig Fast Path

Optimized the transient contact and gesture layer after Antigravity accepted the EG-059 feedback weight cache and agreed that speculative feedback-loop `tanh` replacement should stay rejected.

Current behavior:

- The visible model label is now `StringVoice EG-060 ContactTrigFast`.
- `StringVoice::renderSample` uses a contact-local sine approximation for pick-contact ring, grind, coin, heavy-pick, finger-impact, and pull-off transient tones.
- The contact burr term now computes `abs(sin)^7` with explicit multiplications instead of `std::pow`.
- Exact `std::tanh` remains in the contact soft-clip/ridge path and in the global feedback loop.
- The modal string path, MPE pitch path, and feedback weight cache are unchanged.

Measured offline render checks:

- Player-articulation MIDI with `Pick Texture` 100%, `Pick Stiffness` 100%, `Finger Noise` 60%, and `Amp Feedback` 0%: 35.965x to 38.020x realtime in local sequential runs. Max block time fell from 1.705 ms to 0.671 ms in those runs.
- Player-articulation MIDI, default pick/contact settings and `Amp Feedback` 0%: 43.446x baseline to 43.705x realtime in one local run.
- Player-articulation MIDI, `Amp Feedback` 100%: 30.445x baseline to 30.103x realtime in one local run.
- Heavy contact render difference against EG-059: about 0.060% relative RMS.
- Default player-articulation render difference against EG-059: about 0.0012% relative RMS.
- High-feedback player-articulation render difference against EG-059: about 0.0012% relative RMS.

Manual listening should focus on pick scrape, coin edge, heavy pick rasp, hammer-on/tap impact, and pull-off snap. The intentionally exaggerated pick-contact render changes more than default playing because it drives the newly approximated transient tones hardest.

## 2026-05-09 — EG-061 Render Helpers

Refactored `StringVoice::renderSample` after Antigravity accepted EG-060 and recommended a modal/contact maintainability pass.

Current behavior:

- The visible model label is now `StringVoice EG-061 RenderHelpers`.
- The modal execution and feedback-injection loop now lives in `renderModalBank`.
- Pick transient decay now lives in `renderPickTransient`.
- Pick/contact, finger-impact, and pull-off transient generation now lives in `renderContactLayer`.
- The arithmetic and update order are preserved; this is intended as a behavior-neutral readability pass.
- No modal count, feedback cache, pitch cache, contact approximation, or saturation behavior changed.

Measured offline render checks:

- Player-articulation MIDI with default settings and `Amp Feedback` 0%: audio sample data matched EG-060 exactly.
- Player-articulation MIDI with `Pick Texture` 100%, `Pick Stiffness` 100%, `Finger Noise` 60%, and `Amp Feedback` 0%: audio sample data matched EG-060 exactly.
- Player-articulation MIDI with `Amp Feedback` 100%: audio sample data matched EG-060 exactly.

Because this pass is behavior-neutral, reviewer focus should be on helper boundaries, naming, and whether future profiling/maintenance work now has cleaner surfaces.

## 2026-05-09 — EG-062 Neck Slide

Added the first global slide playback lane after the slide gesture research and Phase 1 audition assets.

Current behavior:

- The visible model label is now `StringVoice EG-062 NeckSlide`.
- New plugin instances expose an automatable `Neck Slide` parameter on the Performance page.
- `Neck Slide` ranges from `-24.0 st` to `+24.0 st` and defaults to `0.0 st`.
- Internally, the lane is a smoothed global semitone offset passed from `AudioEngine` into each active `StringVoice`.
- `StringVoice::updatePitchRatio` layers neck slide additively with vibrato, whammy, aftertouch bend, and MPE pitch bend.
- The final aggregate pitch ratio is clamped against the active voice's base frequency before pitch step caching.
- Adjusted per-mode pitch steps are also clamped to the safe frequency range before sine/cosine steps are recalculated.
- The offline renderer accepts `--neck-slide`.

Scope limit:

- This pass is pitch-motion only. It does not yet add fret-crossing noise, slide velocity scrape, open/muted slide tails, or same-string speaking-length preservation.

Measured offline checks:

- `--neck-slide 0` matched the default no-flag render exactly on the slide gesture audition MIDI.
- `--neck-slide 12` produced a different render, confirming the new lane affects playback.
- An extreme stacked render with `--neck-slide 24`, full pitch wheel whammy, and full aftertouch bend completed successfully through the clamp path.

## 2026-05-09 — EG-063 Fret Slide

Refined the first slide lane after human DAW listening. The original smooth two-octave lane moved pitch correctly, but sounded too much like a tension/whammy effect instead of a fretted hand shifting along the neck.

Current behavior:

- The visible model label is now `StringVoice EG-063 FretSlide`.
- `Neck Slide` now ranges from `-12.0 st` to `+12.0 st` and defaults to `0.0 st`.
- New plugin instances expose a `Fret Steps` amount control on the Performance page, defaulting to 65%.
- `Fret Steps` blends the `Neck Slide` pitch contribution toward semitone plateaus with a smoothed half-fret transition between adjacent fret positions.
- The fret-stepped shaping applies only to the global `Neck Slide` component; MPE pitch bend, whammy, aftertouch bend, and vibrato remain continuous.
- While `Neck Slide` moves across a semitone boundary, `StringVoice` adds a small fret-crossing contact tick and short scrape burst scaled by `Fret Steps` and string woundness.
- When `Neck Slide` is static or `Fret Steps` is 0%, the fret-contact layer stays silent.
- The offline renderer accepts `--slide-fret-steps`.

Scope limit:

- This pass approximates fret engagement/disengagement. It does not yet model a true same-string speaking-length transition, slide-in/out tail modes, or explicit notation import.

## 2026-05-09 — EG-064 Fret Step Curve

Recalibrated the `Fret Steps` control after human DAW listening. The EG-063 sound direction was accepted, but the first 90% of the control barely exposed audible fret engagement.

Current behavior:

- The visible model label is now `StringVoice EG-064 FretStepCurve`.
- `Fret Steps` still stores and displays as a normal 0% to 100% automatable parameter.
- Internally, the control is remapped so 0% to 10% covers the former 0% to 90% engagement range.
- The remaining 10% to 100% of the control covers the former 90% to 100% engagement range, giving most of the slider to the audible fret-crossing zone.
- This perceptual mapping feeds both semitone plateau shaping and fret-crossing contact ticks/scrape.

## 2026-05-09 — EG-065 Slide Tail

Added the first slide-ending behavior after EG-064 was accepted by human audition and reviewer verification.

Status: superseded by EG-066 `SlideLift`.

Current behavior:

- The visible model label is now `StringVoice EG-065 SlideTail`.
- New plugin instances expose a `Slide Tail` choice on the Performance page: `Normal`, `Muted`, `Open`, and `Slide Off`.
- `Normal` is the default and keeps existing note-off behavior.
- The non-normal modes only engage when a voice has recent `Neck Slide` motion, tracked inside `StringVoice` as a short decaying slide activity envelope.
- `Muted` quickly damps the modal bank and adds a small release scrape.
- `Open` leaves a more ringing release color with a light pull-off/scrape accent.
- `Slide Off` applies the strongest damping and scrape/contact accent for a quick off-the-string style exit.
- `GuitarAGOfflineRender` accepts `--slide-tail`, using `0 = Normal`, `1 = Muted`, `2 = Open`, and `3 = Slide Off`.
- `GuitarAGOfflineRender --neck-slide-at <seconds>` starts the render at a neutral slide offset, then throws the `--neck-slide` target during rendering. This is a block-granular calibration hook for repeatable slide-tail checks.

Scope limit:

- `Open` is a first-pass release color. It does not yet retune the voice to a true open string or update the physical string/fret assignment.
- Special slide tails are intentionally gated by recent slide movement so settled chord releases and ordinary note-offs stay unchanged.

## 2026-05-09 — EG-066 Slide Lift

Replaced EG-065's note-off-based `Slide Tail` choice with a continuous pressure-lift model.

Current behavior:

- The visible model label is now `StringVoice EG-066 SlideLift`.
- The Performance page exposes `Slide Lift` as a 0% to 100% slider.
- `Slide Tail` has been removed from the plugin and engine.
- `Slide Lift` only acts from recent `Neck Slide` motion; ordinary static notes and non-sliding releases stay on the normal release path.
- Higher lift values fade the effective `Fret Steps` amount after its perceptual mapping, so the slide stops catching fret plateaus as the finger pressure lifts.
- Higher lift values add a light lifted-string scrape while the slide is moving.
- Higher lift values add modal damping while the lift envelope is active, making the slide behave more like a progressive mute.
- The lift rise time is tied to the slider: 50% reaches its lift over about 500 ms, while 100% reaches full lift over about 50 ms.
- `GuitarAGOfflineRender` accepts `--slide-lift`; `--slide-tail` was removed.
- `GuitarAGOfflineRender --neck-slide-at <seconds>` remains as a block-granular calibration hook for moving-slide checks.

Scope limit:

- `Slide Lift` models pressure easing and damping during global `Neck Slide`. It does not yet retune the string to an open string or update same-string speaking length.

## 2026-05-09 — EG-067 Finger Squeak

Human DAW listening found that the EG-066 slide-lift control was musically useful, but the existing finger/string noise read too much like white or pink hiss. EG-067 revoices the contact noise layers around friction squeak:

- `AudioEngine::FingerNoiseVoice` now has filtered scrape state plus short stick-slip impulse state.
- Global `Finger Noise` approach/release events excite a string/fret-dependent squeak carrier instead of relying primarily on high-passed random noise.
- Wound strings receive stronger, more frequent ridged friction pulses. Plain strings keep a smoother, quieter scrape profile.
- `StringVoice::renderContactLayer` uses the same filtered-friction/ridge-squeak direction for `Slide Lift` and fret-slide scrape.
- The broad random scrape component remains, but at a lower weighting so it supports contact texture rather than dominating the sound.

Research references used for the model direction:

- Babici, Tudor, and Romeu, “Stick-Slip Phenomena and Acoustic Emission in the Hertzian Linear Contact” (2022): https://doi.org/10.3390/app12199527.
- Groves and Kemp, “Applicability of the Capstan Equation to Guitar Strings” (2019): https://doi.org/10.24425/aoa.2019.129261.
- Djellouli et al., “Squeaking at soft-rigid frictional interfaces” (2026): https://www.nature.com/articles/s41586-026-10132-3.

## 2026-05-10 — EG-068 Motion-Coupled Slide Squeak

Human spectrum/listening feedback on EG-067 showed that the finger/slide noise still sounded like a stable hiss floor with small ridges, and it did not change enough between fast and slow slides.

EG-068 changes the slide path from an event-like scrape layer into a motion-coupled contact layer:

- `StringVoice` now tracks `slideFretMotionDrive`, a short envelope raised by recent `Neck Slide` velocity.
- Slide squeak pulse spacing becomes denser as slide speed increases and sparser as speed decreases.
- The slide squeak carrier is retuned from the current modal/string frequency and recent slide speed instead of staying near the note-start value.
- `slideFretScrapeDecay` is shorter so squeak/scrape falls away quickly when movement stops.
- The random scrape component in slide noise is reduced and gated by motion so it supports the squeak instead of becoming a steady hiss.
- The global `Finger Noise` approach/release layer keeps its existing trigger model but has less broadband scrape weighting.

## 2026-05-10 — EG-069 Slide Squeak Control

EG-069 adds a separate balance control for the motion-coupled slide squeak:

- The Performance page exposes `Slide Squeak` as a 0% to 200% slider.
- 100% preserves the EG-068 slide-squeak balance.
- The control scales only the slide contact squeak/scrape output in `StringVoice::renderContactLayer`.
- It does not change `Neck Slide` pitch, `Fret Steps`, `Slide Lift` damping, or the global `Finger Noise` approach/release layer.
- `GuitarAGOfflineRender` accepts `--slide-squeak` for calibration renders.

## 2026-05-10 — EG-070 Performance UI Groups

Human audition found the useful `Slide Squeak` range around 20%, and the Performance page was becoming too dense. EG-070 keeps the existing top-level tabs but adds a second hierarchy on the Performance page:

- `Neck Slide` owns `Fret Steps`, `Slide Lift`, and `Slide Squeak`.
- `Finger Noise` owns `Lookahead`.
- `Amp Feedback` owns `Return Clip`.
- Secondary rows keep the same slider alignment but use quieter labels.
- `Slide Squeak` is now a 0% to 100% control and defaults to 20%.

## 2026-05-10 — EG-071 Slide Squeak Direction

Human audition found that upward slide squeak and downward slide squeak benefit from separate balances. EG-071 keeps the same contact model but splits the amount control by movement direction:

- The existing `slideSqueak` parameter ID is preserved and host-facing renamed to `Slide Squeak Up`.
- A new `slideSqueakDown` parameter exposes `Slide Squeak Down`.
- Both controls range from 0% to 100% and default to 20%.
- `StringVoice` stores the last active `Neck Slide` direction when the slide offset changes, so the short decaying contact tail keeps the direction of the motion that produced it.
- The split only scales the slide contact squeak/scrape contribution in `renderContactLayer`; pitch, fret-step shaping, slide-lift damping, MPE, whammy, and global `Finger Noise` are unchanged.
- `GuitarAGOfflineRender` keeps `--slide-squeak` as the legacy/upward flag, also accepts `--slide-squeak-up`, and adds `--slide-squeak-down`.

## 2026-05-10 — EG-072 Pick Stroke Plectrum

Human audition after EG-071 found that staggered chords feel much more convincing than isolated single-note picked attacks. EG-072 starts the pick realism work with one normal plastic plectrum model:

- `Pick Stroke` is a host parameter with `Down`, `Up`, and default `Alternate` modes.
- Alternation advances only for `PlayerGesture::Picked` note starts. Hammer-ons, pull-offs, and right-hand taps receive their existing gesture models and do not consume the alternating stroke.
- `AudioEngine` resolves a concrete `PickStrokeDirection` and deterministic attack seed before starting a `StringVoice`.
- The attack seed includes note, channel, assigned string/fret, render timeline sample, stroke direction, gesture, and a resettable attack counter. Re-rendering the same MIDI and parameters stays byte-identical, while repeated note attacks no longer reuse a note/channel-only fingerprint.
- Down and up strokes slightly change release polarization, horizontal string motion, pluck position, contact amount, and contact carrier scale.
- The default plastic-pick region has less exposed tonal contact ring and shorter chirp/transient decay. High `Pick Texture` still reaches rougher/coin-like contact regimes, but the ordinary midpoint is protected from sounding like a synthetic oscillator attack.
- `GuitarAGOfflineRender` accepts `--pick-stroke down|up|alternate`.

## 2026-05-10 — EG-073 Plastic Pick Bite

Human DAW audition found EG-072 useful but still too much like a substantial brittle/glassy pick on isolated notes. EG-073 keeps the same single-plastic-plectrum scope and changes the physical interpretation:

- Added `Pick Bite` as a 0% to 100% parameter. It is the strength of the pick/string contact imprint, not a separate pick-click volume.
- Low `Pick Bite` leaves the attack closer to pure string displacement/release. Higher values emphasize edge contact, short scrape, contact ring, chirp modes, and high-texture regimes.
- The default contact model is less rigid:
  - rough `highTexture` behavior starts later in the `Pick Texture` range;
  - local pick contact is broader/complier at normal settings;
  - contact ring level, decay, and carrier frequency are reduced;
  - chirp modes are lower, shorter, and more age-damped.
- Deterministic attack variation now includes extra pick-depth and pick-angle scatter, affecting pluck offset, contact strength, contact carrier, and attack-mode gain.
- `String Age` now darkens the pick-contact fingerprint more explicitly:
  - it lowers coherent contact ring/chirp brightness;
  - it shortens bright contact decay;
  - it lowers the contact-scratch high-pass coefficient so older-string pick scrape is less brand-new and glassy.
- `GuitarAGOfflineRender` accepts `--pick-bite`.

Physical framing for future player interpretation:

- The pickup hears string motion, not an independent pick-noise bus.
- Controls should map to player/string interactions where possible: pick compliance, stroke direction, contact bite, fretting pressure, slide motion, and timing.
- When a practical amount slider is needed, it should scale the modeled interaction terms rather than bypass the physical signal path.

## 2026-05-10 — EG-074 Economy Pick Stroke

Human audition found that simple down/up toggling was useful for repeated single-string notes, but not for chord strokes or string-to-string movement. EG-074 keeps the `Pick Stroke` choices unchanged and makes the existing `Alternate` mode right-hand-aware:

- `Down` and `Up` still force a single stroke direction.
- `Alternate` now remembers the last picked string and last picked stroke.
- If the next picked note is on the same assigned string, `Alternate` flips direction.
- If the next picked note moves from a lower string index to a higher string index, `Alternate` uses a downstroke.
- If the next picked note moves from a higher string index to a lower string index, `Alternate` uses an upstroke.
- This applies to adjacent strings and string skips.
- Hammer-ons, pull-offs, and right-hand taps do not consume or advance the pick-stroke state.

Current string-index convention:

- `0` is low E.
- `5` is high E.
- Increasing index means low-to-high physical string travel, modeled as a downstroke.
- Decreasing index means high-to-low physical string travel, modeled as an upstroke.

This is the first small right-hand player-interpretation model on top of the fretboard heuristic.

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

Current simplification:

- Pickup readout currently lives inside `StringVoice` as a fixed normalized read position.
- `ElectricGuitarTone` is a post-voice conditioning stage.
- This is not yet a full per-string, per-pickup, pickup-width, or circuit model.
- Pickup/body microphonics are intentionally not part of the six-string voice count. If added later, model them as a simple auxiliary non-string voice or body/pickup bus for body taps, handling thumps, or pickup vibration against otherwise static strings.

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

## 2026-05-10 — EG-075 Player Feel

Added the first cause-based player-feel layer above picked note starts.

- `Player Feel` controls the amount of deterministic timing and energy variation.
- `Feel Recovery` controls how quickly cognitive load, dexterity load, and endurance clear.
- `Reset Feel` clears the accumulated load and the feel-model string/fret memory, like starting a new take.
- The model uses a separate `FretboardMapper` to estimate the string/fret path at MIDI input time without disturbing the audio voice allocator.
- Cognitive load rises with direction changes, string skips, fret jumps, and fast passages.
- Dexterity load rises with fast same-string repetition, very fast picking, string travel, string skips, and awkward reversals.
- Endurance rises more slowly from the cognitive/dexterity impulses and recovers more slowly than either immediate load.
- The output is intentionally modest: picked note-ons may be delayed by a few milliseconds and note-on velocity may be scaled slightly.
- At `Player Feel = 0%`, the path is neutral. The default offline render matched the explicit `--player-feel 0` render byte-for-byte.

This is not random DAW humanization. It is deterministic from musical context and render position, so repeated offline renders with the same MIDI and settings remain byte-identical.

Limitation: the first pass can only safely delay note starts in real time. Early timing would require lookahead/latency compensation. The engine also still treats a re-picked held chord tone as a new note event; held-shape repick semantics remain a separate follow-up.

## 2026-05-10 — EG-076 Player Feel Calibration

Human DAW audition of EG-075 found the feel useful, but the accepted natural setting lived at the top of the slider. EG-076 retunes the control and adds visibility/export support:

- `Player Feel` is scaled internally so 50% is near the previous EG-075 100% feel.
- The top half of `Player Feel` intentionally overshoots into sloppier late timing and stronger velocity/energy variation.
- `Feel Recovery` defaults to 2.0 seconds and can extend to 8.0 seconds for longer accumulated-load carryover.
- The audio engine decays the visible load state at audio-block boundaries and exposes current cognitive load, dexterity load, and endurance as real-time-safe meter values.
- The plugin UI polls those meters at 20 Hz and shows them on the Articulation page near the feel controls.
- `Export Settings` emits a compact JSON snapshot of current APVTS parameter values plus the live Player Feel meters.

Verification:

- Build/install succeeded locally.
- Offline renders with `Player Feel = 0`, `50%`, and `100%` produced distinct hashes.
- Repeating the same `Player Feel = 100%` render produced byte-identical WAVs.

## 2026-05-10 — EG-077 Player Feel Slop

Human audition of EG-076 found two follow-up issues:

- cognitive/dexterity/endurance meters accumulated to maximum too quickly;
- `Player Feel = 100%` still sounded natural rather than clearly sloppy.

EG-077 changes the player-feel transfer function without adding new parameters:

- Cognitive and dexterity impulses are smaller across fast picking, string skips, fret jumps, and direction changes.
- Endurance accumulates more slowly from cognitive/dexterity impulses.
- `Player Feel` keeps the natural midpoint, but values above 50% add a quadratic overshoot term.
- At high settings, timing delay can now reach a wider cap of about 75 ms instead of the previous 28 ms cap.
- Velocity/energy variation has a wider top-end range, especially when load and the overshoot term are both high.
- `Export Settings` moved to the global header because it describes the whole plugin state, not only articulation.

The model remains deterministic: it still uses musical context and stable render position, not arbitrary per-render randomness.

## 2026-05-10 — EG-078 Strum-Aware Player Feel

Human audition after EG-077 clarified that chord strums should not accumulate player load like a run of unrelated single-note picks. EG-078 adds a small strum-continuation heuristic inside `AudioEngine::processPlayerFeelNoteOn`:

- If a note-on follows the previous picked note within about 70 ms;
- and it moves to a different, near-adjacent string;
- and it has a clear cross-string travel direction;
- then it is treated as a continuing strum stroke for Player Feel load accounting.

For these continuation notes:

- cognitive impulse is reduced strongly;
- dexterity impulse is reduced substantially;
- timing/energy variation still uses the accumulated load and remains deterministic.

This does not schedule strums from block chords yet. It only prevents existing staggered strum MIDI from overcharging the load model. Full Auto Strum planning lives in `plans/0079-auto-strum-interpreter.md`.

## 2026-05-10 — EG-079 Auto Strum Exact-Block Interpreter

EG-079 adds the first Auto Strum implementation for chords whose MIDI note-ons arrive at the exact same sample position.

Implementation shape:

- `AudioEngine::render` groups MIDI messages with the same block sample position before dispatching them.
- When a same-sample group contains at least two note-ons and `Strum Speed` is above 0%, the engine predicts fretboard string assignments for the chord.
- Groups containing same-sample controller/expression events bypass Auto Strum and use the normal dispatch order for this first pass.
- The predicted notes are sorted by `Pick Stroke` direction:
  - `Down`: low-string index toward high-string index;
  - `Up`: high-string index toward low-string index;
  - `Alternate`: follows existing right-hand memory, including repeated chord down/up alternation.
- Each note-on receives an internal delay based on string distance. At 100%, adjacent strings are spaced by about 100 ms, so a six-string chord can spread to roughly half a second.
- The predicted string assignment is remembered until the delayed note-on fires, so the final `FretboardMapper` assignment matches the strum order that was calculated up front.
- Player Feel now receives the strum delay as the effective note-event time, so cognitive/dexterity load decays and accumulates along the generated stroke instead of treating all block-chord notes as zero-time events.

Neutral behavior:

- `Strum Speed = 0%` bypasses the Auto Strum path.
- Single-note MIDI is unchanged.
- Already-staggered authored strums are still handled by the EG-078 strum-continuation heuristic, not rescheduled.

Known limitation:

This pass intentionally does not collect notes that arrive a few samples apart. Supporting DAW jitter/tolerance windows will require an explicit lookahead/collection design so live latency and host compensation remain understandable.

## 2026-05-10 — EG-080 Strum Direction Balance

Human DAW audition accepted the EG-079 Auto Strum sound and requested a simple way to make generated downstrokes and upstrokes differ in strength.

EG-080 adds `Strum Balance`:

- Center/0.0 is balanced and preserves EG-079 generated velocities.
- Positive values reduce generated upstroke note velocities.
- Negative values reduce generated downstroke note velocities.
- The extreme ends reduce the selected stroke direction by about 94%, leaving near-ghosted strokes rather than absolute zero.

Implementation details:

- The balance is applied inside `AudioEngine::handleAutoStrumGroup` after the group stroke direction is resolved and before the note-on enters Player Feel.
- The control only changes generated Auto Strum note-on velocities.
- Single-note picking and already-staggered authored strums are unaffected.
- Player Feel still applies deterministic timing/energy variation after the direction balance, so the generated stroke remains part of the same player-interpretation path.

## 2026-05-10 — EG-081 Articulation Groups and Defaults

EG-081 is a release-candidate usability/defaults pass rather than a new DSP algorithm.

- The visible model label is now `StringVoice EG-081 ArticGroups`.
- The Articulation page now groups secondary character controls under their parent gesture:
  - `Pick Bite` with `Pick Stiffness` and `Pick Texture`;
  - `Pick Stroke` with `Strum Speed` and `Strum Balance`;
  - `Player Feel` with `Feel Recovery`, `Reset Feel`, and the load meters.
- New-instance and offline-render defaults now start closer to the accepted audition setup:
  - `Pick Texture`: 25%;
  - `Strum Speed`: 10%;
  - `Strum Balance`: -13%;
  - `Player Feel`: 50%.

These defaults make block-chord previews lightly strummed and human-feeling immediately, while existing sessions should retain their saved parameter values.

## 2026-05-17 — Plan 0082 Fretboard Heuristic Improvements

This pass uses the visible model label `StringVoice EG-082 FretboardHeuristics`. It changes the performance-interpreter heuristics, not the string model itself.

Fretboard behavior:

- `FretboardMapper` now keeps mutable string-open notes for its own instance.
- If a note below standard E2 arrives, string 0 is tuned down to that exact MIDI note before candidate scoring.
- The drop state persists until `FretboardMapper::reset()`.
- `getDropSemitones()` exposes the current string-0 drop amount for diagnostics.
- `getFretForString()` now reads the mapper instance tuning, so legato-source checks respect the current drop state.

Assignment behavior:

- Single-note scoring keeps the existing position-memory shape but adds a small register-affinity term so low notes prefer lower strings and high notes prefer higher strings in close calls.
- Same-sample block chords now enter the chord-aware path even when `Strum Speed = 0%`; the inter-string delay collapses to zero, but the string assignment is still predicted as a chord.
- Partial block chords smaller than six notes use a fixed-size group search over string/fret candidates. This prevents greedy dyads such as G4+A4 from choosing high-E fret 3 plus B-string fret 10 when B-string fret 8 plus high-E fret 5 is a better combined grip.
- Full six-note block chords keep the existing sequential preview assignment path.

Verification notes:

- A mapper probe confirmed D2 maps to string 0 fret 0 with `getDropSemitones() == 2`.
- D2+A2 maps to string 0 fret 0 plus string 1 fret 0.
- After low-position context, G4+A4 maps to string 4 fret 8 plus string 5 fret 5.
- Offline D2 render favored D2 over E2 by 56.12 dB in the analysis window.

## 2026-05-17 — EG-083 Input Octave Mode

EG-083 adds an explicit `Input Octave` choice because DAWs disagree about visible octave names.

- `MIDI E2=40` leaves incoming MIDI note numbers unchanged. This is the scientific/guitar mapping where MIDI note 40 is low E2.
- `DAW E2=52` subtracts one octave at the MIDI ingestion boundary. This lets DAWs that label MIDI note 40 as E1 play the guitar low E when the piano roll shows E2.
- The transpose is applied before fretboard assignment, note-off matching, key/poly aftertouch, finger-noise assignment, Player Feel, and Auto Strum grouping. It is not a post-synthesis pitch shifter.
- New plugin instances default to `DAW E2=52` for the current Bitwig audition workflow. Set `Input Octave` to `MIDI E2=40` for existing MIDI clips already authored around MIDI note 40.

## 2026-05-17 — EG-084 Chord Legato Guard

EG-084 fixes a performance-interpreter collision between block-chord assignment and automatic legato gestures.

- The input-octave switch still only changes incoming MIDI note numbers before guitar interpretation.
- Chord-aware Auto Strum assignments now bypass legato-source lookup when the generated note-on reaches the voice path. A block chord that previewed separate strings keeps those strings even when `Legato Articulation` is high.
- Legato-source lookup now requires a candidate source note to have existed briefly before it can become a hammer-on, pull-off, or right-hand-tap source. This keeps same-instant or near-same-instant chord tones from stealing each other.
- Offline verification with a DAW-label E2+B2 fixture showed default settings at max 2 active string voices, and `Legato Articulation = 100%` now also reaches max 2 active string voices instead of collapsing to 1.

## 2026-05-17 — EG-085 Slide Panic Reset

EG-085 fixes a stale performance-state path exposed by `Neck Slide` auditioning in Bitwig.

- `Neck Slide` still applies after native guitar note/fret assignment; it does not retune the fretboard mapper.
- When `Neck Slide` is set back to neutral, the engine now snaps its smoothed slide value to `0.0 st` instead of letting a tiny residual glide survive into later note starts.
- Standard MIDI panic messages are now handled: CC120 All Sound Off and CC123-127 All Notes Off clear active voices, scheduled note events, articulation notes, finger-noise assignments, feedback focus, and fretboard occupancy.
- Before a new note-on group is interpreted, the audio engine reconciles fretboard occupancy against the actual active string voices. If a host stopped playback without ordinary note-offs, stale occupied-string slots no longer bias the next chord assignment.

## 2026-05-17 — EG-086 Panic Reset Control

EG-086 exposes the panic cleanup path directly for DAW debugging and automation.

- The Setup page has a `Panic Reset` button that requests the audio engine panic cleanup on the next audio block.
- A new automatable `Panic Reset` parameter appears in host parameter lists. It triggers when automated from below 50% to 50% or higher, so a short 0% to 100% pulse can be drawn in Bitwig.
- The reset path clears active voices, scheduled note events, fretboard occupancy, articulation memory, finger-noise assignments, feedback focus, pitch/mod controller state, and player-feel memory.

## 2026-05-17 — EG-087 Assignment Diagnostics

EG-087 adds assignment instrumentation for diagnosing Bitwig clips that leave the fretboard mapper unable to reuse apparently free strings.

- The visible model label is now `StringVoice EG-087 AssignmentDiagnostics`.
- The audio engine keeps a fixed-size rolling diagnostics buffer for the last 1000 incoming MIDI, assignment, note-off, and panic-reset events. It uses preallocated atomic slots so the audio thread does not allocate or block.
- Each assignment event records host note, engine note after input-octave transpose, velocity, string/fret, preferred string, strum-preferred string, legato-source string, stolen voice, mapper occupancy mask, active-voice mask, input transpose, current drop tuning, `Neck Slide`, and `Legato Articulation`.
- The editor header shows a six-string status strip. Green means mapper occupancy and active voice state agree; amber means the mapper still considers a string occupied even though no voice is active on that string.
- `Copy Log` opens and copies a JSON snapshot with the live string statuses and rolling event log for handoff/debugging.

## 2026-05-17 — EG-088 Mixed MPE Chord Assignment

EG-088 fixes the Bitwig MPE event shape that made chord-aware string assignment disappear partway through a song.

- The visible model label is now `StringVoice EG-088 MixedMpeChordAssignment`.
- Same-sample groups may contain pitch-wheel, CC74, channel pressure, note-off, and note-on events together. The audio engine now routes expression and note-off events first, then still sends the remaining note-ons through the chord-aware assignment path.
- Incoming-MIDI diagnostics now record post-routing mapper and voice masks instead of a stale zeroed `after` snapshot for events that only update controller or note-off state.
- Bass-register scoring now penalizes high-fret reuse of the low string when a low-fret neighboring string is a musically closer option, while preserving explicit chord-preview string choices.

## 2026-08-27 — Plan 0090 Stateful Waveguide A/B

Plan 0090 adds a second, offline-only string engine without changing the VST3's legacy modal sound.

`StatefulStringVoice` currently contains:

- one fixed-size primary and secondary delay loop per physical string
- fractional reads for smooth pitch/MPE movement
- slight polarization detuning and coupling
- frequency-dependent loop loss and separate polarization damping
- a finite-aperture magnetic pickup read plus temporal pickup filtering
- a nonlinear finite plectrum trajectory with compliance, damping, and release threshold
- deterministic attack variation that changes contact geometry rather than mixing audible noise
- optional state-preserving repicks on an already vibrating physical string

The delay state is interpreted as a force/velocity traveling wave, so pickup output reads the spatially averaged wave directly before temporal filtering and DC blocking. Differentiating that signal again produced an overly bright, short-lived attack and was rejected during the implementation pass.

The contact force is injected into the two string polarizations and is never mixed directly into output. The stateful path does not render the legacy pick chirp/ring bank or global finger-noise voices.

The engine is compiled only when `GUITAR_AG_ENABLE_STATEFUL_ENGINE` is defined for `GuitarAGOfflineRender`. The VST3 target does not compile or instantiate the experimental delay buffers.

Current limitations are deliberate:

- a changed fret retunes the persistent loop but does not yet move a physical fret/finger boundary
- hammer-on, pull-off, tap, slide, harmonic touch, and finger noise do not yet have stateful contact models
- amp-feedback focus diagnostics still belong to the legacy engine
- the stateful note body is less dense than legacy at peak-matched level
- the existing tone stage is shared rather than recalibrated for the new pickup output

Windows verification at 48 kHz:

- legacy single-note calibration SHA-256 remained `C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`
- the focused stateful render repeated byte-identically
- preserved-state and reset-on-note renders were identical before the repick section and diverged during the eight F2 repicks
- the six-voice Auto Strum render reached 33.0x real time, with a 3.03 ms maximum offline block and a -2.4 dBFS peak
- the stateful single-note reference diagnostic moved full/attack/early/late log-spectral distance from legacy `40.42/39.19/40.76/40.43 dB` to `30.88/26.88/28.58/33.05 dB`

The distance movement is encouraging but not an acceptance result. Stateful
spectral-flatness ratio remains too high and flux remains low relative to the
selected Guitar-TECHS examples. Human listening decides whether the new
attack/body is musically preferable.

## 2026-08-27 — Plan 0091 Legacy Layer Ablation

Human A/B listening rejected the Plan 0090 stateful engine as synth-like and
described the accepted legacy engine as having a spectral-chirp attack, a
mirror-like glassy body, and pick/finger noises that sound laid over the string.

The source topology supports that diagnosis. `StringVoice` renders its modal
bank, then separately adds `renderPickTransient()` and `renderContactLayer()`.
`AudioEngine` separately generates global finger-noise voices made from filtered
noise, a sinusoidal ridge carrier, and stick-slip impulses, then adds them to the
summed strings before `ElectricGuitarTone`. Those finger noises do not currently
excite or interact with a physical/modal string state.

The offline renderer now exposes three research-only switches:

- `--legacy-attack-modes 0|1`
- `--legacy-pick-transient 0|1`
- `--legacy-contact-layer 0|1`

`GUITAR_AG_ENABLE_OFFLINE_ABLATION` is defined only for
`GuitarAGOfflineRender`; the VST3 does not compile the switches. Disabled chirp
modes retain their modal slots and frequencies with zero amplitude, preventing
other modes from filling the space and contaminating the ablation.

The known default legacy calibration remains byte-identical at SHA-256
`C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`.
The attack and finger-noise comparison sets are stereo 48 kHz and exactly
486,000 samples (10.125 seconds) per file. Subtraction residuals expose the
aligned final-output difference after the shared tone stage.

`String Age` is not a single tone roll-off. It simultaneously reduces pick
brightness, shortens/darkens contact layers, steepens high-modal damping, and
shortens several side/winding/chirp decays. It can therefore hide glassiness
without identifying whether the root cause is the entry transient or modal body.

Human listening found that removing the short chirp modes was barely different
at ordinary pick settings. The explicit transient/contact layers supply audible
pick identity, but removing them did not reveal a different sustained body. The
high E4 instead sounded like a lower-register guitar model digitally transposed
upward, suggesting that the harmonic/modal construction is too pitch-invariant.

A targeted rerender used 100% `Pick Bite`, 10% `Pick Stiffness`, and 75%
`Pick Texture`. The current attack read as a sparse, pronounced woody rattle;
disabling the short chirp modes again changed little. The isolated correctly
subtracted pick/contact contribution sounded like crude digital synthesis rather
than a plectrum/string material interaction. The deep-pick defect therefore
lives primarily in the independently rendered transient/contact path, not the
short modal chirp modes. The next prototype should replace that path with a
finite-duration force/energy injection into the modal string so the pickup hears
the string's response instead of an additive attack signal.

The isolated finger-noise residual was rejected as resembling a stiff plastic
hair comb. Its periodic stick impulses and multi-sine ridge carrier are therefore
the wrong foundation. The accepted design target is continuous motion-speed-
driven friction hiss plus less-periodic transverse/bowing motion, with finger
depth optionally coupling into a restrained string/harmonic-position squeak.

The first residual-render command was invalid: FFmpeg `amix` treated a negative
weight as a magnitude and summed the two inputs, producing a residual exactly
6 dB louder and perceptually similar to the sources. Correct residuals use
`amerge` plus explicit `pan` channel subtraction. Adding each corrected residual
back to its ablated source reconstructs the original with infinite measured
audio PSNR.

## 2026-08-27 — Plan 0092 Modal-Coupled Pick Excitation

Plan 0092 adds an offline-only alternative to the direct picked transient and
contact output:

- `--legacy-pick-excitation additive|modal`
- `--legacy-modal-pick-force 0.0..3.0`

In `modal` mode, a picked note receives a deterministic half-sine plectrum-force
envelope lasting roughly 1–7 ms. Pick stiffness shortens the contact, bite sets
the impulse, and texture adds bounded non-periodic roughness to the force. The
force updates each configured oscillator's position/velocity quadrature; it is
not mixed into audio directly. Coupling follows the existing configured modal
amplitudes, so the experiment inherits the current pluck position, pickup
response, aperture, string/side-mode recipe, and modal decay. The existing
modal representation already bakes pickup response into amplitude, so this is a
controlled listening test of state-coupled excitation, not yet a complete
separation of physical string state from pickup observation.

For picked notes in this mode, `renderPickTransient()` is skipped and the picked
section of `renderContactLayer()` is disabled. Later slide/fret contact remains
available. Non-picked gestures retain their existing behavior. The selector,
force scale, extra state, and per-sample injection are compiled only into
`GuitarAGOfflineRender`; the VST3 neither stores nor executes them.

Both Windows Release targets build. The normal calibration render remains byte-
identical at SHA-256
`C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`,
and repeated 1.75x modal renders are byte-identical. The prepared deep-pick set
is stereo 48 kHz and exactly 486,000 samples per file. Its isolated 1.75x modal
contribution reconstructs the coupled source from the body-only track with
infinite measured audio PSNR.

Human listening selected the 1.75x modal-force version as the useful foundation
for the next attack pass. It still needs some of the current additive texture,
but at a much lower level and with greater event density. This preserves the
modal string response as the main attack while testing whether a subordinate,
less sparse surface-friction contribution can supply missing pick material.

The same listening pass described the high-register failure by analogy to vocal
pitch shifting without formant correction: the first low note sounds relatively
natural, while upper notes acquire a small/Munchkin-like identity. This matches
the current implementation's extensive use of harmonic number for contact
filtering, partial tilt, decay curvature, and pickup electrical tilt. Those
spectral envelopes therefore move upward with the fundamental. A separate
offline experiment should keep modal pitches harmonic while anchoring more of
the material/pickup envelope in absolute frequency; do not combine that change
with the hybrid-pick comparison.

## 2026-08-27 — Plan 0093 Hybrid Pick Texture

The offline renderer now accepts:

- `--legacy-additive-pick-mix 0.0..1.0`
- `--legacy-pick-texture-density 1.0..6.0`

These controls act only when modal pick excitation is selected. The direct mix
scales the old picked transient, contact scratch/ring, grind, coin, heavy-contact
state, and heavy choke together. It does not scale subsequent slide/fret contact.
At zero mix, the Plan 0092 modal-only output is exactly preserved.

Density shortens grind/coin slip intervals while scaling each new impulse by
`1 / sqrt(density)`. This makes events overlap more often without intentionally
turning density into an equivalent loudness control. The shared nonlinear tone
path still makes exact perceptual level preservation impossible, so the A/B
holds mix constant when comparing sparse and dense candidates.

Both Windows Release targets build, and the default calibration remains byte-
identical at SHA-256
`C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`.
Zero direct mix reproduces the Plan 0092 1.75x render at SHA-256
`9FC966858E60532AF269F331DE486D4F75707D24B5A7419E669C59C69D702094`.
Repeated 22% / 2.5x-density hybrid renders are byte-identical. The isolated
dense contribution reconstructs the full hybrid from the modal foundation with
infinite measured audio PSNR and measures -56.8 dB mean / -31.4 dB peak.

Human loudness-matched listening accepted 12% direct texture at 2.5x density as
a good medium setting and 22% at the same density as a good maximum. The useful
hybrid therefore has a calibrated range: modal force remains the foundation,
while the denser legacy layer contributes 12–22% surface detail. Use 12% as the
neutral baseline when testing register/body changes so maximum pick texture does
not mask their effect.

## 2026-08-27 — Plan 0094 Register/Formant Envelope Anchor

The offline renderer now accepts `--legacy-register-anchor 0.0..1.0`. Zero uses
the existing harmonic-number envelope exactly. Above zero, each harmonic's
envelope index moves geometrically toward its absolute frequency divided by the
low-E2 reference frequency (82.40689 Hz).

The anchored index controls contact-width filtering, partial tilt, string-age
damping, pickup electrical tilt, and modal decay curvature. The mechanical
amplitude terms are normalized at each note's fundamental before applying the
fixed-Hz spacing, while pickup electrical tilt and decay retain absolute-
frequency behavior. Modal pitch, inharmonic stiffness, pickup-position nodes and
aperture, pluck geometry, and harmonic attack emphasis remain pitch-relative.

A literal unnormalized anchor reduced high-E mean level by roughly 4/8/14 dB at
35/65/100% and was rejected as a confounded audition. The final offline test adds
a smooth register-dependent level compensation proportional to
`(frequency / E2)^(1.08 * anchor)`. This is an audition normalization, not a
proposed production loudness law. It holds per-note mean levels within about
1 dB of the current fixture so listening can focus on perceived instrument scale.

Both Windows Release targets build. Default calibration remains byte-identical
at SHA-256
`C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`,
and anchor zero exactly reproduces the accepted Plan 0093 medium hybrid. The 65%
render repeats byte-identically at
`51AE6DBEC942F150469C731AA91B02AB0789F1C5D8F12D3ACAE80BC811DE0D57`.
All five A/B files are stereo 48 kHz and 424,800 samples. Adding the signed
`baseline - 65%` difference to the 65% render reconstructs baseline with infinite
measured audio PSNR.

Human loudness-matched listening selected 35% as much better than the current
harmonic-number envelope. The upper notes nevertheless flattened and lost some
metal ring/brightness as pitch rose. Treat this as evidence for a partial
fixed-Hz envelope, not permission to promote the scalar unchanged. The next
offline test should keep 35% amplitude anchoring fixed, independently restore
harmonic-number modal decay, and optionally blend a small amount of unanchored
amplitude only into higher-frequency inharmonic side modes. This targets metal
identity without restoring the broad pitch-shifted body or the rejected attack
chirp modes.

## 2026-08-27 — Plan 0095 Decay and Metal-Ring Separation

The offline renderer now accepts two controls that leave the 35% amplitude
envelope unchanged:

- `--legacy-register-decay-anchor 0.0..1.0` overrides decay anchoring. If it is
  omitted, decay follows `--legacy-register-anchor` exactly for compatibility.
- `--legacy-register-metal-restore 0.0..8.0` adds back the difference between
  anchored and unanchored inharmonic side-mode amplitude. A smooth 650–2500 Hz
  gate and `1 - 0.72 * woundAmount` weighting focus the change on upper/plain
  metal-string cues. Main harmonic modes and the attack chirp bank are untouched.

A factor of 1 is the literal lost-energy restoration after frequency/string
weighting, but its E4 contribution measured only -60.6 dB mean. The focused A/B
uses 6x as a labelled diagnostic exaggeration, roughly -45 dB mean on E4, so
the side-mode hypothesis is actually hearable. It is not a proposed control
range or production default.

The comparison is a 2x2: anchored versus harmonic-number decay, each with zero
or 6x side-mode restoration. The accepted attack and 35% amplitude anchor remain
fixed. Both Windows Release targets build; the default calibration remains exact
at `C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`,
and the default-follow 35% render exactly reproduces Plan 0094 at
`94BA731900865E612887B9FE3CE6FB533323C14302D878418FD518F9C5CEBEF7`.
All five review tracks are 424,800 samples, the combined candidate repeats
byte-identically, and its signed added contribution reconstructs it from the
accepted baseline with infinite measured audio PSNR.

Human loudness-matched listening called the combined harmonic-number-decay plus
6x side-mode candidate "pretty good." Treat that as a positive combined result,
not proof that both single-axis candidates were individually necessary: neither
received a written verdict. The next no-code calibration should hold
harmonic-number decay fixed and compare 0x, 2x, 4x, and 6x restoration, with 6x
first as the accepted reference and an isolated 6x metal contribution. Select
the lowest factor that retains the improvement.

## 2026-08-27 — Plan 0096 Metal-Restoration Calibration

Plan 0096 makes no DSP or VST3 changes. It holds the accepted 35% amplitude
anchor, harmonic-number decay, and medium hybrid pick constant while rendering
0x, 2x, 4x, and the selected 6x side-mode restoration. The 6x reference is first
and an isolated `6x - 0x` contribution is included.

All full mixes are within 0.1 dB whole-file mean and 0.2 dB per note. The 6x
reference exactly reproduces Plan 0095 at
`D0D4B3E6AFD28CAEAA56CDCF5C27D6CCED6294F7EE7EFEFEDE8C007260F5019F`;
2x and 4x repeat byte-identically. Every track is stereo 48 kHz and 424,800
samples. Adding the isolated 6x contribution to the 0x render reconstructs the
reference with infinite measured audio PSNR. Human listening should select the
lowest amount that retains the accepted character, including zero if decay alone
is sufficient.

Human loudness-matched listening selected 2x restoration as good. The accepted
offline recipe is therefore:

- modal pick excitation at 1.75x force
- 12% direct pick texture at 2.5x event density
- 35% absolute-frequency amplitude/formant anchoring
- harmonic-number modal decay (`--legacy-register-decay-anchor 0.0`)
- 2x fixed-Hz/plain-string-weighted side-mode restoration

The exact selected open-string render is
`ABF261ECD386B652755244D6A63786E6DB4A8899E1948DDF263750EC770AC041`.
These remain offline research controls. Before production promotion, compare the
consolidated recipe against the current VST3-equivalent legacy path over normal
and deep pick gestures, wound/plain registers, and at least one phrase rather
than inferring generality from the six-open-string calibration alone.

## 2026-08-27 — Plan 0097 Production-Tone Promotion Gate

Plan 0097 makes no production DSP change. It compares the current
production-equivalent legacy path against the complete accepted offline recipe
in one 26.9-second aligned program:

- ordinary-pick open strings across E2–E4
- the deep/flexible/textured failure regime across the same register
- a compact wound-string riff, cross-string arpeggio, upper melody, and chord

The candidate uses 1.75x modal force, 12% direct texture at 2.5x density, 35%
amplitude/formant anchoring, harmonic-number decay, and 2x side-mode restoration.
All other renderer settings and MIDI are identical. Candidate level is about
1 dB above current in each of the three sections, allowing the A/B tool's global
loudness match to remain meaningful.

Current plus the signed `candidate - current` track reconstructs the candidate
with infinite measured audio PSNR. All tracks are stereo 48 kHz and 1,291,200
samples. Human listening across all three sections is the gate for a later,
separate production implementation; the VST3 remains unchanged here.

Human loudness-matched listening confirmed that the complete candidate works
across all three Plan 0097 contexts. The recipe has therefore passed its
production-tone gate. Promote it in a separate change, update the build identity
and default-render expectation, and retain an explicit offline previous-tone
recipe for regression. The gate itself did not modify the VST3.

## 2026-08-27 — EG-089 Modal/Formant Tone Production Promotion

EG-089 moves the complete Plan 0097 recipe into the normal `StringVoice` build:

- a short, stiffness-dependent plectrum force updates modal quadrature state at
  1.75x the originally auditioned modal-force scale
- 12% of the existing direct pick texture remains as subordinate material detail,
  with its stochastic event density raised to 2.5x and impulse energy compensated
- modal amplitude/contact envelopes blend 35% toward an E2-referenced
  absolute-frequency index, while modal decay remains harmonic-number based
- only existing inharmonic side modes receive 2x restoration, smoothly gated
  over 650–2500 Hz and reduced on wound strings
- the register compensation used throughout the accepted listening chain is now
  part of production so the promoted VST3 is the exact approved candidate

These are fixed model constants, not new parameters. Harmonic-touch gestures do
not use modal pick excitation, and later slide/fret contact remains independent
of the 12% picked-onset mix.

The offline renderer defaults to the same EG-089 recipe and exposes
`--legacy-tone-recipe previous` for exact EG-088 regressions. The new default
matches the accepted Plan 0097 ordinary, deep, and phrase section files byte for
byte. The previous recipe matches all three corresponding current references and
the canonical single-note hash
`C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`.
The new deterministic single-note hash is
`32AF0A4AE9A04B3E77F74161E963D4AE3E8431433012D642EA13A96C2C5C47D7`.
