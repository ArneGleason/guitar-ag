# Plan 0060 - Spaced Single-Coil Out Of Phase Pickup

## Goal

Replace the old third pickup choice with a more conventional two-single-coil out-of-phase model.

## Background

The old `Humbucker OOP` choice subtracted two very nearby coil readouts. That creates a strong small-spacing cancellation effect, but it is not the more familiar neck/middle or bridge/middle out-of-phase electric-guitar sound.

## Implementation

- Rename the third pickup choice to `Singles OOP`.
- Keep `Single Coil` and `Humbucker` unchanged.
- Use two single-coil-width readouts instead of humbucker-width readouts.
- Space the two readouts by 0.086 of the speaking string length.
- Treat `Pickup Position` as the center of that pair, constrained so both readouts stay inside the existing pickup-position range.
- Subtract the bridge-side readout from the neck-side readout.
- Update the visible model label to `StringVoice EG-053 SinglesOOP`.
- Bump the local plugin version to `0.2.2`.

## Manual Verification

- Compare pickup model 1 and 2 to confirm single coil and humbucker remain familiar.
- Select `Singles OOP` and move `Pickup Position` from bridge-side to neck-side.
- Listen for a nasal, notched, wider-spaced two-pickup phase-cancellation character.
- Verify the output is not simply much quieter or hollowed out beyond usefulness.
- Check the new label appears in the UI before release prep.
