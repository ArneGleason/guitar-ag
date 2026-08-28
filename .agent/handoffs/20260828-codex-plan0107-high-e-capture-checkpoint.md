# Plan 0107 High-E Capture Checkpoint Handoff

Date: 2026-08-28
Lead: Codex on unassigned Windows PC
Branch: `codex/0107-high-e-capture-checkpoint`
Implementation commit: `a6f5966d62ee001ae1818df0add6a32c46658e8b`

## Outcome

Human listening accepted Plan 0106's 1 ms candidate as an excellent de-click.
The next capture step is deliberately limited to the opposite endpoint rather
than filling all remaining strings.

The live inventory retains the approved noise floor and six low-E items, then
adds open-high-E ringing down/up/alternate batches at 6/6/12 strokes. Existing
session evidence remains unchanged and the first incomplete item resolves to
`high-e-eval-ringing-down`.

## Setup Record

- EVH Wolfgang Special, neck humbucker.
- Picked halfway between bridge and neck pickups.
- Likely approximately .009 strings; exact set is unknown.
- Strings have been installed a long time and are somewhat grimy, but are not
  described as heavily played or clearly fatigued.
- Generic, worn/broken-in medium celluloid-style pick; exact brand, thickness,
  and composition are unknown.

These uncertainty labels are intentional and must be preserved.

## Human Capture Gate

Close and reopen the already-running Release capture app so it loads inventory
v3. Record one approved batch for each high-E item; a second batch is optional:

1. six independent downstrokes;
2. six independent upstrokes;
3. 12 continuous down-first alternate strokes.

Keep the established 44.1 kHz Focusrite ASIO setup, guitar controls, gain, pick,
force, depth, and picking location fixed. Stop after these three items before
A/D/G/B or damping capture.

## Verification

- Reference-capture Python unit tests: 3 passed.
- Inventory generator compiles with `py_compile`.
- Windows Release `GuitarAGReferenceCapture` builds.
- Live inventory contains 10 items / 3 phases and routes next to high-E down.
- All 43 pre-existing session files retained the same aggregate SHA-256 before
  and after forced definition regeneration:
  `36CADB4BA1B4B5EB3E9EDF68D3D5779AACD6F9CD8D05FAFD197214B4DD07CED9`.
- `git diff --check` passes.

## Next Engineering Step

After the three high-E files are approved, compare low/high endpoint spectrum,
decay, onset, variation, and alternate-repick behavior. Decide register scaling
before requesting middle strings or transferring the accepted offline controls
into the production VST3.
