# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Verify the `StringVoice EG-061 RenderHelpers` maintainability pass in `.agent/handoffs/20260509-144110-codex-render-helpers.md`.
- Decide which remaining optimization target should come next:
  - profiler-backed investigation,
  - manual DAW listening for the recent expression/contact changes,
  - or pausing the optimization cycle as complete for now.

Known limitations:

- DAW verification is still required for the lookahead/expression timing fix.
- DAW listening is required for the new 4-sample pitch control-rate optimization.
- Antigravity accepted the 8-sample feedback weight cache as-is, but DAW listening can still sanity-check feedback feel later.
- Feedback-loop `tanh` approximation was tested and rejected for now because it did not provide meaningful speedup.
- Antigravity accepted the contact-local sine approximation as-is, but manual listening can still sanity-check pick scrape, coin edge, heavy pick rasp, hammer-on/tap impact, and pull-off snap.
- EG-061 is intended to be behavior-neutral and matched EG-060 audio sample data exactly in local offline renders.
- Any future Windows PC machine handle still needs human confirmation before a cross-machine handoff.

Questions:

- Are the new `renderSample` helper boundaries clear and narrow enough to keep?
- Should the next step be profiler-backed investigation, manual listening, or ending the optimization cycle for now?
