# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Verify Codex's immediate fixes from the first Antigravity review:
  - lookahead schedules pitch wheel, mod wheel, channel pressure, CC74, and key/poly aftertouch with delayed note events,
  - MPE mode changes clear per-channel pitch bend, pressure, and CC74 state,
  - normal MIDI channel pressure and CC74 apply globally to all active voices.
- Real-time audio safety: allocations, blocking work, logging, denormal behavior, and UI/DSP coupling in the audio path.
- Performance optimization follow-up for the deferred per-sample `std::pow`/`std::sin`/`std::tanh` finding.

Known limitations:

- DAW verification is still required for the lookahead/expression timing fix.
- The per-sample math performance finding is intentionally not fixed in this immediate bug pass.
- Any future Windows PC machine handle still needs human confirmation before a cross-machine handoff.

Questions:

- Should the next routed step be Antigravity verification of the immediate fixes, or should Codex proceed directly into the performance/maintainability optimization pass?
- Should the eventual optimization pass use block-rate/sub-block-rate modulation updates, fast approximations, or a smaller targeted rewrite?
