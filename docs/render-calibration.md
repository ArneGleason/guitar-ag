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

## Verified MIDI Contents

The generated MIDI has 12 note events:

```text
E2 A2 D3 G3 B3 E4  long notes, 1.5 seconds each
E2 A2 D3 G3 B3 E4  short notes, about 0.12 seconds each
```

All notes are on MIDI channel 1 at velocity 100.
