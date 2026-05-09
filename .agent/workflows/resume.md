---
description: Resume work on Guitar AG
---

1. Refresh and inspect the repo.
   ```sh
   git fetch origin --prune
   git status --short --branch
   ```
2. If the branch is behind and there are no local changes, pull with `git pull --ff-only`.
3. Read `AGENTS.md`, `LOCAL_DEV_NOTES.md`, `.agent/session.json`, `.agent/PROJECT_LOG.md`, and `.agent/REVIEW_QUEUE.md`.
4. If `lastHandoff` is listed in `.agent/session.json`, read that handoff.
5. For major changes, also read the project core docs listed in `AGENTS.md`.
6. Run validation when practical.
   ```sh
   cmake --build build --config Release --target GuitarAG_VST3
   ```
7. Continue from the newest user request, preserving existing local changes that are not yours.
