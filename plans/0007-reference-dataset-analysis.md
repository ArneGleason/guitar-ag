# Plan 0007 — Reference Dataset Setup and Analysis

## Goal

Set up Guitar-TECHS as an external reference dataset and create lightweight analysis tools that help compare real electric guitar recordings with Guitar AG.

## Scope

- Keep downloaded audio outside the Git repository.
- Add a download script for selected Guitar-TECHS archives.
- Add a dependency-free WAV manifest/statistics script.
- Add a dependency-free MIDI note-region extractor.
- Document the local data layout and first analysis workflow.
- Download a small starter subset if local permissions/network access allow.

## Out of Scope

- Do not commit downloaded audio or generated manifests.
- Do not train models.
- Do not add sample playback to the plugin.
- Do not implement pickup modeling yet.

## Implementation Notes

Start with `P1_singlenotes.zip`, then add techniques after the workflow is proven.

The first analysis should answer simple orientation questions: file counts, duration, sample rates, channels, peak, and RMS. Richer spectral analysis can come later.

## Verification

The human should be able to verify:

- [ ] Reference data lives outside the repo.
- [ ] `scripts/download-guitar-techs-reference.sh` downloads selected archives.
- [ ] `scripts/analyze-reference-audio.py` writes a CSV manifest.
- [ ] `scripts/extract-midi-notes.py` writes note-region CSVs from MIDI labels.
- [ ] `docs/reference-data.md` explains the workflow.
- [ ] Git status does not include downloaded dataset files.

## Completion Notes

2026-04-25:

- Added `docs/reference-data.md`.
- Added `scripts/download-guitar-techs-reference.sh`.
- Added `scripts/analyze-reference-audio.py`.
- Added `scripts/extract-midi-notes.py`.
- Downloaded and extracted `P1_singlenotes.zip` to `/Users/arnegleason/code/reference-audio/Guitar-TECHS/`.
- Generated `analysis/manifest.csv` for the DI and amp-mic WAVs.
- Generated `analysis/P1_singlenotes_notes.csv` with 142 note events from the synchronized MIDI file.
