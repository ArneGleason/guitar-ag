# Studio Handoff: Antigravity Baseline Code Review Findings

- Timestamp: 2026-05-09T13:35:25-04:00
- Status: handoff
- Branch: `main`
- To: `guitar-ag lead`
- From: `guitar-ag reviewer`
- Subject: Return baseline review findings to Codex for routing
- To agent: `Codex`
- To machine: `macbook-pro-m5`
- From agent: `Antigravity`
- From machine: `mac-mini-pro-m4`

## Human Context

The reviewer agent (Antigravity on mac-mini-pro-m4) has completed the static baseline code review. No code changes were made during this pass. Please read the findings below and decide which items to fix immediately and which to defer to the upcoming optimization pass.

## Findings

- **[P1] Lookahead Delay Desyncs Expression and Notes**
  - **File:** `src/dsp/AudioEngine.cpp:717` (`AudioEngine::handleIncomingMidiMessage`)
  - **Problem:** Pitch Wheel, Mod Wheel (CC1), Timbre (CC74), and Channel Pressure are processed immediately and update their target values. However, Note On/Off and Aftertouch events are delayed by `lookaheadSamples` via the `scheduleMidiMessage` function. This causes continuous expression events to immediately desync from the notes they belong to when lookahead is enabled (e.g., 150ms or 250ms delay).
  - **Suggested fix:** Schedule continuous controllers in the delay queue alongside Note On/Off/Aftertouch, so they are applied exactly when the delayed note plays.

- **[P1] Extreme CPU Usage from Per-Sample `std::pow` and `std::sin`**
  - **File:** `src/dsp/StringVoice.cpp:753` (`StringVoice::renderSample`)
  - **Problem:** `std::pow` and `std::sin` are evaluated every single sample, for every active voice, to calculate `aftertouchRatio`, `mpePitchRatio`, `vibratoRatio`, and `whammyRatio`. This is expensive on the audio thread and may lead to audio dropouts during polyphonic play. Furthermore, `std::tanh` is called per-sample in `AudioEngine::updateAmpFeedbackLoop`.
  - **Suggested fix:** Calculate pitch modulation ratios at block rate (or every N samples), or use fast approximations for `pow2`, `sin`, and `tanh`.

- **[P2] Incomplete MPE State Reset**
  - **File:** `src/dsp/AudioEngine.cpp:238` (`AudioEngine::setMpeEnabled`)
  - **Problem:** When MPE mode is disabled, `mpePitchBendByChannel` is correctly filled with `0.0f` and voices are reset. However, `mpePressureByChannel` and `mpeTimbreByChannel` are left at their last values. If MPE is later re-enabled, stale pressure and timbre values may unexpectedly snap into effect.
  - **Suggested fix:** Add `mpePressureByChannel.fill(0.0f)` and `mpeTimbreByChannel.fill(0.0f)` inside the `!mpeEnabled` block.

- **[P3] Non-MPE CC74 and Channel Pressure Routing**
  - **File:** `src/dsp/AudioEngine.cpp:779` (`AudioEngine::handleMidiMessage`)
  - **Problem:** Channel Pressure and CC74 are strictly routed via `applyMpePressure` and `applyMpeTimbre`, even when MPE mode is off. In standard MIDI mode, this means they operate per-channel instead of globally. While this technically functions if standard MIDI rests entirely on Channel 1, it breaks standard MIDI expectations if a DAW transmits CCs globally while notes are mapped to other channels.
  - **Suggested fix:** If `!mpeEnabled`, apply continuous controllers globally to all voices, similar to the fallback logic already implemented for the pitch wheel.

## Checks Run

- Read `src/plugin/PluginProcessor.cpp` and `src/dsp/AudioEngine.cpp` for audio thread safety and DSP/UI boundary clarity.
- Read `src/dsp/StringVoice.cpp` for real-time DSP performance bottlenecks.
- Read `src/plugin/PluginEditor.cpp` for UI/DSP separation.
- Verified MPE lower-zone master channel logic for pitch bend.

## Checks Not Run

- Building the project (`cmake` / `make`) was not run because the JUCE dependency folder (`/Users/arnegleason/code/JUCE`) was not found locally on this machine.
- Comprehensive end-to-end testing with a DAW (requires a compiled VST3).
- Offline rendering calibration validation (requires a successful build).

## Open Questions

- For the Lookahead feature, should the delay apply to the entire audio block processing (i.e. delaying the output buffer itself), instead of trying to schedule individual MIDI events? Delaying audio would guarantee perfect sync of all events natively with much less complexity.
- For the `std::pow` performance risk, do we want to implement a custom fast math library, or simply switch to block-rate (or sub-block rate) updates?

## Suggested Next Routing

- **Fix Now (Codex):**
  - The Lookahead Delay Desync bug ([P1]).
  - The Incomplete MPE State Reset bug ([P2]).
  - The Non-MPE CC74/Pressure Routing logic ([P3]).
- **Wait for Optimization Pass:**
  - The per-sample `std::pow` / CPU usage ([P1]), as this falls strictly under the future performance/maintainability optimization pass.
