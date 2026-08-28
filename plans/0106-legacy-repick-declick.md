# Plan 0106 — Legacy Repick De-click

## Goal

Remove the tick heard in Plan 0105 alternate picking without changing the
accepted low-E geometry/decay candidate or hiding a legitimate pick attack.

## Diagnosis Boundary

- Treat the human report as a restart artifact hypothesis and measure the exact
  note-on neighborhood before changing DSP.
- Keep the accepted 0.14 pluck position / 1.50x body-decay candidate fixed.
- Ablate existing direct attack/contact layers before attributing the tick.
- Prototype the smallest phase-continuous transition offline before any VST3
  promotion.

## Candidate

At a same-string legacy restart, copy the outgoing voice into an offline tail
slot and fade that still-running modal state while the restarted voice follows
its existing attack ramp. This preserves the outgoing waveform at the boundary
without retaining long overlapping strings or allocating on the audio thread.

## Verification

- [x] Quantify note-on discontinuity/spike behavior in the accepted alternate
      render and its layer ablations.
- [x] Default offline output remains byte-identical with repick crossfade off.
- [x] Crossfade candidates are deterministic and bounded.
- [x] Windows Release offline renderer and VST3 build.
- [x] Produce a loudness-matched control/candidate alternate-picking A/B.
- [x] Require human confirmation that the tick is reduced without smearing the
      legitimate pick attack before promotion.

Human verdict: “Excellent declick.” Production transfer remains a separate
change, coordinated with the accepted low-E candidate and endpoint scaling.
