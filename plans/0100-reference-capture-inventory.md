# Plan 0100 — Guided Reference Capture Inventory

> Superseded for current capture work by Plan 0103's focused low-E evaluation
> exercise. This file preserves the original inventory decision and history.

## Goal

Turn the single-request recorder into a guided checklist so the human always
knows what to capture next, why it matters, and when to stop for analysis.

## Scope

- Define a finite, phased starter inventory rather than an exhaustive factorial
  matrix.
- Put the smallest useful pick-contact baseline first.
- Add later phases for direction/dynamics/depth, pick-material comparison, and
  finger/contact gestures.
- Generate stable request files and one inventory JSON outside Git.
- Let the app load the inventory, show completion state, and open a selected
  request without file browsing.
- Treat an item as complete after its required number of approved take batches.
- Let Space start and stop a take while the app is foreground, except during
  note entry or Audio Settings interaction.

## Human Workflow

Complete Phase 0 and Phase 1 first, then stop and ask for analysis. Do not spend
hours completing later phases before the first approved baseline proves that the
capture method and measurements are useful.

Each ordinary pick item requests three candidate take batches. Each batch
contains eight isolated strokes. Approve at least one representative batch;
approve more when repeat-to-repeat variation is genuinely useful.

## Safety Boundary

- Keep captures outside Git and outside the plugin.
- Preserve rejected takes and session history.
- Do not treat the inventory as a demand to complete every item immediately.
- Do not interpret muted/ringing subtraction as a pure isolated pick source.

## Verification

- [x] Inventory generator creates stable requests and a valid inventory file.
- [x] Capture app automatically discovers the default inventory.
- [x] Selecting an inventory item loads its request and existing session.
- [x] Approving a take updates the inventory progress without restarting.
- [x] Completed items remain complete after app restart.
- [ ] Human completes Phase 0 and Phase 1, then hands the sessions back for
      analysis before proceeding further.
