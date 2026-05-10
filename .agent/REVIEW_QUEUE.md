# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Review the EG-071 `SlideSqueakDirection` implementation.
- Confirm the existing `slideSqueak` parameter ID remains the upward amount and the new `slideSqueakDown` parameter is wired end to end.
- Confirm `StringVoice::slideFretDirection` only affects slide contact squeak/scrape amount selection and does not alter pitch, MPE, whammy, `Fret Steps`, or `Slide Lift`.
- Confirm the Performance page shows `Squeak Up` and `Squeak Down` under `Neck Slide` with complete info notes.
- Review `plans/0072-preset-model.md` for architecture sanity, treating it as planning-only.

Known limitations:

- EG-071 uses one persisted last-direction value per voice, so overlapping up/down contact tails are direction-approximated rather than separately accumulated.
- The preset model is planned but not implemented.
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
- Decide whether the next implementation pass should be preset save/load UI or same-string speaking-length work.
