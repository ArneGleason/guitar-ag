# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Plan 0094 is awaiting listening. Judge whether 35/65/100% anchoring restores physical scale to B3/E4 without making them dull, hollow, or unnaturally fundamental-heavy; the low E2 tone is effectively unchanged before A/B matching.
- Treat Plan 0094's register gain as audition normalization only. A positive timbre verdict still requires a principled string/gauge energy model before production promotion.
- Plan 0093 listening calibrated 12% dense direct texture as medium and 22% as maximum. Preserve these bounds; use 12% as the fixed attack baseline for register/formant review.
- Plan 0092 listening selected the 1.75x modal-force direction as a useful foundation. Review the next hybrid for whether low-level denser direct texture adds material detail without restoring the sparse woody digital rattle.
- Keep register/formant correction separate: inspect which harmonic-index envelopes should instead use absolute modal frequency so high notes retain instrument scale without becoming merely darker.
- Review the Plan 0092 quadrature update as an offline hypothesis test only: current modal amplitudes already combine string excitation and pickup observation, so promotion would require a clearer state/output separation if listening is positive.
- Plan 0091 shows the next useful work is on the accepted legacy engine: replace the rejected deep-pick additive extras with modal-string excitation, fix register-specific modal identity, and replace the comb-like finger-noise overlay with motion-driven friction.
- The deep/flexible/textured A/B attributed the woody digital rattle to `renderPickTransient()` / `renderContactLayer()` rather than the short chirp-mode bank. Review the next prototype for energy entering through modal state instead of an independent audible layer.
- Review whether the modal recipe scales too uniformly with pitch and whether plain/high strings need different modal density, decay irregularity, phase/excitation, and pickup behavior rather than a brightness-only correction.
- Finger-noise review should reject free-running periodic ridge oscillation; require movement speed and finger depth/position to cause hiss, transverse motion, and any restrained pitched squeak.
- Plan 0090 failed its human timbre gate: both stateful preserve/reset variants sounded like synths. Do not promote the engine or proceed to left-hand-state work in its current form.
- The next focused review should challenge the stateful excitation, dispersion/loss, pickup readout, and body-response assumptions against isolated electric-guitar DI identity—not optimize repick behavior or spectral-distance metrics yet.
- Preserve the offline selector/A-B fixture and the compile-time boundary that excludes stateful storage from the VST3 while exploring the smallest next refinement.

Recently cleared:

- Antigravity review at `2026-05-10T17:45:00-04:00` cleared EG-081 `ArticGroups` at commit `6ed57ff` with no required fixes.
- The EG-081 review specifically passed Articulation grouping/readability, release-candidate defaults, session/export behavior, and `Strum Balance` string conversion.
- Antigravity broad review at `2026-05-10T16:55:00-04:00` cleared EG-066 through EG-079 at commit `585049a` with no required fixes.
- The review specifically passed audio-thread safety, scheduling correctness, deterministic Player Feel rendering, parameter wiring, offline flag coverage, UI grouping, changelog/decision/learning hygiene, and plan focus.
- Reviewer cadence recommendation: ask for a reviewer handoff every third major feature or whenever DSP core math changes fundamentally.

Known limitations:

- The current global finger-noise voice is an independent filtered-noise/stick-impulse/multi-sine generator mixed over the strings and was rejected in isolated listening as plastic-comb-like.
- The high E in the Plan 0091 fixture sounded like a lower-register guitar model pitch-shifted upward, indicating a register-identity problem not solved by high-frequency roll-off.
- Human A/B rejected the current stateful note timbre even with loudness matching; preserve/reset differences are secondary until the isolated note stops reading as a synth.
- Plan 0090 is offline-only. The VST3 cannot select the stateful engine.
- The stateful engine preserves same-string vibration but does not yet model a moving fret/finger boundary; hammer-ons, pull-offs, taps, slides, harmonic touch, and finger contact remain provisional or absent.
- The stateful note body is about 9 dB less dense than legacy in the focused peak-matched whole-file comparison.
- Stateful amp-feedback focus and assignment `stolenVoice` diagnostics still reflect legacy-engine assumptions.
- The current `StringVoice` delay-line buffers are initialized but unused during rendering; the audible core is modal plus explicit contact layers.
- All current note starts reset voice state, so inferred legato, repicks, and fret changes cannot preserve physical string vibration.
- The preset model is planned but not implemented.
- EG-071 uses one persisted last-direction value per voice, so overlapping up/down contact tails are direction-approximated rather than separately accumulated.
- EG-073 is still one ordinary plastic plectrum model only; fingerpicking, fingernail attack, material families, and explicit pick-depth UI remain deferred.
- EG-079/EG-080/EG-081 groups exact same-sample block chords only. It does not collect near-simultaneous notes that arrive across adjacent samples or a 1 ms tolerance window.
- EG-075 can only delay picked note starts in real time. Early human timing requires a future lookahead-aware pass.
- The pick-stroke audition MIDI uses original/common exercise idioms rather than copied repertoire; it is intended as a functional listening workout, not a reference performance.
- EG-066 is still an approximation of lifted fretting pressure. Same-string speaking-length preservation remains future work.
- DAW verification is still required for the lookahead/expression timing fix.
- DAW listening is required for the new 4-sample pitch control-rate optimization.
- Antigravity accepted the 8-sample feedback weight cache as-is, but DAW listening can still sanity-check feedback feel later.
- Feedback-loop `tanh` approximation was tested and rejected for now because it did not provide meaningful speedup.
- Antigravity accepted the contact-local sine approximation as-is, but manual listening can still sanity-check pick scrape, coin edge, heavy pick rasp, hammer-on/tap impact, and pull-off snap.
- Antigravity accepted EG-061 and recommended ending the code-level optimization cycle.
- EG-061 is behavior-neutral and matched EG-060 audio sample data exactly in local offline renders.
- Initial human listening provisionally accepted sound equivalence and identity, with modest DAW-visible performance improvement.
- The human has confirmed a Windows PC will be used for this Plan 0082 build/deployment pass, but a canonical machine handle still has not been assigned. The PC handoff intentionally says not to infer the handle from the OS hostname.

Questions:

- If performance still feels short of target in a specific DAW scenario, capture host, sample rate, buffer size, amp-sim chain, MIDI clip, and parameter settings before profiling.
- If up/down squeak levels still feel uneven in a DAW, capture `Squeak Up`, `Squeak Down`, `Fret Steps`, `Slide Lift`, slide speed, interval, and direction before changing the contact model.
- If EG-074 feels wrong on dense strums, capture MIDI note ordering, assigned strings, `Pick Stroke`, and articulation settings before changing the heuristic; the next refinement may need explicit strum grouping rather than another per-note rule.
