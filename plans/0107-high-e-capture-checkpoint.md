# Plan 0107 — High-E Capture Checkpoint

## Goal

Extend the accepted low-E reference work to one opposite endpoint without asking
the player to fill every string before the model's register scaling is tested.

## Confirmed/Estimated Setup

- guitar: EVH Wolfgang Special;
- pickup: neck humbucker;
- picking location: halfway between bridge and neck pickups;
- strings: likely a .009-gauge set, exact brand/gauges unknown;
- string condition: installed a long time and somewhat grimy, but not heavily
  played or obviously fatigued;
- pick: generic medium-gauge celluloid-style plastic, exact composition and
  thickness unknown, well worn/broken in.

The uncertainty is part of the metadata and must not be rewritten as a measured
specification.

## Capture Boundary

- Retain the existing noise-floor and six completed low-E items.
- Add only open-high-E ringing independent down, independent up, and continuous
  down-first alternate exercises.
- Use 6/6/12 strokes, natural timing, and one required approved batch per item;
  a second batch is optional.
- Keep the established pickup, controls, Focusrite gain, pick, medium force,
  shallow depth, and picking location fixed.
- Do not add damping, A/D/G/B strings, new dynamics, or new pick materials yet.

## Verification

- [x] Generator emits 10 unique items across three phases with the three new
      high-E requests last.
- [x] Every high-E request carries the setup metadata and its uncertainty.
- [x] High-E exercises request 6, 6, and 12 strokes with one approved batch
      required and an optional second batch.
- [x] External inventory regeneration changes none of the 43 existing session
      files.
- [x] Existing approvals resolve the first incomplete item to high-E ringing
      independent downstrokes.
- [ ] Complete the three high-E items, then stop for endpoint model comparison.
