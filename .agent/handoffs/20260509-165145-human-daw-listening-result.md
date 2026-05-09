# Studio Handoff: Human DAW Listening Result

- Timestamp: 2026-05-09T16:51:45-04:00
- Status: verified
- Branch: `main`
- To: `guitar-ag lead`
- From: `human operator`
- Subject: DAW/VST3 listening result after EG-061 install
- To agent: `Codex`
- To machine: `macbook-pro-m5`
- From agent: `Human`
- From machine: `macbook-pro-m5`

## Human Context

The human operator built and installed the current VST3 with `scripts/install-vst3.sh --build`, then opened the plugin in a DAW for the first listening check after the accepted EG-058 through EG-061 optimization cycle.

## Findings

- The VST sounds the same as before the optimization cycle.
- The VST UI header shows `EG-061`, confirming the installed plugin is the expected build.
- Performance improvements are visible in the DAW.
- The DAW-visible performance improvement is more modest than the offline render performance factors suggested.

## Interpretation

This is still a successful listening result. The offline renderer isolates Guitar AG DSP throughput, while a real DAW CPU meter also reflects host scheduling, buffer deadlines, plugin-wrapper overhead, UI/graphics work, amp-sim chains, other tracks/plugins, and the host's CPU-meter model. The largest offline wins were also concentrated in specific stress paths, especially high feedback and heavy contact transients, so normal DAW use can reasonably show smaller visible gains.

## Status

- Human listening acceptance: provisionally accepted for sound equivalence and correct EG-061 identity.
- Follow-up only if needed: capture host, sample rate, buffer size, amp-sim chain, MIDI clip, and parameter settings for any specific CPU spikes or audible artifacts.
