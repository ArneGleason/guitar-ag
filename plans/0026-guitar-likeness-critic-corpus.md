# Plan 0026 — Guitar-Likeness Critic Corpus

## Goal

Create the smallest useful data scaffold for a future guitar-likeness critic.

The goal is not to train a model yet. The goal is to preserve real DI references and human listening judgments in a machine-readable manifest so future analysis tools can be checked against the actual project target.

## Scope

- Add a repo-local corpus manifest that points to external audio files.
- Include at least one positive real DI reference.
- Include the current preferred Guitar AG render.
- Include several rejected Guitar AG renders.
- Add a lightweight summary/validation script.
- Document why pretrained `electric guitar` scores should not be trusted blindly.

## Out of Scope

- No ML dependency.
- No downloaded pretrained model.
- No classifier training.
- No DSP changes.
- No audio files committed to Git.

## Implementation Notes

The manifest lives at:

```text
analysis/guitar-likeness-corpus.json
```

The summary script is:

```text
scripts/summarize-guitar-likeness-corpus.py
```

The script uses only Python standard-library modules so it can run before any ML environment exists.

## Verification

- [x] Manifest parses as JSON.
- [x] Summary script runs.
- [x] Summary script validates all local external audio paths with `--strict`.
- [x] Summary shows positive, preferred, and rejected buckets.

## Completion Notes

2026-04-25:

- Added `docs/guitar-likeness-critic.md`.
- Added `analysis/guitar-likeness-corpus.json`.
- Added `scripts/summarize-guitar-likeness-corpus.py`.
- Seeded corpus with Guitar-TECHS P1 DI, KS017 ModalCluster, and rejected KS016/KS015/KS013/KS003 renders.
