# 0062 — Feedback String Focus

Date: 2026-05-03

## Goal

Restore the musical runaway/takeover quality of `Amp Feedback` while preserving the six-string cap and passive CPU optimizations.

## Context

Listening feedback after EG-054 said the high feedback range again sounded too even, as if all active strings were being excited together. The global dominant-band loop still existed, but the voice-side response did not explicitly choose a physical string winner.

## Plan

1. Keep the existing global resonator band loop.
2. Add a cheap per-voice coupling score against the current loop frequency.
3. Track a dominant physical string in `AudioEngine`.
4. Boost loop return into the focused string and reduce loop/local feedback on other strings.
5. Add an optional clipped feedback return so the internal feedback source can behave more like a distorted amp signal without distorting the main DI output.
6. Update UI, docs, offline render flags, and test notes.

## Result

- The model label is `StringVoice EG-055 FeedbackFocus`.
- `Amp Feedback` now has a string-focus stage above the dominant-band loop.
- `Distorted Return` toggles extra clipping only in the feedback return path.
- The offline renderer accepts `--feedback-return-distorted`.

## Manual Verification

- Hold a chord with `Amp Feedback` around 80-100%.
- Confirm one string/harmonic starts to dominate instead of all strings lifting evenly.
- Try `Distorted Return` Off and On.
- Confirm the On state makes feedback more assertive, while the main output remains a clean DI source for external amp sims.
