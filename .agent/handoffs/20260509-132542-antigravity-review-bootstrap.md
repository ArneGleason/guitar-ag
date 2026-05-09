# Studio Handoff: Antigravity Baseline Code Review Bootstrap

- Timestamp: 2026-05-09T13:25:42-04:00
- Status: handoff
- Branch: `main`
- Source commit before this handoff update: `80e02ddf93c9406add0447be3cae7ecaa040df71`
- Studio Pattern: 2026-05-09 initial canonical scaffold from `5624928215232b7b82702929a7b9f8f11f19fd5d`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Created: 2026-05-09
- Origin: `guitar-ag` Studio Pattern adoption session
- Subject: Bootstrap Antigravity on Mac Mini from GitHub before baseline code review
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

The human will manually create a local folder with the same repository name on `mac-mini-pro-m4`. Before doing any review, Antigravity should make that folder a fresh checkout of the GitHub repository, or update the existing checkout if the repo is already there.

The requested next step is a baseline code review of the current Guitar AG implementation before any performance or maintainability optimization work.

## First Action On Mac Mini

If the human-created `guitar-ag` folder is empty, run this from inside that folder:

```sh
git clone https://github.com/ArneGleason/guitar-ag.git .
git switch main
git pull --ff-only
git status --short --branch
```

If the folder is already a Git checkout, run this from inside it:

```sh
git fetch origin --prune
git switch main
git pull --ff-only
git status --short --branch
```

Expected status after bootstrap:

```txt
## main...origin/main
```

If the folder is not empty and is not a Git checkout, do not delete local files without human confirmation. Either move stray files aside with the human's approval or clone into the parent directory instead.

## Reviewer Starting Points

Read these first:

- `AGENTS.md`
- `LOCAL_DEV_NOTES.md`
- `.agent/session.json`
- `.agent/PROJECT_LOG.md`
- `.agent/REVIEW_QUEUE.md`
- `PROJECT.md`
- `docs/requirements.md`
- `docs/architecture.md`
- `docs/mpe-behavior.md`
- `docs/dsp-notes.md`
- `docs/test-plan.md`

Then inspect the implementation:

- `src/dsp/`
- `src/plugin/`
- `tools/render-calibration.cpp`
- `CMakeLists.txt`

## Review Request

Please perform a review-only pass. Do not implement fixes in this first reviewer cycle unless the human explicitly redirects.

Focus on:

- real-time audio safety,
- DSP and UI boundary clarity,
- MPE/channel-to-voice routing correctness,
- six-string voice allocation behavior,
- maintainability risks,
- performance risks,
- missing tests or manual verification gaps,
- any Studio Pattern memory surface that would confuse a future lead or reviewer.

Use concrete findings with severity:

```md
## Findings

- [P1] <brief title>
  - File: <path>:<line>
  - Problem: <what can go wrong>
  - Suggested fix: <smallest useful fix>
```

If no issue is found in an area, say that plainly and mention remaining test gaps.

## Checks To Run If Practical

If JUCE and CMake are available on `mac-mini-pro-m4`:

```sh
cmake -S . -B build -DJUCE_PATH=/Users/arnegleason/code/JUCE -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target GuitarAG_VST3
```

For DSP-sensitive findings, also consider:

```sh
cmake --build build --config Release --target GuitarAGOfflineRender
```

If the dependencies are not installed, do not block the review. Record checks not run and why.

## Current Repo State From Lead

Before writing this handoff, Codex on `macbook-pro-m5` confirmed:

```txt
## main...origin/main
```

The handoff itself should be committed and pushed after creation so Antigravity can pull it from GitHub.

## Open Questions

- Which review findings should become immediate fixes?
- Which review findings should feed the planned performance and maintainability optimization pass?
- Should a reusable Studio Pattern lesson be sent back to `the-studio-pattern` after this first cross-machine review cycle?
