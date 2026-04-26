# Plan 0035: Pick Range Retune

## Goal

Respond to the first listening pass on the pick material controls:

- make the soft end of `Pick Stiffness` softer and more flexible
- make `Pick Texture` clearly audible at high settings
- keep the 50%/50% default close to the previous pick feel
- avoid changing the sustain, fretboard mapping, MPE roadmap, or amp/cab scope

## Implementation

- Label the DSP build as `StringVoice KS-027 PickRange`.
- Widen the stiffness mapping below 50% by increasing onset ramp time and reducing pick-edge/partial gains.
- Keep the upper half close to the previous hard-pick behavior, with a little extra snap available at 100%.
- Change texture scaling so 0% is effectively frictionless/no texture, 50% is the previous default, and 100% is intentionally more scrape-forward.
- Mix the existing pick-contact scrape envelope into `renderSample`; it was previously configured at note start but not rendered as its own audible contact component.

## Verification

- Build the offline renderer and VST3 target.
- Install the VST3 using `scripts/install-vst3.sh --build`.
- Manually audition the three pick controls in Bitwig, especially new notes after changing parameters.
