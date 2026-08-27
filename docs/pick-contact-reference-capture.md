# Pick-Contact Reference Capture

## Purpose

Capture real electric-guitar DI examples that emphasize plectrum/string contact
so Guitar AG can fit a real-time synthesized excitation model. The recordings
are analysis references, not samples for playback.

## Signal Chain

- Record clean DI with no amp, cabinet, compression, gate, denoiser, or time
  effects.
- Use 48 or 96 kHz, 24-bit WAV. Leave at least 12 dB of peak headroom.
- Keep guitar pickup selection, volume, and tone fixed for the whole set and
  write those settings down.
- Record 5–10 seconds of untouched input noise before playing.

## Muting

Make two complementary muted takes; neither perfectly isolates contact alone:

1. Lightly touch the string with the fretting hand at several non-harmonic
   positions while picking normally. This suppresses sustained pitch but keeps
   much of the local pick/string interaction.
2. Weave soft foam or cloth through the strings near the neck pickup/fretboard,
   away from the pick. This damps propagation while avoiding palm material at
   the picking point.

Also record an ordinary ringing counterpart for every setup. Subtracting or
comparing the muted and ringing measurements is more trustworthy than treating
the muted take as pure pick noise.

## Minimum Performance Matrix

For low E, D, B, and high E strings, record 8–12 isolated strokes for each:

- downstroke and upstroke
- light, medium, and hard velocity
- shallow and deep pick depth
- one flexible and one stiff pick if available

Leave roughly half a second between strokes. Add a spoken slate or filename
that identifies string, direction, velocity, depth, pick, muting method, guitar,
and pickup.

## Measurements to Fit

- contact duration and onset/release envelope
- transient crest factor and repeat-to-repeat variation
- event/zero-crossing density through the contact window
- energy and decay in fixed-Hz bands rather than only harmonic-number bands
- tonal versus stochastic energy and its dependence on string, depth, and pick
- muted-to-ringing transfer, used to separate excitation traits from the string
  response without copying either waveform

The implementation target is a deterministic stochastic force/contact model
whose distributions match these measurements. No captured waveform needs to be
shipped or replayed by the instrument.

