# Plan 0090 — Stateful Waveguide A/B Slice

## Goal

Implement the smallest testable part of Plan 0089 without changing the released plugin's default audio:

- a development/offline engine selector
- one persistent two-polarization waveguide voice per physical string
- finite compliant plectrum contact whose force enters string state
- state-preserving same-string repicks
- deterministic A/B renders and a focused human listening fixture

## Scope Boundary

This slice intentionally does not claim realistic hammer-ons, pull-offs, slides, fret collision, neck admittance, sympathetic coupling, or nonlinear magnetic pickup behaviour. Existing plugin UI/state does not expose the experimental engine.

## Implementation

- [x] Add `StatefulStringVoice` with fixed-size delay storage and no audio-thread allocation.
- [x] Add two slightly detuned/coupled polarization loops with fractional-delay pitch control.
- [x] Add frequency-dependent loop loss, finite pickup aperture, pickup temporal filtering, and clean DI readout.
- [x] Add a deterministic compliant pick trajectory with a nonlinear depth/release mapping.
- [x] Feed pick force only into string state; do not mix a separate click, chirp oscillator, or contact-noise bus.
- [x] Preserve delay-line state when the assigned physical string is repicked.
- [x] Add offline `--string-engine legacy|stateful` selection.
- [x] Add offline `--stateful-repick 0|1` to isolate the audible contribution of preserved residual state.
- [x] Keep the plugin on `LegacyModal`; do not add a persisted host parameter yet.

## Verification

- [x] Windows Release `GuitarAGOfflineRender` builds.
- [x] Windows Release `GuitarAG_VST3` builds after final code changes.
- [x] Rendering without an engine flag remains byte-identical to the pre-slice baseline.
- [x] Stateful renders are deterministic.
- [x] Stateful offline performance has ample headroom.
- [x] Preserved-state and reset-on-note repeated-note renders differ while all other inputs remain equal.
- [x] Run calibration/reference diagnostics and record their limits without optimizing blindly for them.
- [ ] Human listens to isolated wound/plain attacks and repeated F2 repicks at matched level.

## Focused Fixture

`tests/midi/guitar-ag-stateful-waveguide-audition.mid` contains:

1. isolated open low E2
2. isolated open high E4
3. eight F2 notes forced onto the low-E string
4. an E2-to-G2 same-string hammer-on candidate

The first three sections are the acceptance gate. The fourth deliberately previews the next missing capability and should not block this slice.

## Acceptance

Keep the slice only if human listening finds that:

- isolated attacks have less separate tonal chirp/specular contact noise than the legacy engine
- the note body still reads as an electric-guitar DI source rather than a generic muted delay
- repeated repicks feel continuous without obvious runaway buildup or clicks
- the stateful engine is promising enough to justify the persistent left-hand-state milestone

If the body or decay is not yet convincing, keep the A/B harness and prototype branch, record the listening result, and refine the string/pickup mechanics before exposing the engine in the VST3.
