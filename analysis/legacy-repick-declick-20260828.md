# Plan 0106 Legacy Repick De-click

## Human Finding

Plan 0105's 0.14 pluck-position / 1.50x body-decay candidate was judged better,
but its alternate-picking section exposed a regular tick resembling a synth
voice restart at a nonzero waveform value.

## Diagnosis

The production legacy engine selects the already-active voice for a physical
string and calls `StringVoice::start()` again. That reconstructs its modal bank
instead of applying a pick force to the existing phase/energy state.

The exact output sample at each new note-on is not the largest discontinuity:
the tone filter carries enough state that the median boundary derivative is
-56.2 dBFS. The abnormal transient appears 0.6-3 ms after the restart. Across
the 11 repicks, the largest derivative in the following 5 ms is a median 9.2 dB
above the 99th percentile of the preceding 50 ms, with a 15.0 dB 90th
percentile and 15.2 dB maximum.

Disabling attack modes, the contact layer, the pick transient, or all direct
layers leaves the result effectively unchanged. The tick therefore belongs to
the reconstructed modal onset/state transition, not an exposed additive click
sample.

## Offline Candidate

On a same-string picked restart, copy the outgoing modal voice into a dedicated
offline tail slot. Let that copy continue at its real phase while applying a
smooth fade, and let the restarted voice retain its existing attack ramp. Tail
storage is allocated during `AudioEngine::prepare()` and copied without
allocation at the note event. The production VST3 does not compile this path.

| Tail crossfade | Median post-onset spike | P90 | Maximum |
| --- | ---: | ---: | ---: |
| Off | +9.2 dB | +15.0 dB | +15.2 dB |
| **1 ms** | **-3.6 dB** | **+0.9 dB** | **+1.8 dB** |
| 3 ms | -3.5 dB | +0.9 dB | +1.8 dB |
| 5 ms | -3.3 dB | +0.8 dB | +1.8 dB |
| 8 ms | -2.7 dB | +1.6 dB | +2.4 dB |

One millisecond is selected because longer fades do not improve suppression and
retain more of an overlapping reset voice. This is a de-click transition, not
yet a physically complete moving-string repick model; it preserves outgoing
phase only long enough to remove the synthetic boundary artifact.

## Listening Gate

The ignored local directory is:

```text
build-vs2022-x64/diagnostics/eg0106-legacy-repick-declick/listening
```

Files 01 and 02 are 5.85-second accepted-candidate alternate sequences with the
crossfade off and at 1 ms. Both are RMS-matched to -18 dBFS with less than
0.001 dB gain difference. Files 03 and 04 contain both playback orders.

Human listening must confirm that the repeated tick is reduced without making
the legitimate pick attack soft, doubled, or flammed. If accepted, promote the
Plan 0105 low-E candidate and this transition in a separate production change,
with broader polyphony and ordinary-playing regression material.

## Reproduction

The offline flag is:

```text
--legacy-repick-crossfade-ms 0.0..20.0
```

Use `scripts/analyze-repick-discontinuities.py` to reproduce the derivative
table and `scripts/create-repick-declick-listening-set.py` to regenerate the
order-balanced A/B. With the flag omitted, the Plan 0104 neutral renderer hash
remains `BF1A101EFF7EF0407590C9C15F8B41AD4CF4117607E7467AF8462AF1A3975DFE`.
The 1 ms render repeats deterministically at
`798F91D6D16C8C372CAB0B0DD670FD8E881D734DABAEFD6AF511FAEFDB0CA886`.
