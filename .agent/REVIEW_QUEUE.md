# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- No code review is currently queued. Antigravity approved EG-065 `SlideTail` at commit `350f6e7`.
- Next step is human DAW audition of `Slide Tail` modes using the installed EG-065 VST3.
- After audition, record whether `Muted`, `Open`, and `Slide Off` feel musically useful as-is or need scalar/gating changes.

Known limitations:

- EG-065 is still an approximation of fretted slide behavior. Same-string speaking-length preservation remains future work.
- `Open` slide tail is a ringing release color only; it does not retune the model to a true open string yet.
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
- If `Slide Tail` modes feel too subtle or too dramatic in a DAW, capture tail mode, slide speed, release timing, and target interval before changing the mode scalars.
- After human audition, decide whether to tune the EG-065 tail scalars or proceed to same-string speaking-length work.
