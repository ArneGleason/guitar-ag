# Plan 0101 Windows ASIO Reference Capture Handoff

## Outcome

Enabled JUCE ASIO support only for the Windows `GuitarAGReferenceCapture`
target. The existing `Focusrite USB ASIO` registration can now appear in the
app's Audio Settings device-type/device selectors. Windows Audio backends remain
available as fallbacks, and neither the VST3 nor offline renderer received the
ASIO compile definition.

Implementation commit: `0a66264`.

## Evidence and Verification

- Reconfigured the Visual Studio 2022 x64 build with the local JUCE checkout.
- Built the Release `GuitarAGReferenceCapture` executable successfully.
- Confirmed `JUCE_ASIO=1` in the capture target's generated Release project.
- Confirmed the executable contains JUCE `ASIOAudioIODevice` and
  `ASIOAudioIODeviceType` symbols/strings.
- Confirmed the 64-bit Windows registry exposes `Focusrite USB ASIO`.
- Confirmed the VST3 and offline-renderer generated projects do not contain the
  ASIO compile definition.
- Ran the three reference-capture Python workflow tests successfully.
- Launched and closed the rebuilt app successfully without opening the ASIO
  driver while Codex dictation could still be using audio.

## Human Gate

1. Finish dictation and fully close Codex/ChatGPT, DAWs, browsers using a mic,
   conferencing apps, and other audio clients.
2. Launch the rebuilt capture executable.
3. In Audio Settings select `ASIO`, then `Focusrite USB ASIO`.
4. Enable only the guitar input and choose 48 kHz with a 1024-sample diagnostic
   buffer.
5. Record a ten-second noise floor and several isolated picks.
6. Audition the WAV for clicks or gaps, then restart the app and confirm the
   ASIO configuration is restored.

Do not resume the full inventory until this diagnostic recording is clean. The
existing writer-overflow field cannot detect device-layer discontinuities: the
first Windows Audio take contained about 8 ms of exact zeros while reporting no
writer overflow.
