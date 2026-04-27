# Plan 0053 — Tabbed Editor UI

## Goal

Reduce the plugin editor's vertical footprint by replacing the tall collapsible section stack with tabbed pages.

## Scope

- Keep all current parameter controls and attachments.
- Convert the existing section buttons into page tabs.
- Show one parameter group at a time.
- Use a fixed compact editor height.

## Out of Scope

- DSP changes.
- New parameters.
- Full visual redesign.
- Preset browser or advanced layout system.

## Verification

- Build `GuitarAG_VST3`.
- Install the VST3.
- Open in Bitwig and verify each tab exposes the expected controls.
