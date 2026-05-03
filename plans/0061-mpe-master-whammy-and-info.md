# 0061 — MPE Master Whammy And Info Copy

Date: 2026-05-03

## Goal

Keep global whammy usable while `MPE Mode` is enabled, and make the in-plugin info popovers clearer for both casual and technical users.

## Context

The first MPE pitch-bend implementation routed every pitch-wheel message to per-channel MPE bend when `MPE Mode` was enabled. That protected independent note bends, but it also meant `Pitch Wheel Whammy` stopped working in MPE mode.

The editor already had clickable info buttons, but several blurbs were short implementation notes rather than friendly first-pass explanations.

## Plan

1. Treat lower-zone channel 1 pitch wheel as master/global whammy when `MPE Mode` is enabled.
2. Keep member-channel pitch wheel on channels 2-16 routed to per-note MPE bend.
3. Update the MPE and whammy info text so the channel split is visible in the plugin.
4. Refresh existing info popovers to use a plain-language first sentence plus a short technical detail.
5. Update docs and manual test notes.

## Result

- `MPE Mode` no longer disables global whammy outright.
- In lower-zone MPE, channel 1 pitch wheel drives the whammy path.
- Member-channel pitch wheel still drives independent MPE bend.
- Info popovers now expose more of the model behavior without requiring external docs.

## Manual Verification

- In Bitwig with MPE enabled, draw or perform a member-channel note bend and confirm only that note bends.
- Send pitch wheel on channel 1 and confirm all active notes move through the whammy path.
- Confirm `Pitch Wheel Whammy` Off disables the global whammy path.
- Open the MPE and Whammy page info buttons and confirm the behavior is understandable from inside the VST.
