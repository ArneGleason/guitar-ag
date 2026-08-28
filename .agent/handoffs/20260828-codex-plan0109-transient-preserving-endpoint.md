# Plan 0109 Transient-Preserving Endpoint Handoff

Date: 2026-08-28
Lead: Codex on unassigned Windows PC
Branch: `codex/0109-transient-preserving-endpoint`
Implementation commit: `4473962d58bd0aa6a77bdd5466a7f989fc3bdfd7`

## Human Correction

Plan 0108 is rejected. The real high-E reference was brighter and more
convincing; the amplitude-damped endpoint sounded dark, large, flat, and less
bright. Production EG-089 was preferred despite a remaining glassy/new-string
quality. Do not promote Plan 0108 based on its close 60 ms aggregate metrics.

The player also noted that Plan 0105 never provided an actual low-E reference
versus accepted-candidate gate. Plan 0109 supplies it.

## Revised Candidate

The new offline high-register control shortens modal decay above a fixed 1.2 kHz
hinge without changing modal start amplitude. The first candidate uses:

- fixed pluck 0.14;
- 1.50x body decay at low E and 1.00x at high E;
- no Plan 0108 amplitude damping;
- upper-partial decay amount 2.0;
- 1 ms legacy repick de-click.

It is intentionally intermediate rather than optimized to the rejected
aggregate target. High-E down initial/60 ms centroids are 1463/1333 Hz versus
1704/1728 Hz production and 966/960 Hz rejected; above-1.5 kHz energy is
40.5/35.1% versus 62.3/62.6% production and 12.3/12.1% rejected.

## Human Gates

Low E, actual reference versus the already accepted candidate:

```text
build-vs2022-x64/diagnostics/eg0109-transient-preserving-endpoint/listening-low-e
```

Use tracks 04 and 05 for both orders.

High E, production versus the mild transient candidate:

```text
build-vs2022-x64/diagnostics/eg0109-transient-preserving-endpoint/listening-high-e
```

Use tracks 07 and 08 for production/candidate in both orders, then tracks 05 and
06 for real reference/candidate. Tracks 09/10 include the rejected dark endpoint
only as a contrast.

Judge whether the low-E candidate is actually closer to the guitar and whether
the high-E transient candidate reduces glass without becoming flat. No further
capture is requested.

## Verification

- Python reference-capture tests: 4 passed; all changed/new scripts compile.
- Windows Release VST3, offline renderer, and capture app build.
- Default and explicit-neutral high-E production renders remain exact at
  `7590F5AC36C36E43F7C4E146FEE436C25E5B116741B6968BAE84CAC96C22EDBE`.
- Transient-candidate low-E down/up/alternate files are byte-identical to the
  accepted Plan 0105/0106 files.
- All low/high listening files are mono 44.1 kHz/24-bit, RMS-matched, and
  unclipped.
- CTest has no configured C++ tests; `git diff --check` passes.

## Next Step

Wait for both listening verdicts. If the high-E candidate remains glassy, the
existing amount-4/0.75x bracket is available; if it is flat, reject this decay
family rather than increasing it. Do not implement production or request middle
strings until the human gate is explicit.
