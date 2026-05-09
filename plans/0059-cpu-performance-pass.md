# Plan 0059 - CPU Performance Pass

## Goal

Reduce real-time CPU load and CPU spikes without flattening the current modeled-guitar sound.

The current sound quality matters more than making the instrument cheap at any cost. The first pass should preserve the EG-050 tone and feedback behavior, then remove obvious avoidable cost and add measurement so later changes are guided by data.

## Observations

- Local repo is current with `origin/main` at `7c4cd22`.
- The plugin audio path already uses raw APVTS parameter atomics in `processBlock`, fixed arrays in the DSP engine, and `ScopedNoDenormals`.
- The hottest path is likely the per-sample, per-active-voice modal loop in `StringVoice::renderSample`.
- Each active voice currently iterates up to 96 modal slots every sample.
- `AudioEngine::maxVoices` is 8, but a guitar-like allocator should not need more than 6 ringing string voices for the core instrument behavior.
- `Amp Feedback` can keep voices alive much longer, multiplying the cost of the already-expensive modal loop.
- `Lookahead` probably smooths perceived CPU spikes by delaying note starts and spreading note-start work, not by reducing total DSP cost.

## Quick Timing Check

Using `build/GuitarAGOfflineRender_artefacts/Release/GuitarAGOfflineRender` with `tests/midi/guitar-ag-player-articulation-audition.mid`, 48 kHz, block size 512, tail 2 seconds:

- Baseline, no feedback, no lookahead: about 5.56 s wall / 5.20 s user.
- Lookahead 250 ms with finger noise 0.6: about 5.38 s wall / 5.30 s user.
- Amp feedback 100%, no lookahead: about 31.30 s wall / 30.90 s user.
- Amp feedback 100%, lookahead 250 ms with finger noise 0.6: about 31.03 s wall / 30.72 s user.

This strongly suggests the feedback control mainly hurts CPU by extending active voice lifetime, not because the global feedback resonator itself is large.

## Candidate Work

1. Add a small offline performance diagnostic mode.
   - Report total rendered samples, max active voices, average active voices, active voice-samples, and optionally max block render time.
   - Keep it in the offline renderer or a small benchmark tool, not in the release plugin UI.
   - Done in the first measurement pass with `--perf-report`.

2. Cap the core guitar allocator at six string voices.
   - Change the active voice pool from 8 to 6 unless a documented future mode needs more.
   - Steal by physical string/quietest released voice rather than simple round-robin where practical.
   - Preserve MPE voice/channel routing rules.
   - Done for the core pool with same-string reuse before inactive/round-robin selection.

3. Skip inactive and silent modal work more aggressively.
   - Track configured/active modal count per voice instead of always looping over all 96 modal slots.
   - Add a per-mode amplitude cutoff so dead modes stop updating before the whole voice dies.
   - Verify that quiet tails and feedback onset still feel natural.

4. Split normal and feedback render paths.
   - When `Amp Feedback` and the global loop amount are effectively zero, avoid feedback-specific harmonic lock, exp/log2/round work inside the modal loop.
   - Keep the exact current path for nonzero feedback until we can compare renders.

5. Make feedback cheaper without changing the musical behavior.
   - Recompute harmonic/loop target weights at a lower control rate, such as every 16 or 32 samples, with smoothing.
   - Precompute per-mode static values at note start where possible: mode position, high/low weighting, base mode frequency.
   - Keep the dominant-band loop behavior audible; do not reduce it to a flat sustain boost.

6. Move pitch-ratio expensive math to control rate where safe.
   - Vibrato, whammy, aftertouch bend, and MPE bend currently create several `pow` calls per active voice per sample.
   - Use smoothed ratios or low-rate updates for slow modulation, while keeping pitch changes click-free.

7. Verify build configuration on both machines.
   - Confirm the PC VST3 is a Release build, not Debug or RelWithDebInfo.
   - Compare Mac and PC at the same sample rate, buffer size, voice count, feedback value, and DAW quality mode.

## Suggested Early Milestones

1. Measurement-only diagnostic pass.
2. Six-string voice cap and string-aware stealing.

This split keeps the first code change behavior-neutral, then lets the six-string cap be tested with better numbers. After that, audition the same MIDI file at feedback 0%, 50%, and 100% before changing modal math.

## 2026-05-03 Measurement Pass

Added an opt-in `AudioEngine::PerformanceStats` sink and a `GuitarAGOfflineRender --perf-report` flag.

The release plugin does not print or time blocks. The offline renderer now reports rendered seconds, total render time, realtime factor, average/max block render time, average/max active string voices, string voice-samples, and finger-noise voice activity.

