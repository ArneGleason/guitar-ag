---
description: Send a reusable Studio Pattern lesson from Guitar AG back to the canonical repo
---

Use this when Guitar AG reveals a reusable Studio Pattern lesson.

1. Record the observation locally first.
   - Use `.agent/PROJECT_LOG.md` for durable decisions or events.
   - Use `.agent/REVIEW_QUEUE.md` for open reviewer questions.
   - Use `.agent/handoffs/` when another participant needs context.
2. Ask the human owner to read the summary and add any stakeholder context.
3. Decide what remains local to Guitar AG.
4. Copy this block to the canonical Studio Pattern repo or thread. For feedback to the canonical repo, set `To` to `the-studio-pattern`.

```md
# Cross-Project Feedback

- To: `<destination project or thread>`
- From: `<person, agent, source project, or source thread>`
- To agent: `<agent or tool, if relevant>`
- To machine: `<machine-handle, if relevant>`
- From agent: `<agent or tool, if relevant>`
- From machine: `<machine-handle, if relevant>`
- Created: `<YYYY-MM-DD>`
- Origin: `<source repo, commit, review, handoff, or session>`
- Subject: `<short subject>`

## Context

<why this is crossing project boundaries>

## Local Evidence

- `<verified file, commit, review finding, or workflow observation>`

## Reusable Question

<what the canonical pattern should consider>

## Proposed Canonical Change

<smallest reusable docs, template, or protocol change>

## Boundary Note

<what remains local to the source project>
```
