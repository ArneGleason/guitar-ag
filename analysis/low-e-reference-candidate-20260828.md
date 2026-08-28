# Plan 0105 Low-E Reference Candidate

## Outcome

The first small reference-driven candidate is measurably closer to the approved
low-E captures. Human listening judged it better; it remains offline only while
Plan 0106 removes the alternate-picking restart tick before promotion.

The candidate fixes physical pluck position at 0.14 of speaking length and
multiplies the long-lived harmonic-body decay time by 1.50. It keeps EG-089's
accepted modal excitation recipe and matches the capture metadata with the
virtual humbucker at its neck-side position. No fretting-hand damping or new
additive pick-noise layer is included.

## Geometry Bracket

Ringing-down 60 ms attack medians at 44.1 kHz:

| Pluck position | Centroid | Energy below 500 Hz | Energy above 1.5 kHz | Early body / attack peak |
| --- | ---: | ---: | ---: | ---: |
| Human reference | 262 Hz | 90.8% | 0.7% | -10.0 dB |
| EG-089 automatic (~0.10) | 537 Hz | 64.0% | 9.0% | -8.3 dB |
| 0.12 | 505 Hz | 72.9% | 8.3% | -9.5 dB |
| 0.13 | 471 Hz | 77.1% | 8.7% | -9.5 dB |
| **0.14** | **360 Hz** | **82.8%** | **4.7%** | **-8.4 dB** |
| 0.15 | 383 Hz | 81.5% | 5.8% | -7.5 dB |
| 0.16 | 413 Hz | 79.4% | 7.3% | -7.6 dB |
| 0.18 | 377 Hz | 75.6% | 4.7% | -7.7 dB |
| 0.22 | 397 Hz | 73.2% | 5.8% | -8.8 dB |

The response is non-monotonic because pluck and pickup positions move harmonic
nodes relative to one another. Position 0.14 is the narrow measured minimum; a
generic "farther from the bridge is darker" rule would have selected poorly.

## Decay Bracket

At fixed position 0.14, the ringing-down decay changed from -2.59 dB/s at 1.0x
to -2.00, -1.68, -1.35, and -1.12 dB/s at 1.3x, 1.5x, 1.8x, and 2.1x. The
reference tails were -1.02 dB/s down, -2.06 dB/s up, and -1.72 dB/s after
alternate picking. The selected 1.5x candidate is a cross-context compromise,
not a fit to the single longest tail.

## Current Versus Candidate

| Context | Source | Centroid | Below 500 Hz | Above 1.5 kHz | Early body / peak | Decay |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| Down | reference | 262 Hz | 90.8% | 0.7% | -10.0 dB | -1.02 dB/s |
| Down | current | 537 Hz | 64.0% | 9.0% | -8.3 dB | -2.75 dB/s |
| Down | candidate | 367 Hz | 82.3% | 5.0% | -8.1 dB | -1.68 dB/s |
| Up | reference | 220 Hz | 95.0% | 0.4% | -9.5 dB | -2.06 dB/s |
| Up | current | 506 Hz | 76.4% | 4.1% | -9.4 dB | -2.72 dB/s |
| Up | candidate | 469 Hz | 79.2% | 5.1% | -9.6 dB | -1.84 dB/s |
| Alternate | reference | 227 Hz | 91.4% | 0.5% | -9.3 dB | -1.72 dB/s |
| Alternate | current | 500 Hz | 71.6% | 5.6% | -9.1 dB | -2.66 dB/s |
| Alternate | candidate | 420 Hz | 79.8% | 4.9% | -9.3 dB | -1.85 dB/s |

The downstroke centroid error falls by about 62% and its sub-500 Hz error by
about 68%. Alternate-picking centroid error falls about 29% and its sub-500 Hz
error about 41%. The upstroke improvement is smaller, and its above-1.5 kHz
residual becomes slightly worse. This is meaningful evidence of progress, not a
claim that low-E identity is finished.

## Listening Gate

The ignored diagnostic directory is:

```text
build-vs2022-x64/diagnostics/eg0105-low-e-reference-candidate
```

Its `listening` subdirectory contains separate 18.9-second current and candidate
files plus both playback orders. Each file concatenates an independent down
stroke, independent up stroke, long downstroke tail, and alternate sequence.
The two tracks are RMS-matched to -18 dBFS; neither clips. Listen through the
same clean-DI or amp-sim chain and judge attack identity, low-E body, tail, and
whether the candidate merely sounds darker/longer.

Human verdict: the candidate is better. The regular alternate-picking tick is a
separate legacy restart artifact diagnosed in
`analysis/legacy-repick-declick-20260828.md`; production promotion waits on that
de-click listening gate.

## Reproduction

The offline-only renderer flags are:

```text
--legacy-pluck-position auto|0.055..0.280
--legacy-body-decay-time-scale 0.50..4.00
```

Use `scripts/analyze-low-e-reference-candidates.py` with Plan 0104's
`events.csv` to reproduce the table and
`scripts/create-low-e-candidate-listening-set.py` to regenerate the compact A/B.
With both new controls omitted, the rebuilt pickup-matched downstroke render is
byte-identical to Plan 0104 (`SHA256
BF1A101EFF7EF0407590C9C15F8B41AD4CF4117607E7467AF8462AF1A3975DFE`).
The Windows Release VST3 builds successfully and does not compile or expose the
offline controls. A full relink changes binary metadata, so plugin-file hashes
are not used as the behavior-neutrality claim.
