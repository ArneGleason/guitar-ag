---
description: Suspend or hand off the current Guitar AG task
---

1. Run validation when practical.
   ```sh
   cmake --build build --config Release --target GuitarAG_VST3
   ```
2. Check repo state.
   ```sh
   git status --short --branch
   git rev-parse HEAD
   ```
3. Update `LOCAL_DEV_NOTES.md` with any reusable command, setup detail, or gotcha discovered this session.
4. Update `.agent/PROJECT_LOG.md` with meaningful decisions, implementation events, or workflow changes.
5. Update `.agent/REVIEW_QUEUE.md` with known risks, missing checks, and reviewer questions.
6. Update `.agent/session.json` with latest task, status, branch, commit, timestamp, participants, and handoff path if any.
7. Create a concise handoff under `.agent/handoffs/` when another participant, reviewer, machine, or future session needs context.
8. Prefer committing and pushing meaningful work so another machine or reviewer can pull the exact state from GitHub.
9. Tell the human owner the latest state, checks run, checks not run, and next recommended action.
