# 0068 — Motion-Coupled Slide Squeak

## Context

Human spectrum/listening feedback on EG-067 showed that the noise still read as a mostly fixed hiss. Even though EG-067 added squeak-like impulses, the slide path still used a mostly fixed carrier and the random scrape component dominated. The finger/string noise also did not scale clearly with the speed of the neck-slide gesture.

## Goal

Make slide squeak behave like a contact phenomenon driven by motion:

- faster `Neck Slide` movement should produce stronger and denser squeak;
- slower movement should soften the squeak;
- stopped movement should let the squeak disappear quickly;
- the tonal part should track the current string/pitch region instead of staying at one fixed pitch;
- broadband noise should support the squeak rather than dominate it.

## Implementation

- Added a short slide-motion drive envelope inside `StringVoice`.
- Converted slide squeak pulse spacing and amplitude to follow recent slide speed.
- Retuned the slide squeak carrier from the current modal/string frequency and slide speed.
- Shortened the slide scrape decay so stopped movement falls away quickly.
- Reduced the random scrape weighting in both slide scrape and global `Finger Noise` approach/release events.
- Updated the model label to `StringVoice EG-068 SlideSqueakMotion`.

## Verification Targets

- A fast `Neck Slide` with `Fret Steps` and/or `Slide Lift` raised should produce stronger squeak than a slow slide.
- Holding `Neck Slide` static should not produce a continuous hiss wash.
- The spectrum should show more discrete moving squeak energy and less straight broadband high-frequency slope.
- `Finger Noise` approach/release events should sound less like static hiss.
