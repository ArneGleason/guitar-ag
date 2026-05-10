# 0075 — Plastic Pick Bite

## Context

Human DAW audition of EG-072 found that `Pick Stroke = Alternate` helped repeated-note realism, but the isolated attack still suggested a substantial brittle/glassy pick rather than a normal plastic plectrum. The human also asked for an adjustable strength control while keeping the physical-model mental model: the pickup should be understood as hearing string motion caused by player/string interaction, not a separate pick-noise sound layered on top.

## Physical Mental Model

For picked notes, the useful abstraction is:

1. The pick displaces the string.
2. The pick edge remains in contact for a short time, with compliance and surface friction.
3. Release imprints a kink, transverse polarization, short scrape, and some high-mode energy into the string.
4. The string then rings according to speaking length, pickup position, string age, and damping.
5. The pickup captures the resulting string motion.

This means a "pick noise amount" is not literally a post-mix volume. It should scale the contact imprint: edge bite, scrape, short contact ring, and attack chirp.

## Implemented Scope

EG-073 implements:

- New host/APVTS parameter: `Pick Bite`, default 50%.
- Articulation-page `Pick Bite` slider and info note.
- Offline-renderer `--pick-bite`.
- Wider deterministic per-attack variation through pick-depth and pick-angle scatter.
- Less brittle default pick-contact behavior:
  - rough contact starts later in the `Pick Texture` range;
  - local pick contact is broader/complier;
  - contact ring is lower, shorter, and less high pitched;
  - chirp modes are lower, shorter, and more strongly gated by `Pick Bite`.
- Stronger `String Age` coupling into pick contact:
  - less coherent bright ring/chirp;
  - shorter bright contact decay;
  - lower contact high-pass coefficient for older-string scrape.

## Verification

Local checks run on 2026-05-10:

- `scripts/install-vst3.sh --build`
- Single-note renders:
  - default `--pick-bite 0.5`
  - repeated default render
  - low bite `--pick-bite 0.15`
  - high bite `--pick-bite 1.0`
  - aged strings `--string-age 0.5`
- Repeated default render was byte-identical:
  - `/private/tmp/guitar-ag-eg073-default-a.wav`
  - `/private/tmp/guitar-ag-eg073-default-b.wav`
- First-attack 20 ms relative RMS differences vs default:
  - low bite: `0.0142`
  - high bite: `0.0257`
  - string age 50%: `0.3513`
- Full-render relative RMS differences vs default:
  - low bite: `0.003613`
  - high bite: `0.006481`
  - string age 50%: `0.353870`
- Larger audition renders completed:
  - `tests/midi/guitar-ag-player-articulation-audition.mid`: `36.688x` realtime, max 4 string voices
  - `tests/midi/guitar-ag-feature-audition.mid`: `31.037x` realtime, max 6 string voices

## Human Audition Targets

- Try `Pick Bite` around 20% to 60% with `String Age` around 50%.
- Confirm the default no longer suggests a chunky glass pick.
- Confirm `Pick Bite` feels like contact strength, not a disconnected click volume.
- Confirm older strings make the pick attack less brand-new and brittle.
- Confirm `Pick Stroke = Alternate` still adds natural variation without over-randomizing the line.

## Deferred

- Pick thickness/material presets.
- Fingerpicking and fingernail attack families.
- Explicit pick depth or pick angle UI.
- A fuller physical plectrum beam/contact solver.
