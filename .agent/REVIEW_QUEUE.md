# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Review the EG-074 `EconomyPickStroke` implementation.
- Confirm the existing `pickStroke` APVTS parameter remains wired from processor to UI to `AudioEngine` and offline `--pick-stroke`.
- Confirm `Pick Stroke = Alternate` now uses economy direction for cross-string picked movement:
  - low E toward high E / increasing string index resolves to downstroke;
  - high E toward low E / decreasing string index resolves to upstroke;
  - string skips follow the same rule.
- Confirm repeated same-string picked notes still alternate against the previous picked stroke.
- Confirm hammer-ons, pull-offs, and taps do not consume or update pick-stroke memory.
- Confirm forced `Down` and `Up` modes remain exact forced directions.
- Confirm `reset()` and switching into `Alternate` reset the right-hand memory to a deterministic first downstroke.
- Confirm the info note accurately describes the smarter `Alternate` interpretation without adding a new user-facing mode.
- Review `plans/0076-economy-pick-stroke.md` for implementation/test coverage and physical-model framing.
- Optionally use `tests/midi/guitar-ag-pick-stroke-audition.mid` to audition repeated picking, string crossings, string skips, chord strums, and riff-like phrases with `Pick Stroke = Alternate`.
- Review the EG-075 `PlayerFeel` pass:
  - confirm `Player Feel = 0` leaves the render path neutral;
  - confirm load accumulation/recovery is deterministic and bounded;
  - confirm `Feel Recovery` and `Reset Feel` clear cognitive/dexterity/endurance state as intended;
  - confirm the new timing delay only applies to note-on events and does not desync delayed lookahead expression more than the documented first-pass limitation.

Known limitations:

- The preset model is planned but not implemented.
- EG-071 uses one persisted last-direction value per voice, so overlapping up/down contact tails are direction-approximated rather than separately accumulated.
- EG-073 is still one ordinary plastic plectrum model only; fingerpicking, fingernail attack, material families, and explicit pick-depth UI remain deferred.
- EG-074 does not group near-simultaneous chord notes into a single strum gesture yet; it uses per-note string travel order as received from MIDI/fretboard assignment.
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
- Any future Windows PC machine handle still needs human confirmation before a cross-machine handoff.

Questions:

- If performance still feels short of target in a specific DAW scenario, capture host, sample rate, buffer size, amp-sim chain, MIDI clip, and parameter settings before profiling.
- If up/down squeak levels still feel uneven in a DAW, capture `Squeak Up`, `Squeak Down`, `Fret Steps`, `Slide Lift`, slide speed, interval, and direction before changing the contact model.
- If EG-074 feels wrong on dense strums, capture MIDI note ordering, assigned strings, `Pick Stroke`, and articulation settings before changing the heuristic; the next refinement may need explicit strum grouping rather than another per-note rule.
