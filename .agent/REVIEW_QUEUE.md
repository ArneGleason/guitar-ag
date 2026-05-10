# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Review the EG-066 `SlideLift` replacement for EG-065 `SlideTail`.
- Confirm the `Slide Tail` choice and note-off release modes are fully removed from live code.
- Confirm `Slide Lift` is neutral at 0%, only responds to recent `Neck Slide` motion, fades fret-step engagement, adds light lifted scrape, and damps sustain without touching MPE/whammy/vibrato pitch paths.
- Confirm the UI info copy clearly explains the control in plain language and technical terms.
- Confirm `GuitarAGOfflineRender` uses `--slide-lift` and no longer uses `--slide-tail`.

Known limitations:

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
- If `Slide Lift` feels too subtle or too aggressive in a DAW, capture lift amount, slide speed, release timing, and target interval before changing the envelope/damping scalars.
- After review/audition, decide whether to tune EG-066 or proceed to same-string speaking-length work.
