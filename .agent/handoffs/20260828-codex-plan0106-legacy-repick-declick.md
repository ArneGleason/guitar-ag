# Plan 0106 Legacy Repick De-click Handoff

Date: 2026-08-28
Lead: Codex on unassigned Windows PC
Branch: `codex/0106-legacy-repick-declick`
Implementation commit: `02d9561d31b76b890bd32be89fe439e75e8f1579`

## Outcome

The human accepted the Plan 0105 low-E candidate as better and identified a
regular tick in alternate picking. The report correctly pointed toward legacy
voice restart, although the largest derivative occurs 0.6-3 ms after MIDI
note-on rather than on the exact boundary sample.

Direct attack/contact ablations leave the spike unchanged. An offline-only
outgoing modal-state tail reduces the median post-onset spike from +9.2 dB above
the preceding waveform slope distribution to -3.6 dB with a 1 ms fade. Three,
five, and eight milliseconds are no better, so 1 ms is the selected human gate.

## Human Gate

Listen under:

```text
build-vs2022-x64/diagnostics/eg0106-legacy-repick-declick/listening
```

`01-accepted-candidate-restart.wav` is the ticked control and
`02-one-ms-modal-tail-crossfade.wav` is the candidate. Files 03 and 04 contain
both orders. The 5.85-second files are -18 dBFS RMS matched with less than
0.001 dB gain difference.

Confirm that the regular tick is reduced without making the legitimate pick
attack soft, doubled, or flammed. If accepted, promote Plan 0105 plus the
accepted de-click behavior in a separate production change with broader
polyphony, register, DAW, and MPE checks.

## Verification

- Windows Release `GuitarAG_VST3` and `GuitarAGOfflineRender` build.
- Neutral renderer output remains Plan 0104 byte-identical at
  `BF1A101EFF7EF0407590C9C15F8B41AD4CF4117607E7467AF8462AF1A3975DFE`.
- Explicit 0 ms reconstructs the accepted ticked control exactly at
  `DADB4399C63FCAB1319A072DC8B63DDDC765460B690CE80AA824E6AFF4AB29E4`.
- The 1 ms render repeats exactly at
  `798F91D6D16C8C372CAB0B0DD670FD8E881D734DABAEFD6AF511FAEFDB0CA886`.
- Reference-capture script unit tests: 3 passed.
- New scripts compile and ran end-to-end; CTest reports no configured C++ tests.

## Reviewer Focus

- Tail storage is allocated during `prepare()`; the picked note event copies
  one already-allocated `StringVoice` without allocating.
- The full modal-state copy remains offline-only and is not automatically the
  correct production architecture.
- Verify production promotion against six-string polyphony, MPE state,
  feedback, note stealing, and ordinary isolated attacks.
