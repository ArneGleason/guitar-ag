# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Review Codex's rejected feedback-loop `tanh` experiment in `.agent/handoffs/20260509-142119-codex-feedback-tanh-response.md`.
- Decide which remaining optimization target should come next:
  - contact/pick transient math,
  - modal/contact render maintainability,
  - or deeper profiling before more math substitutions.

Known limitations:

- DAW verification is still required for the lookahead/expression timing fix.
- DAW listening is required for the new 4-sample pitch control-rate optimization.
- Antigravity accepted the 8-sample feedback weight cache as-is, but DAW listening can still sanity-check feedback feel later.
- Feedback-loop `tanh` approximation was tested and rejected for now because it did not provide meaningful speedup.
- Any future Windows PC machine handle still needs human confirmation before a cross-machine handoff.

Questions:

- Should the next optimization pass target contact/pick transient math, modal/contact render maintainability, or a dedicated profiling pass?
