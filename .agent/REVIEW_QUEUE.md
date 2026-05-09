# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Human DAW/VST3 listening for the accepted optimization cycle:
  - follow `.agent/handoffs/20260509-162331-codex-human-daw-listening.md`,
  - use Bitwig or Live,
  - report host, sample rate, buffer size, amp-sim chain, listening acceptance, audible artifacts, and any CPU regressions.

Known limitations:

- DAW verification is still required for the lookahead/expression timing fix.
- DAW listening is required for the new 4-sample pitch control-rate optimization.
- Antigravity accepted the 8-sample feedback weight cache as-is, but DAW listening can still sanity-check feedback feel later.
- Feedback-loop `tanh` approximation was tested and rejected for now because it did not provide meaningful speedup.
- Antigravity accepted the contact-local sine approximation as-is, but manual listening can still sanity-check pick scrape, coin edge, heavy pick rasp, hammer-on/tap impact, and pull-off snap.
- Antigravity accepted EG-061 and recommended ending the code-level optimization cycle.
- EG-061 is behavior-neutral and matched EG-060 audio sample data exactly in local offline renders.
- Any future Windows PC machine handle still needs human confirmation before a cross-machine handoff.

Questions:

- Does manual DAW listening accept the EG-058 through EG-061 optimization cycle?
- Are there any audible artifacts or DAW CPU regressions that justify profiler-backed follow-up?
