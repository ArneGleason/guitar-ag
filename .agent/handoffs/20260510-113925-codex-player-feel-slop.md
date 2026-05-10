# Studio Handoff: Codex Player Feel Slop Calibration

- Timestamp: 2026-05-10T11:39:25-04:00
- Status: handoff
- Branch: `main`
- To: `guitar-ag reviewer`
- From: `guitar-ag lead`
- Subject: Review EG-077 Player Feel slop calibration and header export
- To agent: `Antigravity`
- To machine: `mac-mini-pro-m4`
- From agent: `Codex`
- From machine: `macbook-pro-m5`

## Pull Instructions

Please run:

```bash
git fetch origin
git switch main
git pull --ff-only origin main
```

Then review the latest pushed `main` commit for EG-077.

## Human Context

Human DAW audition of EG-076 found the load gauges useful, but cognitive/dexterity/endurance accumulated to max too quickly. `Player Feel = 100%` still sounded natural rather than clearly sloppy. The human also liked the settings export, but noted it is a global plugin-state action and should live in the header/main area rather than inside Articulation.

The human provided an EG-076 JSON export for a picked exercise that sounded decent. It is stored at `docs/audition-settings/20260510-eg076-pick-tone-reference.json` as a future tone/preset reference, not as a committed default.

## Implementation Summary

- Changed model label to `StringVoice EG-077 PlayerFeelSlop`.
- Reduced cognitive and dexterity load impulses so meters have more headroom before saturating.
- Reduced endurance accumulation from immediate load impulses.
- Kept the natural middle feel range, but added a nonlinear top-half overshoot term.
- Raised the high-end timing delay cap from about 28 ms to about 75 ms.
- Widened top-end velocity/energy variation while keeping the path deterministic.
- Moved `Export Settings` to the global header and removed its Articulation-page row.
- Updated project docs, test notes, plan notes, Studio Pattern state, and the changelog.

## Checks To Run

- Build the project and confirm new instances show `v0.2.6 / StringVoice EG-077 PlayerFeelSlop`.
- Confirm `Player Feel = 0` remains neutral.
- Confirm 50% remains musical/natural and 100% is clearly sloppier than 50%.
- Confirm cognitive/dexterity/endurance meters rise more gradually than EG-076 and clear during rests.
- Confirm the settings export is visible from all tabs in the header and still opens copyable JSON.
- Confirm repeated offline renders with the same MIDI/settings remain deterministic.

## Known Limitations

- This still delays note starts only. Early timing still requires a future lookahead-aware interpretation pass.
- The saved JSON snapshot is reference material only; it is not an importable preset or stock preset yet.
