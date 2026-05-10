# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Review the EG-072 `PickStrokePlectrum` implementation.
- Confirm the new `pickStroke` APVTS parameter is wired from processor to UI to `AudioEngine` and offline `--pick-stroke`.
- Confirm `Pick Stroke = Alternate` advances only on `PlayerGesture::Picked` note starts and does not consume alternation for hammer-ons, pull-offs, or taps.
- Confirm the attack seed is deterministic, reset with engine reset, and includes enough per-attack information to avoid cloned same-note transients.
- Confirm the normal plastic-pick retune is scoped to picked attack/contact and does not alter slide squeak, MPE, whammy, vibrato, or feedback paths.
- Review `plans/0074-pick-stroke-plectrum.md` for implementation/test coverage.

Known limitations:

- The preset model is planned but not implemented.
- EG-071 uses one persisted last-direction value per voice, so overlapping up/down contact tails are direction-approximated rather than separately accumulated.
- EG-072 is one ordinary plastic plectrum model only; fingerpicking, fingernail attack, material families, and pick-depth UI remain deferred.
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
- If EG-072 still sounds too tonal on isolated single notes, decide whether to target the remaining chirp modes, contact ring, or a fuller contact/release envelope before adding pick-material UI.
