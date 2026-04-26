# Plan 0044: String Age

## Goal

Add a small old-string/new-string control without turning it into a broad tone knob or amp-style EQ.

## Change

- Rename the visible model series from `KS` to `EG` starting with `StringVoice EG-037 StringAge`.
- Add a `String Age` VST parameter.
- Treat 0% as clean/new strings and 100% as old but still musically usable strings.
- Reduce upper-partial excitation and pick/contact brightness as age increases.
- Dampen high harmonics faster as age increases.
- Slightly reduce overall modal sustain at high age.

## Verification

- Build the offline renderer and VST3.
- Render 0%, 50%, and 100% string-age diagnostics.
- Install the VST3 for Bitwig audition.
