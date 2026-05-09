# Studio Handoff: Codex Slide Gesture Research And Plan

- Timestamp: 2026-05-09T17:05:47-04:00
- Status: handoff
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review slide gesture notation and playback plan before implementation
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Human Context

The human wants Guitar AG to make slides easy to simulate and notate using held notes plus automation curves. The target includes single-note slides, chord-shape slides, dramatic up-and-back throws, and slides away into muted or open tails.

This pass is research and planning only. No DSP or UI code has been changed yet.

## Pull Instructions

Before reviewing, pull the latest `main` from GitHub into the local `guitar-ag` checkout:

```bash
git fetch origin
git switch main
git pull --ff-only origin main
```

## Files To Review

- `plans/0064-slide-gesture-notation.md`
- `docs/mpe-behavior.md`
- `docs/audition-midi.md`
- `docs/architecture.md`
- `LEARNINGS.md`
- `CHANGELOG.md`

## Planning Summary

The proposed umbrella term is **Slide Gesture**.

The plan separates authoring into two lanes:

- Single-note slides: use existing MPE per-note pitch bend curves on the held note.
- Chord-shape slides: add a future global automatable `Slide Offset` lane so one curve can move all active eligible voices together.

The plan intentionally avoids mapping MPE CC74/timbre to guitar slide by default. Some DAWs call the MPE Y dimension "Slide", but Guitar AG currently treats CC74 as timbre, and guitar slides should be pitch-motion plus physical contact/noise behavior.

The proposed phases are:

1. Keep the plan and add audition assets once a control exists.
2. Implement a behavior-neutral global `Slide Offset` playback MVP.
3. Add physical same-string slide behavior that preserves string energy while changing speaking length.
4. Add chord-shape interpretation.
5. Later map MusicXML/tab `slide` and `glissando` markings into the gesture model.

## Review Questions

- Is the MPE pitch bend plus global `Slide Offset` split the right first authoring model?
- Should the first exposed lane be named `Slide Offset`, `Fret Slide`, or `Neck Slide`?
- Should the first range be `-48..+48 st` to match the current MPE range, or narrower for guitar-like use?
- Should slide-out initially be muted-only, or should an open/muted tail mode exist from the first implementation?
- Are there any implementation risks in layering global slide offset with MPE pitch bend, whammy, vibrato, and aftertouch bend?

## Checks Run

- Verified `main` was in sync with `origin/main` before editing.
- Reviewed the existing architecture notes, MPE behavior, audition MIDI docs, player articulation plan, and current pitch-expression code touchpoints.
- Researched slide/glissando terminology and MusicXML/DAW notation behavior.

## Checks Not Run

- No build was run because this is a documentation/planning pass only.
- No DAW/VST3 test was run.

## Suggested Next Routing

Please review the plan for correctness and usability. If accepted, route back to Codex to implement the first playback MVP around a global `Slide Offset` lane and a generated slide audition workflow.
