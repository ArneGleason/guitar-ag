# Physical Guitar

A physical-model electric guitar virtual instrument project.

The goal is to build a VST3 plugin that generates guitar-like sound through physical modeling, with MPE support for independent per-note bends and expression.

## Current Status

Initial JUCE/CMake plugin shell has been added. It is intended to build a VST3 instrument named `Guitar AG` once CMake and JUCE are installed.

## MVP Goal

The first successful MVP should:

- Load as a VST3 plugin in a DAW.
- Play multiple notes at once.
- Generate notes from a physical/string model rather than samples.
- Support MPE per-note pitch bend so one held note can bend while another remains fixed.
- Provide a clean DI-style electric guitar tone suitable for external amp sims.

## Repo Guide

- `AGENTS.md` — primary instructions for AI coding agents.
- `PROJECT.md` — plain-language project brief.
- `docs/requirements.md` — top-level requirements.
- `docs/mpe-behavior.md` — MPE behavior and voice routing.
- `docs/dsp-notes.md` — physical modeling notes and experiments.
- `docs/realism-vision.md` — research-backed long-term realism direction.
- `plans/` — milestone-by-milestone implementation plans.
- `DECISIONS.md` — decision log.
- `LEARNINGS.md` — running project memory.

## Suggested First Build Step

Start with `plans/0001-mvp-plugin-shell.md`.

The first task should only create a minimal plugin shell that builds, loads, and produces a simple test tone. Do not start with the full guitar model immediately.

## Local VST3 Install

After building, install the plugin into the user VST3 folder with:

```bash
scripts/install-vst3.sh
```

To build and install in one step:

```bash
scripts/install-vst3.sh --build
```
