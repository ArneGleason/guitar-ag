# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Review the EG-073 `PlasticPickBite` implementation.
- Confirm the new `pickBite` APVTS parameter is wired from processor to UI to `AudioEngine`, `StringVoice`, and offline `--pick-bite`.
- Confirm `Pick Bite` scales contact imprint terms rather than muting the main string displacement/release.
- Confirm `String Age` now darkens the pick-contact fingerprint without making old strings artificially noisy or dead.
- Confirm the new deterministic pick-depth/pick-angle variation remains repeatable across identical offline renders.
- Confirm the existing `pickStroke` APVTS parameter remains wired from processor to UI to `AudioEngine` and offline `--pick-stroke`.
- Confirm `Pick Stroke = Alternate` advances only on `PlayerGesture::Picked` note starts and does not consume alternation for hammer-ons, pull-offs, or taps.
- Confirm the normal plastic-pick retune remains scoped to picked attack/contact and does not alter slide squeak, MPE, whammy, vibrato, or feedback paths.
- Review `plans/0075-plastic-pick-bite.md` for implementation/test coverage and physical-model framing.

Known limitations:

- The preset model is planned but not implemented.
- EG-071 uses one persisted last-direction value per voice, so overlapping up/down contact tails are direction-approximated rather than separately accumulated.
- EG-073 is still one ordinary plastic plectrum model only; fingerpicking, fingernail attack, material families, and explicit pick-depth UI remain deferred.
- EG-066 is still an approximation of lifted fretting pressure. Same-string speaking-length preservation remains future work.
- DAW verification is still required for the lookahead/expression timing fix.
- DAW listening is required for the new 4-sample pitch control-rate optimization.
- Antigravity accepted the 8-sample feedback weight cache as-is, but DAW listening can still sanity-check feedback feel later.
- Feedback-loop `tanh` approximation was tested and rejected for now because it did not provide meaningful speedup.
- Antigravity accepted the contact-local sine approximation as-is, but manual listening can still sanity-check pick scrape, coin edge, heavy pick rasp, hammer-on/tap impact, and pull-off snap.
- Antigravity accepted EG-061 and recommended ending the code-level optimization cycle.
- EG-061 is behavior-neutral and matched EG-060 audio sample data exactly in local offline renders.
- Initial human listening provisionally accepted sound equivalence and identity, with modest DAW-visible performance improvement.
- Any future Windows PC machine handle still needs human confirmation before a cross-machine handoff.

Questions:

- If performance still feels short of target in a specific DAW scenario, capture host, sample rate, buffer size, amp-sim chain, MIDI clip, and parameter settings before profiling.
- If up/down squeak levels still feel uneven in a DAW, capture `Squeak Up`, `Squeak Down`, `Fret Steps`, `Slide Lift`, slide speed, interval, and direction before changing the contact model.
- If EG-073 still sounds too brittle on isolated single notes, decide whether to reduce remaining chirp modes/contact ring further or build a fuller contact/release envelope before adding pick-material UI.
