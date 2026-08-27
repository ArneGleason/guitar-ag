# Plan 0090 Stateful Waveguide Listening Handoff — 2026-08-27

## State

- Branch: `codex/0090-stateful-waveguide-ab`
- Canonical Windows checkout: `C:\code\github\guitar-ag`
- Production VST3 engine: unchanged legacy modal engine
- Experimental engine: offline renderer only

## Implemented

- Two persistent, slightly detuned/coupled polarization delay loops per physical string.
- Fractional-delay pitch movement for MPE/whammy/vibrato/slide input.
- Frequency-dependent loss, finite pickup aperture, temporal pickup filtering, and DC blocking.
- Deterministic finite-compliance pick trajectory with nonlinear depth and release threshold.
- Pick force enters string state only; there is no separate chirp, ring, or contact-noise output.
- Same-string repicks preserve residual delay-line state by default.
- Offline selectors: `--string-engine legacy|stateful` and `--stateful-repick 0|1`.

## Protected Boundary

`GUITAR_AG_ENABLE_STATEFUL_ENGINE` is defined only for `GuitarAGOfflineRender`. The VST3 does not compile or instantiate `StatefulStringVoice`; no plugin parameter/state change was made.

## Verified

- Windows Release VST3 and offline renderer build.
- Legacy calibration SHA-256 remains `C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`.
- Repeated stateful renders are byte-identical.
- Preserve/reset renders match before the F2 repick section and diverge during it.
- 44.1 and 48 kHz renders complete without instability.
- Six-voice stateful Auto Strum render: 33.0x real time, 3.03 ms maximum block, -2.4 dBFS peak.
- Guitar-TECHS diagnostic spectral distances moved materially toward the selected references, but spectral flatness and flux remain warnings rather than proof of realism.

## Listening Files

- `build-vs2022-x64/diagnostics/eg090-legacy.wav`
- `build-vs2022-x64/diagnostics/eg090-stateful-preserve.wav`
- `build-vs2022-x64/diagnostics/eg090-stateful-reset.wav`

The legacy and stateful peaks are within 0.6 dB. The stateful whole-file mean is about 9 dB lower, so its less-dense body should be judged directly rather than hidden with normalization.

## Listening Questions

1. Do isolated E2 and E4 attacks lose the separate tonal chirp/specular-noise character?
2. Does the quieter/less-dense body still read as a useful electric-guitar DI?
3. Do preserved F2 repicks feel more continuous than reset repicks without smearing or runaway buildup?
4. Is the result promising enough to build a stateful experimental VST3 variant, or should string/pickup mechanics be refined offline first?

The final E2-to-G2 section is not an acceptance gate; the physical left-hand transition is the next milestone.

## Cleanup Blocker

`C:\code\guitar-ag` is verified redundant but remains the active workspace root for the originating task. The filesystem guard rejected its recursive deletion. Open a fresh task rooted at `C:\code\github\guitar-ag`, recheck both paths, then remove the old checkout.
