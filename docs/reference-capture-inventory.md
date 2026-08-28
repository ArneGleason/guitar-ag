# Reference Capture Inventory

## What to Do Now

The approved Phase 0 input noise floor and all six Phase 1 low-E exercises are
complete. Now record only the three Phase 2 open-high-E ringing exercises and
stop for a low/high endpoint comparison. Do not fill A, D, G, or B yet.

One take means one WAV containing the complete instructed batch. Approve one
representative batch per item. A second take is only a retry or an intentionally
preserved example of useful variation; it is not required.

Timing should feel natural. Do not use a metronome. Leave about one second of
untouched input at the start, and take as long as needed to move a hand to or
from Space. Event detection will find the actual strokes.

Press Space to start recording and Space again to stop, so both hands can stay
near the guitar. Space remains normal text while editing take notes.

The capture app loads this inventory automatically from:

```text
Documents\Guitar AG Reference Captures\capture-inventory.json
```

Its inventory selector shows `[done]` after an item has the required approved
take. It opens the first unfinished item on launch.

## Phase 0 — Setup Reference

- [x] Input noise floor: two opportunities to capture 10 untouched seconds;
      approve one clean take with the guitar connected and settings fixed.

## Phase 1 — Low-E Model-Evaluation Exercise

Use open low E, one ordinary medium plastic pick, medium force, shallow pick
depth, and fixed guitar/pickup/interface settings throughout.

### Ringing

- [x] Independent downstrokes: four strokes. Let each of the first three ring
      for 1.5–2 seconds, deliberately stop it, then leave about 0.5 seconds of
      quiet reset time. Let the fourth ring naturally for 6–8 seconds.
- [x] Independent upstrokes: use the same four-stroke/reset/final-decay pattern.
- [x] Alternate repicking: play 12 continuous strokes at a comfortable natural
      tempo, starting down (`D U D U D U D U D U D U`). Do not stop the string
      between strokes. Let the final event ring for 4–6 seconds.

### Hand-damped

Touch the string at stable non-harmonic fretting-hand positions, away from the
pick, so sustained pitch is suppressed without moving the picking location.

- [x] Downstrokes: six strokes, roughly 0.6-0.8 seconds apart.
- [x] Upstrokes: six strokes, roughly 0.6-0.8 seconds apart.
- [x] Alternate picking: 12 continuous strokes at a comfortable natural tempo,
      starting down (`D U D U D U D U D U D U`).

These six files supplied independent down/up attacks, natural direction changes
on a moving string, long decay, and the ringing-versus-damped contrast. Plan
0105's candidate and Plan 0106's de-click passed human listening.

Recorded-set context: every completed take used the neck humbucker and a picking
position halfway between bridge and neck. All damped takes used fretting-hand
damping.

## Phase 2 — High-E Endpoint Exercise

Use open high E with the same guitar, neck humbucker, controls, Focusrite gain,
picking location, medium force, shallow depth, and physical pick. Do not change
gain simply to make the waveform as large as low E. If gain must change for a
usable signal-to-noise ratio, document it in selected-take notes.

- [ ] Independent downstrokes: six strokes. Let each of the first five ring for
      1.5–2 seconds, deliberately stop it, then leave about 0.5 seconds of quiet
      reset. Let the sixth ring naturally for 5–8 seconds.
- [ ] Independent upstrokes: use the same six-stroke/reset/final-decay pattern.
- [ ] Alternate repicking: play 12 continuous strokes at a comfortable natural
      tempo, starting down (`D U D U D U D U D U D U`). Do not stop the string
      between strokes. Let the final event ring for 4–6 seconds.

Approve one representative batch per item. A second batch is optional. Stop
after these three items; high E is the opposite endpoint used to decide how the
model should scale before D/G or interpolation checks are requested.

## Recorded Setup Profile

- Guitar: EVH Wolfgang Special.
- Pickup: neck humbucker.
- Picking location: halfway between bridge and neck pickups.
- Strings: likely approximately .009 gauge; exact brand/gauges unknown.
- Condition: installed a long time and somewhat grimy from finger contact, but
  not heavily played or obviously fatigued.
- Pick: generic medium-gauge celluloid-style plastic, exact brand/composition/
  thickness unknown, well worn and broken in.

The estimated fields are intentionally not treated as measured specifications.

## Consistency Rules

- Record clean DI without amp, cabinet, compression, gate, denoiser, or effects.
- Keep the current 44.1 kHz ASIO rate for endpoint comparison. Keep peaks near
  or below -12 dBFS.
- Enable only the intended Focusrite input; the MVP records the first enabled
  input channel.
- Keep guitar volume/tone, pickup, interface gain, and picking location fixed
  within a comparison group.
- Add any setup exception or performance caveat to approved-take notes; the
  known guitar/pick/string description is already embedded in each request.
- Preserve silence around gestures so onset alignment and noise measurement are
  possible.
- Reject bumps, cable handling, accidental harmonics, wrong strings, and obvious
  performance outliers. Delete a rejected take when it has no research value.
- Treat damped-versus-ringing subtraction as a diagnostic residual, not pure
  isolated pick audio.
