# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Review the slide gesture Phase 2b implementation:
  - `plans/0064-slide-gesture-notation.md`,
  - `scripts/create-slide-gesture-midi.py`,
  - `tests/midi/guitar-ag-slide-gesture-audition.mid`,
  - `src/plugin/PluginProcessor.cpp`,
  - `src/plugin/PluginEditor.cpp`,
  - `src/dsp/AudioEngine.cpp`,
  - `src/dsp/StringVoice.cpp`,
  - `tools/render-calibration.cpp`,
  - `docs/audition-midi.md` slide gesture audition section.
- Check especially whether `Neck Slide` is wired as a behavior-neutral global lane at zero, layers correctly with existing pitch controls, and clamps stacked pitch motion safely.
- Check whether `Fret Steps` is behavior-neutral when `Neck Slide` is static/zero, whether the semitone plateau shaping feels plausible, and whether fret-crossing contact texture is scoped to moving slide gestures.

Known limitations:

- EG-063 is still an approximation of fretted slide behavior. Same-string speaking-length preservation and open/muted slide tails remain future work.
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
- If `Fret Steps` feels too notchy or too smooth in a DAW, capture the host automation lane shape, slide speed, and target intervals before changing the default.
- After review, run a DAW/VST3 import test with the slide audition MIDI and draw the `Neck Slide` automation bed manually with `Fret Steps` at 0%, 65%, and 100%.
