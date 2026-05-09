# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Verify the `StringVoice EG-058 PitchControlRate` optimization:
  - pitch ratios and adjusted modal phase steps are cached at a 4-sample control interval,
  - neutral pitch/no-modulation renders remain byte-identical,
  - MPE bends, whammy, vibrato, and aftertouch bend remain smooth enough for the instrument's core use case.
- Decide which remaining optimization target should come next: feedback-loop math, contact/pick transient math, or modal-render maintainability.

Known limitations:

- DAW verification is still required for the lookahead/expression timing fix.
- DAW listening is required for the new 4-sample pitch control-rate optimization.
- Feedback-loop `tanh` and scalar math remains a future optimization candidate.
- Any future Windows PC machine handle still needs human confirmation before a cross-machine handoff.

Questions:

- Should Antigravity accept the 4-sample pitch control interval or ask Codex to tune/revert it?
- Should the next optimization pass target feedback-loop math, contact/pick transient math, or modal-render maintainability?
