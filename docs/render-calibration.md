# Calibration Render Workflow

## Purpose

Use a fixed MIDI file to render Guitar AG from Bitwig so each plugin version can be compared against the same musical input and the Guitar-TECHS DI references.

## MIDI File

Use:

```text
tests/midi/single-note-calibration.mid
```

The MIDI contains:

- long notes: E2, A2, D3, G3, B3, E4
- short notes: E2, A2, D3, G3, B3, E4
- velocity: 100
- long-note duration: 1.5 seconds
- short-note duration: 0.12 seconds
- gap between notes: 1 second
- tempo: 120 BPM

## Bitwig Render Steps

1. Open Bitwig.
2. Create a new instrument track.
3. Load `Guitar AG`.
4. Confirm the plugin editor build label, for example:

   ```text
   v0.1.0 / StringVoice KS-003 / e35bf11
   ```

5. Drag `tests/midi/single-note-calibration.mid` into the instrument track.
6. Disable all other effects, EQ, compression, amp sims, and sends.
7. Render/export the track as WAV.
8. Prefer 48 kHz WAV. Mono is ideal, but stereo is okay.
9. Save the render outside the repo:

   ```text
   /Users/arnegleason/code/reference-audio/Guitar-AG-renders/
   ```

10. Name it with the model label and commit:

    ```text
    guitar-ag-ks003-e35bf11-single-note-calibration.wav
    ```

## Why This Helps

Every future Guitar AG build can render the same MIDI. That lets us compare changes against:

- previous Guitar AG versions
- Guitar-TECHS direct-input single notes
- note-off behavior for long versus short notes
- attack brightness and scrape/noise behavior
- future pickup/twang experiments

## Analysis Commands

Standard harmonic/level comparison:

```sh
scripts/compare-calibration-render.py \
  --render-wav /Users/arnegleason/code/reference-audio/Guitar-AG-renders/<render>.wav \
  --render-notes /Users/arnegleason/code/reference-audio/Guitar-AG-renders/single-note-calibration-notes.csv \
  --reference-wav /Users/arnegleason/code/reference-audio/Guitar-TECHS/extracted/P1_singlenotes/audio/directinput/directinput_allsinglenotes.wav \
  --reference-notes /Users/arnegleason/code/reference-audio/Guitar-TECHS/analysis/P1_singlenotes_notes.csv \
  --output /Users/arnegleason/code/reference-audio/Guitar-AG-renders/<comparison>.csv \
  --long-notes-only
```

Time-varying harmonic comparison:

```sh
scripts/analyze-harmonic-evolution.py \
  --render-wav /Users/arnegleason/code/reference-audio/Guitar-AG-renders/<render>.wav \
  --render-notes /Users/arnegleason/code/reference-audio/Guitar-AG-renders/single-note-calibration-notes.csv \
  --reference-wav /Users/arnegleason/code/reference-audio/Guitar-TECHS/extracted/P1_singlenotes/audio/directinput/directinput_allsinglenotes.wav \
  --reference-notes /Users/arnegleason/code/reference-audio/Guitar-TECHS/analysis/P1_singlenotes_notes.csv \
  --output /Users/arnegleason/code/reference-audio/Guitar-AG-renders/<evolution-comparison>.csv \
  --long-notes-only
```

Perceptual-distance comparison:

```sh
scripts/analyze-perceptual-distance.py \
  --render-wav /Users/arnegleason/code/reference-audio/Guitar-AG-renders/<render>.wav \
  --render-notes /Users/arnegleason/code/reference-audio/Guitar-AG-renders/single-note-calibration-notes.csv \
  --reference-wav /Users/arnegleason/code/reference-audio/Guitar-TECHS/extracted/P1_singlenotes/audio/directinput/directinput_allsinglenotes.wav \
  --reference-notes /Users/arnegleason/code/reference-audio/Guitar-TECHS/analysis/P1_singlenotes_notes.csv \
  --output /Users/arnegleason/code/reference-audio/Guitar-AG-renders/<perceptual-comparison>.csv \
  --summary /Users/arnegleason/code/reference-audio/Guitar-AG-renders/<perceptual-summary>.md \
  --long-notes-only
```

This report gain-normalizes each rendered note to candidate reference notes of the same pitch, selects the closest candidate by log-spectral distance, and reports:

- log-spectral distance across the full note, attack, early sustain, and late sustain
- spectral flatness ratio, useful for catching hiss/noise-like changes
- harmonic-band fraction delta, useful for spotting overly pure/static synthesis
- spectral-flux ratio, useful for seeing whether the render is too static or too busy

## Offline Render Harness

For fast DSP iteration, the calibration MIDI can also be rendered without Bitwig:

```sh
cmake --build build --config Release --target GuitarAGOfflineRender

build/GuitarAGOfflineRender_artefacts/Release/GuitarAGOfflineRender \
  --midi tests/midi/single-note-calibration.mid \
  --output /Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-offline-<model>-<commit>-single-note-calibration.wav \
  --sample-rate 48000 \
  --block-size 512 \
  --tail-seconds 2.0
```

The offline renderer drives the shared `AudioEngine` directly. It is appropriate for fast unattended experiments and analysis, but human DAW renders remain important before deciding whether a sound change actually feels better.

The first KS015 offline render correlated very closely with the Bitwig render, but was about 10 dB louder. Use `--gain 0.316` when a DAW-level-matched WAV is useful.

## Verified MIDI Contents

The generated MIDI has 12 note events:

```text
E2 A2 D3 G3 B3 E4  long notes, 1.5 seconds each
E2 A2 D3 G3 B3 E4  short notes, about 0.12 seconds each
```

All notes are on MIDI channel 1 at velocity 100.

## Velocity Ladder MIDI

Use this file when testing whether velocity changes the character of the pluck rather than only loudness:

```text
tests/midi/velocity-ladder-open-strings.mid
```

The MIDI contains each open string as six 1.2-second notes:

```text
E2 A2 D3 G3 B3 E4 at velocities 25, 45, 65, 85, 110, 127
```

Extract its note regions with:

```sh
scripts/extract-midi-notes.py \
  tests/midi/velocity-ladder-open-strings.mid \
  --output /Users/arnegleason/code/reference-audio/Guitar-AG-renders/velocity-ladder-open-strings-notes.csv
```

Analyze a velocity-ladder render with:

```sh
scripts/analyze-velocity-ladder.py \
  --render-wav /Users/arnegleason/code/reference-audio/Guitar-AG-renders/<render>.wav \
  --notes /Users/arnegleason/code/reference-audio/Guitar-AG-renders/velocity-ladder-open-strings-notes.csv \
  --output /Users/arnegleason/code/reference-audio/Guitar-AG-renders/<analysis>.csv \
  --summary /Users/arnegleason/code/reference-audio/Guitar-AG-renders/<summary>.md
```
