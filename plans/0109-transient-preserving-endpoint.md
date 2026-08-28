# Plan 0109 — Transient-Preserving High-E Endpoint

## Goal

Respond to the human rejection of Plan 0108's dark, large, flat high-E endpoint
candidate. Preserve production's preferred brightness at contact while testing
whether only the glassy upper-partial persistence should be shortened.

## Human Evidence

- In the reference-then-endpoint comparison, the real reference was clearly
  brighter and more convincing than the endpoint candidate.
- In the production-then-endpoint comparison, production was preferred despite
  retaining a slightly glassy/new-string character.
- Plan 0108's amplitude-domain upper-partial loss is therefore rejected as a
  production direction; its 60 ms aggregate match removed perceptually useful
  onset brightness.
- The player also identified a missing listening gate: actual low-E reference
  versus the accepted Plan 0105 low-E candidate.

## Scope

- Keep production high E as the baseline.
- Add an offline-only fixed-Hz upper-partial decay intervention that changes
  persistence, not initial modal amplitude.
- Bracket high-register fundamental/body decay separately from upper-partial
  decay.
- Measure a short initial-onset window separately from the existing 60 ms
  aggregate so the analysis cannot call a flat attack an improvement.
- Prepare actual-reference comparisons for both low E and high E.

## Non-Goals

- No production VST3 change or new user-facing parameter.
- No more reference capture.
- No high-E muting or A/D/G/B expansion.
- No assumption that a numerically close spectral average outranks listening.

## Verification

- [x] Production/default and explicit-neutral renders remain exact.
- [x] New candidates change modal decay coefficients, not the selected
      pre-decay modal-amplitude calculation.
- [x] Low-E reference/candidate gate is generated and unclipped.
- [x] High-E reference/production/candidate gate is generated and unclipped.
- [x] Release VST3/offline/capture builds and Python tests pass.
- [ ] Human selects or rejects the time-shaped direction.
