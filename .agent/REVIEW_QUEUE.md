# Review Queue

Use this file to give reviewers a focused starting point. Add known risks, open questions, weak assumptions, and areas where fresh eyes are useful.

## Next Review

Suggested focus:

- Baseline code review of the current VST3/DSP implementation before the next optimization pass.
- Real-time audio safety: allocations, blocking work, logging, denormal behavior, and UI/DSP coupling in the audio path.
- MPE and MIDI correctness: channel-to-voice ownership, per-note pitch bend, pressure, CC74, and normal MIDI fallback.
- Six-string voice allocation and repeated-note replacement behavior.
- Performance and maintainability opportunities that preserve the current sound and MPE behavior.
- Studio Pattern adoption surfaces: whether `AGENTS.md`, `LOCAL_DEV_NOTES.md`, `.agent/session.json`, and handoff instructions give a reviewer enough context.

Known limitations:

- This adoption pass did not perform a code review or run a build.
- Machine handles for the Mac and Windows environments are not assigned yet.
- No first reviewer handoff has been created yet; create one once the human chooses the first review target and reviewer environment.

Questions:

- What stable machine handles should we use for the Mac and Windows PC?
- Which reviewer agent/tool and machine should receive the first review?
- Should the first review be a full current-code baseline review, or should it focus on performance and maintainability risks first?
