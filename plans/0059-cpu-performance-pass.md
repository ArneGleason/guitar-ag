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
