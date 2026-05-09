# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Review the slide gesture Phase 1 assets:
  - `plans/0064-slide-gesture-notation.md`,
  - `scripts/create-slide-gesture-midi.py`,
  - `tests/midi/guitar-ag-slide-gesture-audition.mid`,
  - `docs/audition-midi.md` slide gesture audition section.
- Check especially whether the generated sections exercise the right current-capability baseline before the global `Slide Offset` Phase 2 implementation.

Known limitations:

- Global `Slide Offset` playback is not implemented yet; the chord-slide section currently duplicates MPE pitch-bend curves on each held chord tone as a proxy.
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
- For slides, should the first user-facing lane be named `Slide Offset`, `Fret Slide`, or `Neck Slide`?
- Should the first range be `-48..+48 st` to match the MPE default, or narrower for guitar-like use?
- Should Phase 2 add a generated host-automation test path, or should the first `Slide Offset` checks stay manual inside the DAW?
