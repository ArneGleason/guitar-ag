# Studio Handoff: Human Feedback on EG-065 Slide Tail

- Timestamp: 2026-05-09T21:38:04-04:00
- Status: human-feedback
- Branch: `main`
- To: `guitar-ag lead`
- From: `human operator`
- Subject: Replace note-off slide tails with slide lift pressure envelope
- To agent: `Codex`
- To machine: `macbook-pro-m5`
- From agent: `Human`
- From machine: `macbook-pro-m5`

## Human Context

The human operator auditioned EG-065 in Bitwig. The `Slide Tail` modes did not feel convincing, and note-off anchoring appears unreliable around loop playback. A Bitwig loop with a short gap before restart showed a large CPU spike at the loop boundary.

## Feedback

- `Slide Tail`/muted and `Open` note-off modes should be removed.
- Normal slide behavior is acceptable enough to write around.
- The better model is a continuous lift of fretting pressure during the slide, not a special release color at note-off.
- Replace the `Slide Tail` choice with a `Slide Lift` parameter.

## Requested Behavior

- `Slide Lift` at 0%: no lift; full fretted slide pressure remains.
- `Slide Lift` at 100%: lift quickly, around 50 ms.
- `Slide Lift` around 50%: lift gradually, around half a second.
- As lift increases, fret engagement should fade and the sound should move toward lighter finger-on-string scrape plus progressive damping/muting.
- This should be driven by slide motion rather than note end.

## Routing

Codex should implement EG-066 `SlideLift`, remove `Slide Tail`, update UI info notes, build/install the VST3, and route the result for review/audition.
