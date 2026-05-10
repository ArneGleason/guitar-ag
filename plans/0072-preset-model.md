# 0072 — Preset Model

## Context

The plugin now has enough expressive controls that a new user may not discover useful setups by randomly moving sliders. The project needs a preset path that supports:

- a small curated stock set for releases;
- user-created presets from DAW audition work;
- easy addition of presets without changing DSP code;
- host session recall remaining reliable.

The current processor already serializes the full `AudioProcessorValueTreeState` in `getStateInformation` and restores it in `setStateInformation`. JUCE program support is currently a one-program stub, so host session recall exists but in-plugin preset browsing does not.

## Recommended Architecture

Use APVTS parameter state as the source of truth, then add a file-backed preset layer above it.

Preset file shape:

```text
GuitarAGPreset
  metadata: name, category, author, description, createdWithVersion, createdWithModelLabel
  parameters: APVTS parameter IDs and normalized or raw values
```

Suggested extension: `.guitaragpreset`.

Suggested locations:

- Stock presets bundled with the release or installed into a read-only stock preset folder.
- User presets in `~/Library/Application Support/Guitar AG/Presets` on macOS.
- Later Windows user presets under `%APPDATA%/Guitar AG/Presets`.

## Stock Presets

Stock presets should be ordinary preset files, not hard-coded C++ constants. The plugin can scan bundled stock presets first, then user presets. That gives release builds a curated default library while keeping the preset format easy to inspect, diff, and route through Studio Pattern review.

There are two reasonable packaging modes:

- Bundle stock presets as binary resources or plugin-bundle resources. This is robust for releases but updating stock presets requires a new package/build.
- Install stock presets into an application-support stock folder. This allows the preset set to change without recompiling code, though release packaging still needs to copy the files.

For this project, prefer a file-copy install step first. It keeps stock presets visible and easy to revise while the product sound is still changing.

## User Presets

Add a small UI later for:

- save current sound as preset;
- load selected preset;
- reveal user preset folder;
- rescan presets.

Saving should write current APVTS values plus metadata. Loading should set APVTS values on the message thread and let the existing parameter attachments and audio-engine smoothing handle the transition.

## Verification Targets

- Host session save/reopen still recalls all parameters through the existing state chunk.
- A preset file saved from the current state can be loaded into a fresh plugin instance.
- A preset can be added to the user preset folder and appears after rescan without recompiling.
- Stock presets can be packaged with a release and remain separate from user presets.
- Unknown future parameters fall back to plugin defaults, and unknown old preset fields are ignored.
