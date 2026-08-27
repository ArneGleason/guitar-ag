# Windows Realism Research Handoff — 2026-08-27

## State

- Canonical Windows checkout: `C:\code\github\guitar-ag`
- Starting GitHub commit: `d6fad1a0ddce650a39595a0c46de1a76676fb587`
- Branch: `main`
- Latest published release: `v0.3.1`
- Windows Release `GuitarAG_VST3` and `GuitarAGOfflineRender` builds pass.

## What Changed

No production DSP behavior was changed. Plan 0089 and supporting project memory document a new realism strategy based on current code inspection, project history, recent research, and a local comparison with Guitar-TECHS direct-input notes.

## Key Diagnosis

The current `StringVoice` is not rendering its two delay lines. It renders a modal bank plus separately synthesized pick/contact components and resets state on every note start. That architecture makes tonal chirps, specular finger noise, and discontinuous legato likely even when the parameters are carefully tuned.

Commit `3457200` contains the last project-owned two-polarization delay-loop implementation with finite pickup-aperture reads. Use it as a source for a new experimental engine; do not revert current main.

## Next Task

Implement Plan 0089 Milestone 0 and the smallest part of Milestone 1:

1. Add an offline/development A/B engine seam while keeping the legacy default byte-identical.
2. Create a single-string stateful two-polarization prototype from project-owned history.
3. Replace additive pick chirps with finite compliant contact-force excitation.
4. Add state-preserving `repluck()`.
5. Build and render the four acceptance fixtures listed in the plan, then stop for human listening.

Do not add fret collision, pickup magnetic nonlinearity, new production UI controls, or a full nonlinear SAV core in this first slice.

## External Reference Data

Guitar-TECHS P1 single-note data is local-only under `C:\code\reference-audio\Guitar-TECHS`. Do not commit the archive or extracted WAV files.

## Cleanup Note

The prior clean checkout at `C:\code\guitar-ag` remains because Windows held it open for the originating Codex workspace. Delete it only in a later task after that workspace is closed and after confirming the canonical checkout is clean and pushed.
