# Reference Capture Inventory

## What to Do Now

Keep the approved Phase 0 input noise floor if the guitar, Focusrite input, and
gain have not changed. Then complete the six Phase 1 low-E exercise items and
stop for a full comparison against the current physical model.

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

Stop here. These six files provide independent down/up attacks, natural
direction changes on a moving string, long decay, and the ringing-versus-damped
contrast. The next inventory will be designed only after this set is compared
against the current model.

## Consistency Rules

- Record clean DI without amp, cabinet, compression, gate, denoiser, or effects.
- Keep the current 44.1 kHz ASIO rate for this low-E comparison unless you
  intentionally choose to restart the entire six-item set at another rate.
  Keep peaks near or below -12 dBFS.
- Enable only the intended Focusrite input; the MVP records the first enabled
  input channel.
- Keep guitar volume/tone, pickup, interface gain, and picking location fixed
  within a comparison group.
- Add exact guitar, pickup, pick, and any performance caveats to approved-take
  notes.
- Preserve silence around gestures so onset alignment and noise measurement are
  possible.
- Reject bumps, cable handling, accidental harmonics, wrong strings, and obvious
  performance outliers. Delete a rejected take when it has no research value.
- Treat damped-versus-ringing subtraction as a diagnostic residual, not pure
  isolated pick audio.
