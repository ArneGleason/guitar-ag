# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Review the EG-080 `StrumBalance` implementation, including the prior EG-079 `AutoStrum` path it builds on.
- Confirm `Strum Speed = 0%` preserves the previous same-sample MIDI behavior.
- Confirm exact same-sample block chords are detected only when at least two note-ons share the same sample and `Strum Speed` is above zero.
- Confirm the predicted fretboard assignment is preserved through delayed note scheduling, so the scheduled note-on uses the same string that drove the strum order.
- Confirm `Pick Stroke = Down`, `Up`, and `Alternate` drive block-chord order coherently, including repeated block chords in Alternate mode.
- Confirm `Strum Balance = 0` preserves EG-079 generated strum velocities.
- Confirm positive `Strum Balance` reduces generated upstroke velocities only, and negative `Strum Balance` reduces generated downstroke velocities only.
- Confirm the selected stroke direction is reduced, not the opposite direction boosted, so high incoming MIDI velocities cannot clip from this control.
- Confirm `Strum Balance` only affects generated Auto Strum note-ons and does not alter single-note picking or authored staggered MIDI.
- Confirm `processPlayerFeelNoteOn` uses the generated strum delay as the effective event time, so generated strums do not look like zero-time unrelated picking events.
- Confirm note-offs are handled before generated block-chord note-ons, and same-sample expression/controller events fall back to the normal dispatch path instead of being reordered into Auto Strum.
- Confirm the Articulation page exposes `Strum Speed` and `Strum Balance`, and `Player Feel` shows Bot/Pro/Loose landmarks without clipping.
- Confirm `scripts/create-auto-strum-audition-midi.py` creates exact same-time block chord test material.
- Historical EG-078 checks: confirm very short cross-string strum continuations reduce cognitive/dexterity load without disabling feel variation.
- Confirm fast same-string picking and non-strum single-note runs still accumulate load normally.
- Confirm the EG-078 strum-continuation window remains deterministic and bounded.
- Confirm `Player Feel` still has a natural middle range while 100% has a meaningfully wider sloppy timing/energy range than EG-076.
- Confirm the high-end timing delay cap and widened velocity/energy scale are bounded and deterministic.
- Confirm cognitive/dexterity/endurance impulses are reduced enough that meters do not saturate too quickly.
- Confirm `Feel Recovery` defaults to 2.0 seconds and clamps to the new 0.10 to 8.0 second range in APVTS, `AudioEngine`, and the offline renderer.
- Confirm cognitive, dexterity, and endurance meter values are exported from `AudioEngine` to the processor through atomics and read by the editor timer without audio-thread allocation or UI calls.
- Confirm meter decay remains deterministic and visibly clears during rests/easier passages.
- Confirm the `Export Settings` popup returns valid JSON with current parameter values plus Player Feel meters, and that it is copyable in the UI.
- Confirm `Export Settings` is globally available in the header, has enough vertical breathing room, and no longer consumes Articulation-page row space.
- Review the EG-075/EG-076/EG-077 `PlayerFeel` path as the base for EG-078:
  - confirm `Player Feel = 0` leaves the render path neutral;
  - confirm load accumulation/recovery is deterministic and bounded;
  - confirm `Feel Recovery` and `Reset Feel` clear cognitive/dexterity/endurance state as intended;
  - confirm the new timing delay only applies to note-on events and does not desync delayed lookahead expression more than the documented first-pass limitation.
- Historical context for the right-hand model: review the EG-074 `EconomyPickStroke` implementation if it has not already been reviewed.
- Confirm the existing `pickStroke` APVTS parameter remains wired from processor to UI to `AudioEngine` and offline `--pick-stroke`.
- Confirm `Pick Stroke = Alternate` now uses economy direction for cross-string picked movement:
  - low E toward high E / increasing string index resolves to downstroke;
  - high E toward low E / decreasing string index resolves to upstroke;
  - string skips follow the same rule.
- Confirm repeated same-string picked notes still alternate against the previous picked stroke.
- Confirm hammer-ons, pull-offs, and taps do not consume or update pick-stroke memory.
- Confirm forced `Down` and `Up` modes remain exact forced directions.
- Confirm `reset()` and switching into `Alternate` reset the right-hand memory to a deterministic first downstroke.
- Confirm the info note accurately describes the smarter `Alternate` interpretation without adding a new user-facing mode.
- Review `plans/0076-economy-pick-stroke.md` for implementation/test coverage and physical-model framing.
- Optionally use `tests/midi/guitar-ag-pick-stroke-audition.mid` to audition repeated picking, string crossings, string skips, chord strums, and riff-like phrases with `Pick Stroke = Alternate`.
- Review `plans/0079-auto-strum-interpreter.md` for the accepted exact-block implementation and future tolerance-window direction.

Known limitations:

- The preset model is planned but not implemented.
- EG-071 uses one persisted last-direction value per voice, so overlapping up/down contact tails are direction-approximated rather than separately accumulated.
- EG-073 is still one ordinary plastic plectrum model only; fingerpicking, fingernail attack, material families, and explicit pick-depth UI remain deferred.
- EG-079/EG-080 groups exact same-sample block chords only. It does not collect near-simultaneous notes that arrive across adjacent samples or a 1 ms tolerance window.
- EG-075 can only delay picked note starts in real time. Early human timing requires a future lookahead-aware pass.
- The pick-stroke audition MIDI uses original/common exercise idioms rather than copied repertoire; it is intended as a functional listening workout, not a reference performance.
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
- If up/down squeak levels still feel uneven in a DAW, capture `Squeak Up`, `Squeak Down`, `Fret Steps`, `Slide Lift`, slide speed, interval, and direction before changing the contact model.
- If EG-074 feels wrong on dense strums, capture MIDI note ordering, assigned strings, `Pick Stroke`, and articulation settings before changing the heuristic; the next refinement may need explicit strum grouping rather than another per-note rule.
