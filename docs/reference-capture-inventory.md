# Reference Capture Inventory

## What to Do Now

Complete only Phase 0 and Phase 1, then stop and ask Codex to analyze the
approved sessions. That is seven checklist items. Later phases are a roadmap,
not homework, and may change after the first real measurements.

For each ordinary pick item, one take is a batch of eight isolated strokes.
Record up to three batches, listen back, and approve at least one representative
batch. Approve more than one when both are clean and the natural variation is
useful.

Press Space to start recording and Space again to stop, so both hands can stay
near the guitar. Space remains normal text while editing take notes.

The capture app loads this inventory automatically from:

```text
Documents\Guitar AG Reference Captures\capture-inventory.json
```

Its inventory selector shows `[done]` after an item has the required approved
take. It opens the first unfinished item on launch.

## Phase 0 — Setup Reference

- [ ] Input noise floor: two opportunities to capture 10 untouched seconds;
      approve one clean take with the guitar connected and settings fixed.

## Phase 1 — Minimum Pick-Contact Baseline

Use one ordinary medium plastic pick, medium downstrokes, shallow pick depth,
and fixed guitar/pickup/interface settings throughout.

- [ ] Open low E — ringing
- [ ] Open low E — fretting-hand damped at non-harmonic touch points
- [ ] Open low E — foam/cloth damped near the fretboard, away from the pick
- [ ] Open high E — ringing
- [ ] Open high E — fretting-hand damped at non-harmonic touch points
- [ ] Open high E — foam/cloth damped near the fretboard, away from the pick

Stop here. These six conditions tell us whether the proposed comparison can
separate useful contact, register, and string-response traits before collecting
a larger matrix.

## Phase 2 — Direction, Force, Depth, and Middle Strings

Do this only after Phase 1 analysis confirms the workflow.

- [ ] Open D — medium downstroke, shallow, ringing
- [ ] Open B — medium downstroke, shallow, ringing
- [ ] Open low E — medium upstroke, shallow, ringing
- [ ] Open high E — medium upstroke, shallow, ringing
- [ ] Open low E — light downstroke, shallow, ringing
- [ ] Open low E — hard downstroke, shallow, ringing
- [ ] Open high E — light downstroke, shallow, ringing
- [ ] Open high E — hard downstroke, shallow, ringing
- [ ] Open low E — medium downstroke, deep, ringing
- [ ] Open high E — medium downstroke, deep, ringing

## Phase 3 — Flexible Versus Stiff Picks

- [ ] Flexible/thin pick — low E ringing
- [ ] Flexible/thin pick — high E ringing
- [ ] Flexible/thin pick — low E hand-damped
- [ ] Stiff/thick pick — low E ringing
- [ ] Stiff/thick pick — high E ringing
- [ ] Stiff/thick pick — low E hand-damped

Write the exact pick brand/material/thickness in the approved take notes when
known. Relative contrast matters more than owning a particular pick.

## Phase 4 — Finger and Surface Gestures

- [ ] Finger-flesh attack — low E
- [ ] Finger-flesh attack — high E
- [ ] Fingernail attack — low E
- [ ] Fingernail attack — high E
- [ ] Wound D-string five-fret slide upward
- [ ] Wound D-string five-fret slide downward
- [ ] Wound D-string picked note followed by a natural fretting-finger lift

These seed later finger-excitation, friction, slide, and release models. They
should not be collapsed into a generic noise layer.

## Consistency Rules

- Record clean DI without amp, cabinet, compression, gate, denoiser, or effects.
- Prefer 48 or 96 kHz and keep peaks near or below -12 dBFS.
- Enable only the intended Focusrite input; the MVP records the first enabled
  input channel.
- Keep guitar volume/tone, pickup, interface gain, and picking location fixed
  within a comparison group.
- Add exact guitar, pickup, pick, and any performance caveats to approved-take
  notes.
- Preserve silence around gestures so onset alignment and noise measurement are
  possible.
- Reject bumps, cable handling, accidental harmonics, wrong strings, and obvious
  performance outliers without deleting their files.
- Treat damped-versus-ringing subtraction as a diagnostic residual, not pure
  isolated pick audio.
