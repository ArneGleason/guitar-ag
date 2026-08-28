# Plan 0100 Guided Capture Inventory Handoff

## Outcome

Added the missing human-facing capture queue. A generator creates 30 stable
requests across five phases and one inventory JSON in the operating system's
Documents known folder. The capture app discovers it automatically, displays
approval-derived progress, selects the first incomplete item, and includes the
research reason in the loaded request context.

The intended first handoff is deliberately only seven items: Phase 0 input
noise floor and six Phase 1 low/high E ringing, hand-damped, and foam-damped
conditions. Stop there for analysis before collecting direction, force, depth,
pick-material, or finger/surface phases.

Implementation commit: `3566523`.
Space-transport follow-up: `95bce64`.

## Verification Completed

- Windows Release `GuitarAGReferenceCapture` builds.
- Python request/session/inventory suite passes three tests.
- Inventory generation produces 30 unique request IDs across five phases and
  refuses replacement without `--force`.
- Windows default output resolves to the current OneDrive-redirected Documents
  known folder.
- App launch without arguments discovers the real inventory and creates/resumes
  `setup-01-input-noise-floor`.
- Runtime smoke with a seeded approved Phase 0 take advances to
  `pick-baseline-low-e-ringing` and preserves the approval.
- Inventory startup no longer creates an unused manual-session folder.
- The app rebuilds with a foreground-only Space record/stop toggle. Space is
  ignored by capture transport while take notes, Audio Settings, or file
  choosers are active, and while another application is foreground.
- `git diff --check` reports only expected Windows line-ending warnings.

## Human Gate

Launch the capture app and inspect the inventory selector, progress line,
instructions, reason, and take controls at normal UI size. Capture and approve:

1. input noise floor;
2. low E ringing;
3. low E hand-damped;
4. low E foam-damped;
5. high E ringing;
6. high E hand-damped;
7. high E foam-damped.

Then stop and ask Codex to validate/analyze the seven sessions. Do not begin
Phase 2 until that analysis confirms or revises the remaining inventory.

Also verify Space starts/stops cleanly from the main capture surface and remains
a normal typing key inside take notes.

## Existing External Data Note

The local capture root contains one pre-inventory manual candidate WAV of about
4.42 seconds plus several empty manual smoke-test sessions. They were left
untouched because the candidate may be human-created. They are not counted by
the guided inventory.
