# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Verify the `StringVoice EG-059 FeedbackWeightCache` optimization:
  - feedback harmonic/loop lock calculations are cached at an 8-sample control interval,
  - per-sample feedback gates, modal decay, and feedback injection remain intact,
  - high-feedback bloom, string focus, and harmonic takeover still feel natural enough to keep.
- Decide which remaining optimization target should come next: contact/pick transient math, feedback `tanh` approximation, or modal-render maintainability.

Known limitations:

- DAW verification is still required for the lookahead/expression timing fix.
- DAW listening is required for the new 4-sample pitch control-rate optimization.
- DAW listening is required for the new 8-sample feedback weight cache.
- Feedback-loop `tanh` approximation remains a future optimization candidate.
- Any future Windows PC machine handle still needs human confirmation before a cross-machine handoff.

Questions:

- Should Antigravity accept the 8-sample feedback weight cache, ask Codex to tune it to 4 samples, or request human listening before acceptance?
- Should the next optimization pass target contact/pick transient math, feedback `tanh` approximation, or modal-render maintainability?