Validation renders on the player-articulation audition MIDI:

- Feedback 0%, lookahead off, finger noise 0%: avg string voices 1.913, max 5, 5,621,281 string voice-samples, 11.210x realtime.
- Feedback 100%, lookahead off, finger noise 0%: avg string voices 7.591, max 8, 22,307,562 string voice-samples, 1.937x realtime.
- Feedback 0%, lookahead 250 ms, finger noise 60%: avg string voices 1.903, max 5, avg finger-noise voices 0.467, max 3, 11.306x realtime.

The `--perf-report` baseline WAV matched the no-report baseline WAV byte-for-byte.

## 2026-05-03 Six-String Cap Pass

Changed the core `AudioEngine` voice pool from eight to six voices and updated note-on allocation to select by assigned physical string first.

Implementation notes:

- `StringVoice` exposes its assigned string index for allocator decisions.
- New notes reuse a still-active voice on the same assigned string before using an inactive voice.
- The old round-robin fallback remains for unexpected all-active/no-matching-string cases.
- MPE pitch bend, pressure, and CC74 state are still applied to the newly selected voice after note start.
- The visible model label is now `StringVoice EG-051 SixStringCap`.
- The cap is specifically for physical string voices. A future pickup/body microphonic source should be a separate non-string auxiliary voice or bus, not a reason to raise the string allocator above six.

Validation renders after the cap:

- Player-articulation MIDI, feedback 0%, lookahead off, finger noise 0%: avg string voices 1.171, max 3, 3,441,827 string voice-samples, 17.847x realtime.
- Player-articulation MIDI, feedback 100%, lookahead off, finger noise 0%: avg string voices 5.191, max 6, 15,255,851 string voice-samples, 2.835x realtime.
- Player-articulation MIDI, feedback 0%, lookahead 250 ms, finger noise 60%: avg string voices 1.189, max 4, avg finger-noise voices 0.467, max 3, 17.300x realtime.
- Feature-audition MIDI with MPE mode enabled: avg string voices 2.063, max 6, 10.394x realtime.

Compared with the 8-voice measurement pass, the high-feedback player-articulation render fell from 22,307,562 to 15,255,851 string voice-samples, and render speed improved from 1.937x to 2.835x realtime.

## 2026-05-03 Passive Fast Path

Added a transparent optimization pass after EG-051.

Implementation notes:

- `StringVoice` now tracks `activeModalCount` and renders only configured modal slots instead of all 96 slots.
- Static per-mode weights and frequencies are precomputed in `configureMode`.
- The neutral-pitch path reuses precomputed sine/cosine phase steps.
- Exactly inactive vibrato, whammy, aftertouch bend, MPE bend, and amp-feedback cases skip their expensive ratio or feedback math.
- The visible model label is now `StringVoice EG-052 PassiveFast`.

This intentionally avoids per-mode amplitude cutoffs, lower quality modes, or modal-count reductions. Those are still candidates for a later quality/performance switch because they may be audible in tails, feedback onset, or high-gain contexts.

Validation renders after the passive fast path:

- Player-articulation MIDI, feedback 0%, lookahead off, finger noise 0%: 45.042x realtime, up from EG-051 at 17.847x.
- Player-articulation MIDI, feedback 100%, lookahead off, finger noise 0%: 4.542x realtime, up from EG-051 at 2.835x.
- Player-articulation MIDI, feedback 0%, lookahead 250 ms, finger noise 60%: 43.876x realtime, up from EG-051 at 17.300x.
- Feature-audition MIDI with MPE mode enabled: 29.644x realtime, up from EG-051 at 10.394x.

The EG-052 player-articulation baseline, lookahead, and feedback-100 WAVs matched the corresponding EG-051 WAVs byte-for-byte.

Listening verification:

- User null-tested an EG-052 bounce against the EG-051 VST output in Bitwig and got a perfect null.
- User observed roughly another halving of visible CPU usage in Bitwig.

## Manual Verification

- Load the plugin in Bitwig on Mac and PC.
- Test one sustained note, a six-note chord, and the player-articulation audition MIDI.
- Compare CPU at `Amp Feedback` 0%, 50%, and 100%.
- Confirm no more than six guitar-string voices ring at once.
- Confirm MPE bends still affect only the intended held note.
- Listen for lost tail detail, feedback onset changes, and note-stealing artifacts.

## 2026-05-09 Pitch Control-Rate Pass

Antigravity's baseline review re-raised the per-sample pitch modulation cost in `StringVoice::renderSample`.

Implementation notes:

