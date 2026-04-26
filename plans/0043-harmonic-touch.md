# Plan 0043: Harmonic Touch

## Goal

Add a first playable natural/artificial-harmonic style articulation without changing the fretboard mapper or MPE behavior.

## Research Notes

- Natural harmonics are produced by lightly touching a node on the speaking string while picking, then lifting the touch finger quickly enough for the selected harmonic to ring.
- The most practical first nodes are string divisions 1/2, 1/3, and 1/4, corresponding to the 12th, 7th/19th, and 5th/24th fret harmonic families.
- In the current modal voice, the simplest model is a note-start modal mask: exact touches strongly suppress modes that do not have a node at the touched point, while inaccurate touches leak more of the normal string spectrum.

## Change

- Label this pass `StringVoice KS-035 HarmonicTouch`.
- Add a `Harmonic Touch` VST parameter.
- Map 0% to 25% to normal, non-harmonic playing.
- Map 25% to 50% to the 1/4 string-length harmonic family.
- Map 50% to 75% to the 1/3 string-length harmonic family.
- Map 75% to 100% to the 1/2 string-length midpoint octave harmonic.
- Within each harmonic band, the top of the band is the most accurate touch. Lower values leak more normal-string partials and are less pure.

## Verification

- Build the offline renderer and VST3.
- Render diagnostic harmonic-touch values at 0%, 50%, 75%, and 100%.
- Install the VST3 for manual Bitwig audition.
