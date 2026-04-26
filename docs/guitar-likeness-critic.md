# Guitar-Likeness Critic

## Purpose

The current spectral analyzers are useful diagnostics, but they do not reliably answer the question we actually care about:

```text
Does this sound like an isolated clean DI electric guitar string?
```

The goal of the guitar-likeness critic is to preserve listening judgments and reference examples in a machine-readable form, then later use those labels to test embedding models or train a small local classifier.

This is not intended to replace listening. It is intended to reduce blind alleys between listening passes.

## First Small Step

The first step is a labeled corpus manifest:

```text
analysis/guitar-likeness-corpus.json
```

The manifest points to audio files outside the repo and groups them into buckets:

- `positive_real_di`: real isolated electric-guitar DI references
- `preferred_synth`: Guitar AG renders that sound directionally closer by listening
- `rejected_synth`: renders that are known to sound synthetic, noisy, static, or otherwise less useful

The repository does not store the audio files themselves.

## Why Not Use a Pretrained Class Score Directly?

Broad pretrained audio models may learn `electric guitar` from mixed, distorted, amplified, or genre-context examples. That can be useful, but it may not match this project's target: isolated clean DI single-note realism.

The safer approach is:

1. Use our own DI references and listening labels as ground truth.
2. Use pretrained models only as feature extractors at first.
3. Compare whether embedding distances agree with the known labels.
4. Train a tiny local classifier only if the embedding space proves useful.

## Current Tooling

Validate and summarize the corpus:

```sh
scripts/summarize-guitar-likeness-corpus.py
```

The summary reports:

- which files exist locally
- WAV duration, sample rate, channel count, and sample width
- bucket counts
- linked note-region CSVs
- current perceptual-analysis means when available

Use `--strict` if missing external files should fail the command.

## Next Possible Step

After the manifest is useful, add an optional embedding stage:

- extract embeddings from a pretrained audio model
- compute distance from each render to the `positive_real_di` examples
- compute distance from each render to known rejected examples
- report whether a new render moved toward real DI or toward rejected synth examples

The first embedding experiment should be a report-only tool. It should not drive DSP changes until it agrees with at least a few human listening labels.