- Added a 4-sample pitch control interval inside `StringVoice`.
- Kept aftertouch and MPE pitch smoothing per-sample.
- Recomputed vibrato, whammy, aftertouch-bend, and MPE pitch ratios every 4 samples.
- Cached adjusted modal phase-step sine/cosine values every 4 samples when pitch modulation is active.
- Left neutral-pitch rendering on the existing precomputed modal phase-step path.

Local validation:

- `cmake --build build --config Release --target GuitarAG_VST3`
- `cmake --build build --config Release --target GuitarAGOfflineRender`
- Player-articulation MIDI, feedback 0%: 37.636x baseline to 43.545x optimized in one local run; WAVs byte-identical.
- Player-articulation MIDI, feedback 100%: 17.726x baseline to 17.702x optimized in one local run; WAVs byte-identical.
- Feature-audition MIDI with MPE mode enabled: 31.692x baseline to 37.858x optimized in one local run.
- The MPE feature WAV differs because pitch modulation is now control-rate. The 4-sample interval measured about 0.74% relative RMS difference against the previous per-sample render.

Follow-up:

- Manual DAW listening should confirm MPE bends, whammy, vibrato, and aftertouch bend remain smooth.
- A later pass can still address feedback-loop `tanh` and feedback scalar math if profiling shows it matters.

## 2026-05-09 Feedback Weight Cache Pass

Antigravity verified the 4-sample pitch control-rate pass and cleared Codex to target feedback-loop math next.

Implementation notes:

- Tested a global feedback scalar cache first, then discarded it because it slightly worsened local high-feedback render speed and changed voice-sample counts.
- Added an 8-sample feedback control interval inside `StringVoice`.
- Cached per-mode feedback harmonic weights and loop-lock weights when feedback is active.
- Moved expensive `round`, `exp`, and `log2` work out of the per-mode/per-sample feedback inner loop.
- Kept per-sample energy gates, feedback rise, release scaling, loop signal scaling, modal decay, and feedback injection.
- Left exact global feedback-loop `std::tanh` saturation unchanged.
- The visible model label is now `StringVoice EG-059 FeedbackWeightCache`.

Local validation:

- `cmake --build build --config Release --target GuitarAG_VST3`
- `cmake --build build --config Release --target GuitarAGOfflineRender`
- Player-articulation MIDI, `Amp Feedback` 100%: 17.768x baseline to 29.543x optimized in one local run.
- Feature-audition MIDI with MPE enabled and `Amp Feedback` 100%: 9.315x baseline to 19.648x optimized in one local run.
- Player-articulation MIDI, `Amp Feedback` 0%: audio sample data remained identical to the EG-058 no-feedback render.
- High-feedback player-articulation WAV difference: about 0.0015% relative RMS.
- High-feedback feature-audition WAV difference: about 0.0199% relative RMS.

Follow-up:

- Manual DAW listening should confirm feedback bloom, string focus, and harmonic takeover still feel natural.
- A later pass can still evaluate fast or approximate `tanh`, but only with explicit listening checks because it directly changes feedback saturation tone.

## 2026-05-09 Feedback Tanh Experiment

Antigravity verified EG-059, accepted the 8-sample feedback weight cache as-is, and cleared Codex to evaluate the remaining feedback-loop `std::tanh` target.

Result: no production code change kept.

Implementation candidates tested:

- A high-accuracy rational `tanh` approximation for the three global feedback-loop saturation calls in `AudioEngine::updateAmpFeedbackLoop`.
- A cheaper rational `tanh` approximation for the same three calls.

Local measurements:

- EG-059 baseline, player-articulation MIDI with `Amp Feedback` 100%: 30.445x realtime.
- EG-059 baseline, feature-audition MIDI with MPE enabled and `Amp Feedback` 100%: 19.865x realtime.
- EG-059 baseline, player-articulation MIDI with `Amp Feedback` 0%: 43.446x realtime.
- EG-059 baseline, player-articulation MIDI with heavy pick/contact settings and feedback off: 36.268x realtime.
- High-accuracy `tanh` candidate, player-articulation MIDI with `Amp Feedback` 100%: 30.172x realtime.
- High-accuracy `tanh` candidate, feature-audition MIDI with MPE enabled and `Amp Feedback` 100%: 19.906x realtime.
- Cheaper `tanh` candidate, player-articulation MIDI with `Amp Feedback` 100%: 30.139x realtime, with a slight voice-sample count change.

Conclusion:

- Keep exact `std::tanh` in the global feedback loop for now.
- The approximation does not create a meaningful CPU win on the local offline renderer.
- The next optimization should target contact/pick transient math, modal/contact render maintainability, or a dedicated profiler-backed pass.

