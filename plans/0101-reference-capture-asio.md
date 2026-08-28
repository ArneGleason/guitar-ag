# Plan 0101 — Reference Capture ASIO

## Goal

Let the Windows reference-capture companion use the installed Focusrite USB
ASIO driver, avoiding the shared/exclusive Windows Audio path that produced a
confirmed input dropout during the first real capture.

## Scope

- Enable JUCE ASIO only for the Windows `GuitarAGReferenceCapture` target.
- Keep the VST3 and offline renderer build definitions unchanged.
- Document `ASIO` / `Focusrite USB ASIO` as the preferred Windows capture path.
- Verify the app compiles against JUCE's bundled ASIO headers and that the
  current machine registers the Focusrite driver for 64-bit applications.

## Constraints

- The Focusrite driver remains an external dependency; do not redistribute it.
- Enabling ASIO uses the ASIO SDK-derived headers bundled with JUCE and remains
  subject to their applicable license terms.
- Do not force a driver in code. Preserve Audio Settings so another compatible
  interface or a Windows Audio fallback can still be selected.
- ASIO does not make concurrent audio clients safe. Close dictation, DAWs, and
  other microphone clients for the controlled capture test.

## Verification

- [x] Reconfigure the Visual Studio x64 build after changing compile definitions.
- [x] Build the Windows Release `GuitarAGReferenceCapture` target.
- [x] Confirm the compiled target contains ASIO backend symbols or strings.
- [ ] Human selects `ASIO` / `Focusrite USB ASIO` and records a clean diagnostic
      take before resuming the Phase 0/1 inventory.
