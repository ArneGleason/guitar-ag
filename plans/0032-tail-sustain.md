# Plan 0032 — Tail Sustain

## Goal

Extend held-note sustain without using compressor-style gain riding.

## Scope

- Keep KS023 clear DI tone and velocity curve.
- Add per-modal tail damping scale.
- Let early note decay behave normally.
- Relax modal damping in the held tail.
- Preserve short attack/chirp decay.
- Lower the quiet-tail voice cutoff.

## Non-Goals

- No compressor or AGC.
- No new pickup model.
- No UI control.
- No MPE changes.

## Result

Implemented as `StringVoice KS-024 TailSustain`.

Offline long-note render:

```text
/Users/arnegleason/code/reference-audio/Guitar-AG-renders/guitar-ag-ks024-tailsustain-dirty-long-sustain-open-strings.wav
```

In the 8-second open-string test, 6-second RMS remains around 12-16% of the 0.5-second RMS depending on string.
