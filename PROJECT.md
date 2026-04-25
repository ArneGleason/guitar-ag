# Project Brief: Physical Model Electric Guitar Instrument

## Vision

Build a virtual electric guitar instrument that uses physical modeling rather than sample playback.

The instrument should behave like a playable guitar-style synth: each note acts like an independently vibrating string, with expressive control over pitch bend, pluck behavior, damping, muting, and tone.

The key control requirement is **MPE**, so individual notes can bend, slide, and change expression independently.

## First Major Success

The first major success is not perfect guitar realism.

The first major success is this:

> Two notes are held at the same time, and one note can bend independently while the other remains stable.

That proves the core MPE and voice-routing model is working.

## Sound Direction

The instrument should sound like an electric guitar source, not an acoustic guitar or a generic plucked synth.

For the MVP, the instrument may output a clean DI-style tone that can be run through external amp and cabinet simulators.

A full amp, cabinet, and effects suite is out of scope for the MVP.

## Implementation Philosophy

Start simple and make it playable.

A Karplus-Strong or digital waveguide-style string model is acceptable for the first version. Improve realism later through better damping, pickup modeling, string stiffness, fret interaction, and guitar-specific behavior.

Avoid large speculative systems before the core voice, MPE, and plugin state are working.
