# Plan 0025 — Modal Cluster Listening Pivot

## Goal

Make the modal-cluster experiment the active sound branch because it was strongly preferred by listening, despite worse current analyzer scores.

## Motivation

The user auditioned the experiment WAVs and found:

```text
/Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-offline-ks016-modalcluster-dirty-single-note-calibration.wav
```

to sound much more like a real electric guitar string than the later `KS016 HybridModal Pickup-002` installed build.

This is a useful mismatch. It suggests the analysis pipeline is not yet measuring the perceptual cues that matter most.

## Scope

- Recreate the modal-cluster candidate as a clean source-controlled build.
- Keep the calibration render byte-identical to the liked render if possible.
- Install the VST3 for Bitwig audition.
- Document why the current analyzer may be misleading.

## Out of Scope

- No fretting model.
- No MPE work.
- No amp/cab simulation.
- No UI controls.
- No new tuning pass until the recreated branch is auditioned.

## Results

2026-04-25:

- Recreated the liked modal-cluster render as `StringVoice KS-017 ModalCluster`.
- Verified the new offline render SHA-256 matches the earlier liked render exactly:
  `44ecaeaf190d82d3bdfb9a09fcab4bd43ca10258a0e98a3d048c1e4b0a747baf`.
- The current analyzer still reports a worse full log-spectral distance, about 32.20 dB, so the mismatch is confirmed rather than accidental.

## Verification

- [x] Offline renderer builds.
- [x] Offline calibration render matches the liked modal-cluster WAV byte-for-byte.
- [x] Perceptual, calibration, and harmonic-evolution analyses ran.
- [x] VST3 installed for Bitwig audition.
- [ ] Human listening check in Bitwig.
