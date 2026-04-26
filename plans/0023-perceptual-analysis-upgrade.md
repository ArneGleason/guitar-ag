# Plan 0023 — Perceptual Analysis Upgrade

## Goal

Improve the analysis loop so it better matches what the ear calls "electric guitar" instead of only rewarding narrow harmonic-ratio movements.

Recent experiments showed that the current metrics can move in a positive direction while the sound remains synthetic or even gets worse. The analysis layer should become a guide for experiments, not a misleading objective function.

## Problems With Current Metrics

- Upper-to-low and air-band ratios can reward hiss/noise that sounds unlike a string.
- Harmonic movement metrics do not distinguish coherent metallic partial motion from stochastic texture.
- The scripts mostly compare isolated summary values, not the time-frequency shape of the whole note.
- Loudness/gain differences can obscure whether the spectral shape actually improved.
- The analysis does not yet compare the rendered note against multiple reference examples of the same pitch/articulation.

## Proposed Additions

- Gain-normalized multi-scale spectral distance.
- Log-magnitude STFT distance across attack, early sustain, and late sustain.
- Spectral centroid, flatness, rolloff, and noisiness over time.
- Harmonic-to-noise ratio or residual-noise estimate after harmonic tracking.
- Attack transient duration and pick/noise decay curve.
- Per-note ranking against several Guitar-TECHS DI examples, not only one selected region.
- A compact score report that keeps objective metrics separate from subjective listening notes.

## Out of Scope

- Do not train a neural model yet.
- Do not collapse the result into one magic realism number.
- Do not treat metrics as more important than listening.

## Verification

- [x] The upgraded analysis can flag KS013-style hiss as noise-like even when air-band ratios improve.
- [x] The report compares KS versions against same-pitch reference candidates with gain normalization.
- [x] The output is concise enough to guide the next DSP experiment.

## Completion Notes

2026-04-25:

- Added `scripts/analyze-perceptual-distance.py`.
- The script is standard-library only, using log-spaced Goertzel bins instead of NumPy/SciPy.
- Generated reports for KS013, KS014, and KS015 under `/Users/arnegleason/code/reference-audio/Guitar-AG-renders/`.
- The new metrics better reflect the listening story:
  - KS013: lower wound-string spectral distance improved, but flatness shows the added material is too noise-like.
  - KS014: reduced the noise-like flatness but stayed in the same local region.
  - KS015: moved farther away spectrally and became too sparse/static.
- Next use: run offline render batches and rank candidates with this report before asking for a Bitwig audition.