## 2026-05-09 Contact Trig Fast Path

Antigravity verified the rejected feedback-loop `tanh` experiment and cleared Codex to target contact/pick transient math.

Implementation notes:

- Added a contact-local sine approximation for transient-only pick/gesture tones in `StringVoice::renderSample`.
- Replaced the contact burr `std::pow(abs(sin), 7)` with explicit multiplication.
- Kept exact `std::tanh` in the contact ridge/soft-clip path because the earlier feedback-loop approximation showed that tanh swaps need profiler evidence.
- Left note-start physical string initialization, modal rendering, MPE pitch modulation, and feedback weight caching unchanged.
- The visible model label is now `StringVoice EG-060 ContactTrigFast`.

Local validation:

- `cmake --build build --config Release --target GuitarAGOfflineRender`
- Contact-heavy player-articulation render with `Pick Texture` 100%, `Pick Stiffness` 100%, `Finger Noise` 60%, and feedback off: 35.965x baseline to 38.020x optimized in local sequential runs. Max block time fell from 1.705 ms to 0.671 ms in those runs.
- Default player-articulation render with feedback off: 43.446x baseline to 43.705x optimized in one local run.
- Player-articulation render with `Amp Feedback` 100%: 30.445x baseline to 30.103x optimized in one local run.
- Default player-articulation WAV difference: about 0.0012% relative RMS.
- High-feedback player-articulation WAV difference: about 0.0012% relative RMS.
- Heavy contact WAV difference: about 0.060% relative RMS.

Follow-up:

- Manual listening should confirm pick scrape, coin edge, heavy pick rasp, hammer-on/tap impact, and pull-off snap remain natural.
- A later maintainability pass should consider splitting the modal and contact render branches into smaller helpers after the performance profile settles.

## 2026-05-09 Render Helper Maintainability Pass

Antigravity verified EG-060, accepted the contact trig fast path as-is, and cleared Codex to make `StringVoice::renderSample` easier to maintain.

Implementation notes:

- Extracted modal-bank execution and feedback injection to `renderModalBank`.
- Extracted pick transient decay to `renderPickTransient`.
- Extracted pick/contact, finger-impact, and pull-off transient generation to `renderContactLayer`.
- Added `FeedbackRenderContext` to group feedback gate values passed into the modal bank.
- Preserved arithmetic and update order; this is a behavior-neutral refactor.
- The visible model label is now `StringVoice EG-061 RenderHelpers`.

Local validation:

- `cmake --build build --config Release --target GuitarAGOfflineRender`
- Player-articulation MIDI, default settings, `Amp Feedback` 0%: audio sample data matched EG-060 exactly.
- Player-articulation MIDI, `Pick Texture` 100%, `Pick Stiffness` 100%, `Finger Noise` 60%, `Amp Feedback` 0%: audio sample data matched EG-060 exactly.
- Player-articulation MIDI, `Amp Feedback` 100%: audio sample data matched EG-060 exactly.

Follow-up:

- Antigravity should review whether the helper boundaries are clear enough for future work.
- The next performance decision should be profiler-backed rather than speculative math substitution.

## 2026-05-09 Optimization Cycle Closeout

Antigravity verified and accepted EG-061 as-is.

Conclusion:

- End the code-level optimization cycle for now.
- EG-059 addressed the feedback-path bottleneck.
- EG-060 addressed the transient contact math bottleneck.
- EG-061 made the render path easier to maintain without changing audio sample data.
- Do not continue speculative math substitutions without a profiler-backed reason.

Next step:

- Human DAW/VST3 listening in Bitwig or Live using `.agent/handoffs/20260509-162331-codex-human-daw-listening.md`.
- Listen specifically for MPE pitch/expression smoothness, feedback bloom/string focus, contact transient naturalness, lookahead expression sync, and DAW CPU stability.

## 2026-05-09 Human DAW Listening Result

Initial human DAW/VST3 listening after installing EG-061:

- The VST sounds the same.
- The UI header shows `EG-061`, confirming the expected installed build.
- DAW-visible performance improvements are present.
- The DAW-visible gains are more modest than the offline render factors suggested.

Interpretation:

- This is provisionally accepted for sound equivalence and build identity.
- The smaller DAW-visible gain is expected because offline render reports isolate Guitar AG DSP throughput, while a DAW CPU meter includes host scheduling, buffer deadlines, wrapper overhead, UI work, amp-sim chains, other plugins/tracks, and host-specific CPU-meter behavior.
- If performance still feels short in a specific scenario, capture host, sample rate, buffer size, amp-sim chain, MIDI clip, and parameter settings before starting profiler-backed follow-up.
