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
