# Plan 0045: Pickup Model

## Goal

Add a first electric-guitar pickup model with one active pickup at a time.

## Research Notes

- Pickup position acts as a spatial harmonic filter: each mode contributes according to its mode shape at the pickup readout point.
- A point readout near the bridge emphasizes higher harmonics because the fundamental has little displacement near the string end.
- Moving the readout toward the neck increases low/fundamental contribution, but it also creates harmonic-specific peaks and nulls rather than a simple linear tone control.
- A wider pickup aperture averages string motion over a larger region, which smooths/dulls high partials.
- A humbucker can be approximated as two nearby readout points. Normal humbucker wiring sums them; an out-of-phase variant subtracts them for a thinner, notched response.

## Change

- Label this pass `StringVoice EG-038 PickupModel`.
- Add a `Pickup Model` choice parameter:
  - `Single Coil`
  - `Humbucker`
  - `Humbucker OOP`
- Add a `Pickup Position` parameter from bridge-side to neck-side.
- Map the pickup position to a normalized string readout range from about 0.055 to 0.335 of the speaking length measured from the bridge.
- Add UI markers at 1/6, 1/5, 1/4, and 1/3 of the speaking length.
- Keep the model to one pickup at a time.

## Verification

- Build the offline renderer and VST3.
- Render bridge/neck pickup-position diagnostics.
- Render the three pickup model choices at the default position.
- Install the VST3 for Bitwig audition.
