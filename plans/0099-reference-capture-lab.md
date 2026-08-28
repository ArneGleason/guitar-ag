# Plan 0099 — Interactive Reference Capture Lab

## Goal

Build a small desktop companion that lets the human record clean DI reference
takes in response to a structured Guitar AG research request, audition the
takes, and approve one or more for later analysis.

## Scope

- Add a JUCE desktop app with audio-device selection suitable for a Focusrite
  interface.
- Record the first enabled input channel as mono 24-bit WAV at the device sample
  rate.
- Show input level and headroom guidance while recording.
- Load a JSON capture request containing instructions and structured context.
- Keep multiple takes, audition them, and mark each candidate, approved, or
  rejected without deleting audio.
- Write a machine-readable session manifest beside the external WAV files.
- Add a dependency-free request generator so an agent can prepare the next
  focused recording brief.

## Safety Boundary

- Captures are external analysis references, not playback assets for the VST3.
- Do not commit captured audio or generated session manifests.
- Do not claim that damped/ringing subtraction perfectly isolates pick contact.
- Do not place recording, file I/O, or companion-app state in the plugin audio
  path.

## Request/Approval Loop

1. An agent creates one focused request JSON, normally for a comparison group
   such as ringing, hand-damped, and foam-damped versions of the same stroke.
2. The human loads the request, enables only the intended Focusrite guitar
   input, and records as many takes as needed.
3. The human auditions takes and marks one or more approved.
4. The companion app updates `session.json`; the agent can then analyze only
   approved takes while retaining rejected takes as useful listening evidence.

## Verification

- [ ] Windows Release target builds.
- [ ] App opens and lists the available audio device types and channels.
- [ ] A generated request loads and creates an external session folder.
- [ ] Recording writes a mono 24-bit WAV at the active device sample rate.
- [ ] Multiple takes can be selected, played, approved, and rejected.
- [ ] `session.json` stays valid after each state change.
- [ ] Human records and approves the first real Focusrite capture set.
