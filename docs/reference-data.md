# Reference Data Workflow

## Purpose

Use external electric guitar recordings as listening and analysis references while keeping downloaded audio out of the Git repository.

The first reference source is Guitar-TECHS:

- Website: https://guitar-techs.github.io/
- Dataset: https://zenodo.org/records/14963133
- License: Creative Commons Attribution 4.0
- Paper: Hegel Pedroza et al., "Guitar-TECHS: An Electric Guitar Dataset Covering Techniques, Musical Excerpts, Chords and Scales Using a Diverse Array of Hardware," ICASSP 2025.

## Local Data Location

Reference data should live outside this repository:

```text
/Users/arnegleason/code/reference-audio/Guitar-TECHS/
```

Do not commit downloaded dataset files, extracted WAVs, generated analysis CSVs, or other derived audio assets to this repo.

## Initial Download

Start small:

```bash
scripts/download-guitar-techs-reference.sh P1_singlenotes.zip
```

Useful next files:

```bash
scripts/download-guitar-techs-reference.sh P1_techniques.zip
scripts/download-guitar-techs-reference.sh P2_singlenotes.zip
scripts/download-guitar-techs-reference.sh P2_techniques.zip
```

The script downloads archives to:

```text
/Users/arnegleason/code/reference-audio/Guitar-TECHS/archives/
```

and extracts them to:

```text
/Users/arnegleason/code/reference-audio/Guitar-TECHS/extracted/
```

## Initial Analysis

Generate a dependency-free audio manifest:

```bash
scripts/analyze-reference-audio.py \
  /Users/arnegleason/code/reference-audio/Guitar-TECHS/extracted \
  --output /Users/arnegleason/code/reference-audio/Guitar-TECHS/analysis/manifest.csv
```

The manifest includes:

- relative path
- file size
- sample rate
- channels
- sample width
- frame count
- duration
- peak amplitude
- RMS amplitude

This is enough to orient around the dataset before doing deeper spectral work.

Extract MIDI note regions:

```bash
scripts/extract-midi-notes.py \
  /Users/arnegleason/code/reference-audio/Guitar-TECHS/extracted/P1_singlenotes/midi/midi_allsinglenotes.mid \
  --output /Users/arnegleason/code/reference-audio/Guitar-TECHS/analysis/P1_singlenotes_notes.csv
```

This CSV gives note names, velocities, start/end times, and durations. It can be used to locate corresponding regions in the DI and amp-mic WAV files.

## Current Local Snapshot

As of 2026-04-25, the local reference folder contains:

- `P1_singlenotes.zip`
- extracted `P1_singlenotes`
- `analysis/manifest.csv`
- `analysis/P1_singlenotes_notes.csv`

Initial manifest:

- `directinput_allsinglenotes.wav`: 48 kHz mono, 24-bit, 552.0 seconds, peak 0.244766, RMS 0.010144
- `micamp_allsinglenotes.wav`: 48 kHz mono, 16-bit, 552.0 seconds, peak 0.206970, RMS 0.010070

Initial MIDI extraction:

- 142 note events
- first note: E2 at 0.000 seconds
- note events include MIDI channel, note name, velocity, start/end time, and duration

## Why This Helps Guitar AG

Current listening feedback says `StringVoice KS-003` mutes better, but the tone still reads more nylon-like than steel-string electric. The likely missing pieces are not just EQ:

- steel-string stiffness and inharmonicity
- pick scrape and release detail
- pickup-position comb filtering
- pickup and guitar-circuit resonance
- pickup mixing/phase behavior for Strat-like twang or quack

The reference data should help answer:

- How bright are real DI single notes by string, fret, player, and pickup?
- How much noise/scrape exists before and after the pitched attack?
- How quickly do short notes decay versus long notes?
- How different are DI, amp mic, and room/head-mounted microphone perspectives?
- Which guitar/player/pickup combinations sound closest to the target instrument?

## Suggested First Comparisons

Start with DI single-note examples:

- compare real DI attack duration to `StringVoice KS-003`
- compare short note-off damping behavior
- compare spectral centroid or high-frequency energy once a richer analyzer exists
- inspect whether twang/quack appears in DI or mainly in amp/mic signals

Then move to techniques:

- palm mute for damping behavior
- vibrato for pitch modulation shape
- bends for pitch trajectory and sustain
- harmonics/pinch harmonics for future excitation and contact models
