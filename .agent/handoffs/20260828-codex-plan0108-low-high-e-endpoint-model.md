# Plan 0108 Low/High-E Endpoint Model Handoff

Date: 2026-08-28
Lead: Codex on unassigned Windows PC
Branch: `codex/0108-low-high-e-endpoint-model`
Implementation commit: `a2c6618928e7ebca94beee19d15a8bf3b428a05f`

## Outcome

The three high-E requests are complete with two approved takes each. Every file
contains the expected 6/6/12 onsets, is unclipped 44.1 kHz/24-bit PCM, reports no
dropped audio, and supplies a usable final tail.

The real high E invalidates a uniform transfer of Plan 0105: independent tails
are near -7.1 dB/s, while the accepted 1.50x low-E decay makes modeled high E
only about -2.3 dB/s. Production also places roughly 54–63% of high-E attack
energy above 1.5 kHz versus roughly 4–10% in the recording.

## Offline Candidate

- fixed pluck position: 0.14;
- low-E body decay scale: 1.50x;
- high-E body decay endpoint: 0.50x, geometrically interpolated E2-to-E4;
- high-register partial loss: amount 0.25 with `(harmonic - 1)^1.2` curvature;
- legacy repick crossfade: 1 ms.

The two endpoint controls compile only into the offline renderer. No VST
parameter, preset, or production default changed.

Measured high-E down/up results:

| Source | Centroid | Below 500 Hz | Above 1.5 kHz | Decay |
| --- | ---: | ---: | ---: | ---: |
| Reference | 1015 / 838 Hz | 14.1 / 20.2% | 9.7 / 4.3% | -7.35 / -7.13 dB/s |
| Production | 1728 / 1668 Hz | 1.0 / 2.3% | 62.6 / 53.9% | -3.92 / -3.66 dB/s |
| Endpoint candidate | 960 / 849 Hz | 14.2 / 22.2% | 12.1 / 8.7% | -6.90 / -6.75 dB/s |

Alternate decay remains faster than the recording because the 1 ms de-click
does not preserve moving-string repick energy. Keep that residual visible.

## Human Gate

Listen under:

```text
build-vs2022-x64/diagnostics/eg0108-low-high-e-endpoint/listening
```

Tracks 01–04 are the RMS-matched real reference, production EG-089, uniform
low-E candidate, and endpoint candidate. Tracks 05–08 provide reference/
endpoint and production/endpoint in both orders. Judge string scale/identity,
brittle fizz, legitimate metallic ring, down/up balance, tail, and alternate
restart character.

Do not request high-E damping or A/D/G/B until this verdict. If accepted, the
next engineering task is a separately reviewed production/per-string design,
not simply exposing the offline scalars.

## Verification

- Python reference-capture tests: 4 passed.
- All new/changed Python scripts compile.
- Windows Release `GuitarAG_VST3`, `GuitarAGOfflineRender`, and
  `GuitarAGReferenceCapture` build.
- CTest has no configured C++ tests.
- Default and explicit-neutral new controls reproduce the previous high-E
  control exactly at
  `7590F5AC36C36E43F7C4E146FEE436C25E5B116741B6968BAE84CAC96C22EDBE`.
- Candidate low-E down/up/alternate files are byte-identical to Plan 0105/0106.
- All listening files are mono 44.1 kHz/24-bit, RMS-matched, and unclipped.
- `git diff --check` passes.
