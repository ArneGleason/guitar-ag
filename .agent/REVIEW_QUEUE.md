# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- No code-review item is currently queued.
- Await the human Bitwig audition of EG-081.
- If the human signs off, the next likely task is the release/version bump decision for v3.0.

Recently cleared:

- Antigravity review at `2026-05-10T17:45:00-04:00` cleared EG-081 `ArticGroups` at commit `6ed57ff` with no required fixes.
- The EG-081 review specifically passed Articulation grouping/readability, release-candidate defaults, session/export behavior, and `Strum Balance` string conversion.
- Antigravity broad review at `2026-05-10T16:55:00-04:00` cleared EG-066 through EG-079 at commit `585049a` with no required fixes.
- The review specifically passed audio-thread safety, scheduling correctness, deterministic Player Feel rendering, parameter wiring, offline flag coverage, UI grouping, changelog/decision/learning hygiene, and plan focus.
- Reviewer cadence recommendation: ask for a reviewer handoff every third major feature or whenever DSP core math changes fundamentally.

Known limitations:

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
- Any future Windows PC machine handle still needs human confirmation before a cross-machine handoff.

Questions:

- If performance still feels short of target in a specific DAW scenario, capture host, sample rate, buffer size, amp-sim chain, MIDI clip, and parameter settings before profiling.
- If up/down squeak levels still feel uneven in a DAW, capture `Squeak Up`, `Squeak Down`, `Fret Steps`, `Slide Lift`, slide speed, interval, and direction before changing the contact model.
- If EG-074 feels wrong on dense strums, capture MIDI note ordering, assigned strings, `Pick Stroke`, and articulation settings before changing the heuristic; the next refinement may need explicit strum grouping rather than another per-note rule.
