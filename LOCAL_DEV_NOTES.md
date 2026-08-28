# Local Developer Notes

This is the Studio Pattern local workflow memory for Guitar AG. Keep commands, setup notes, validation steps, and recurring gotchas here when they are useful across sessions or machines.

## Studio Pattern Source

- Version/date: 2026-05-09 initial canonical scaffold
- Source repository: `the-studio-pattern`
- Source URL: `https://github.com/ArneGleason/the-studio-pattern`
- Source commit: `5624928215232b7b82702929a7b9f8f11f19fd5d`
- License: CC BY-SA 4.0
- Local adaptation: lightweight lead/reviewer memory for a physical-model guitar VST3 project; `AGENTS.md` remains the project-specific authority.

## Local Environment

- Machine handle: `macbook-pro-m5`
- Primary agent/tool here: Codex
- Local checkout: `/Users/arnegleason/code/github.com/arnegleason/guitar-ag`
- Canonical repo: `https://github.com/ArneGleason/guitar-ag`
- Canonical Studio Pattern repo: `https://github.com/ArneGleason/the-studio-pattern`

Machine handles are assigned by the human owner. Do not treat OS hostname, username, serial number, or network name as canonical unless the human explicitly maps it here.

## Known Studio Environments

- `macbook-pro-m5`: Codex lead environment for this checkout.
- `mac-mini-pro-m4`: Antigravity reviewer environment. The human may create an empty `guitar-ag` folder there before handoff; Antigravity should clone or pull from GitHub before reviewing.
- Human Windows PC: PC build/deployment environment for Windows VST3 builds and DAW audition. A canonical machine handle is not assigned yet; do not infer it from OS hostname.

## Windows PC Checkout and Build

The canonical Windows checkout is:

```text
C:\code\github\guitar-ag
```

JUCE is currently available at `C:\code\JUCE`. Configure and build from PowerShell:

```powershell
cmake -S . -B build-vs2022-x64 -DJUCE_PATH=C:\code\JUCE -A x64
cmake --build build-vs2022-x64 --config Release --target GuitarAG_VST3
cmake --build build-vs2022-x64 --config Release --target GuitarAGOfflineRender
```

Expected artifacts:

```text
build-vs2022-x64\GuitarAG_artefacts\Release\VST3\Guitar AG.vst3
build-vs2022-x64\GuitarAGOfflineRender_artefacts\Release\GuitarAGOfflineRender.exe
build-vs2022-x64\GuitarAGReferenceCapture_artefacts\Release\Guitar AG Reference Capture.exe
```

Build the interactive clean-DI reference recorder separately:

```powershell
cmake --build build-vs2022-x64 --config Release --target GuitarAGReferenceCapture
```

Its request/session workflow is documented in
`docs/reference-capture-tool.md`. Requests and audio sessions default to
`Documents\Guitar AG Reference Captures`, outside Git. Enable only the intended
Focusrite input because the MVP records the first enabled input channel.

The Windows capture target compiles JUCE ASIO support. In Audio Settings,
prefer `ASIO` / `Focusrite USB ASIO` and close other ASIO clients before opening
the device. The Focusrite driver is an external machine dependency and is not
bundled with the application.

Create the guided low-E model-evaluation inventory and stable request files in
the operating system's Documents known folder:

```powershell
python scripts\create-reference-capture-inventory.py
```

The app discovers that inventory automatically. Keep the existing noise-floor
approval when capture settings are unchanged, complete the six Phase 1 low-E
exercise items, then stop for a model comparison. Later strings, dynamics,
depths, and pick materials are deliberately excluded until that evaluation.

Reproduce the Plan 0104 low-E analysis after rendering the matching MIDI into a
diagnostic directory:

```powershell
python scripts\create-low-e-model-evaluation-midi.py `
  --output-directory build-vs2022-x64\diagnostics\eg0104-low-e-reference-model

python scripts\analyze-low-e-model-reference.py `
  --capture-root "$([Environment]::GetFolderPath('MyDocuments'))\Guitar AG Reference Captures" `
  --model-directory build-vs2022-x64\diagnostics\eg0104-low-e-reference-model `
  --output-directory build-vs2022-x64\diagnostics\eg0104-low-e-reference-model
```

The analyzer is research tooling and requires NumPy and SciPy. See
`analysis/low-e-reference-model-comparison-20260828.md` for the rendered control
and ablation set and the interpreted results.

Plan 0105 keeps the low-E candidate offline. After rendering current and
candidate down/up/alternate files into
`build-vs2022-x64\diagnostics\eg0105-low-e-reference-candidate`, reproduce its
measurement summary and compact RMS-matched listening set with:

```powershell
python scripts\analyze-low-e-reference-candidates.py `
  --reference-events build-vs2022-x64\diagnostics\eg0104-low-e-reference-model\events.csv `
  --candidate-directory build-vs2022-x64\diagnostics\eg0105-low-e-reference-candidate `
  --output build-vs2022-x64\diagnostics\eg0105-low-e-reference-candidate\candidate-grid.json

python scripts\create-low-e-candidate-listening-set.py `
  --render-directory build-vs2022-x64\diagnostics\eg0105-low-e-reference-candidate `
  --output-directory build-vs2022-x64\diagnostics\eg0105-low-e-reference-candidate\listening
```

The selected candidate adds
`--legacy-pluck-position 0.14 --legacy-body-decay-time-scale 1.50` to an
otherwise identical EG-089 humbucker/neck render. See
`analysis/low-e-reference-candidate-20260828.md`; do not promote it before the
human listening verdict.

The previous checkout at `C:\code\guitar-ag` was clean and at the same GitHub commit when the canonical clone was created. Windows would not move it while the original Codex workspace held the directory open. Remove that duplicate only from a later task after the original workspace is closed and the exact path is re-verified.

On this PC, Git may reject a checkout as dubious ownership. Prefer an explicit per-command override such as:

```powershell
git -c safe.directory=C:/code/github/guitar-ag status --short --branch
```

Do not add a broad global safe-directory wildcard.

## Windows Realism Reference Data

Large reference audio stays outside Git:

```text
C:\code\reference-audio\Guitar-TECHS\archives\P1_singlenotes.zip
C:\code\reference-audio\Guitar-TECHS\extracted\P1_singlenotes
C:\code\reference-audio\Guitar-TECHS\analysis
```

The current Windows calibration render is generated under:

```text
build-vs2022-x64\diagnostics\baseline-v031-single-note.wav
```

Use the existing repository analysis scripts for repeatable comparisons, but treat their scores as diagnostic evidence rather than a listening-test replacement.

Plan 0090's experimental engine is available only in the offline renderer:

```powershell
build-vs2022-x64\GuitarAGOfflineRender_artefacts\Release\GuitarAGOfflineRender.exe `
  --midi tests\midi\guitar-ag-stateful-waveguide-audition.mid `
  --output build-vs2022-x64\diagnostics\eg090-stateful-preserve.wav `
  --string-engine stateful `
  --stateful-repick 1 `
  --player-feel 0 `
  --strum-speed 0 `
  --finger-noise 0 `
  --legato-articulation 1
```

Omitting `--string-engine` selects the protected legacy modal path. The VST3 target does not compile the experimental stateful engine.

## Multi-Machine Git Hygiene

Before meaningful work:

```sh
git fetch origin --prune
git status --short --branch
```

If the branch is behind and there are no local changes:

```sh
git pull --ff-only
```

If there are local changes, ahead commits, or uncertainty about work on another machine, pause and ask whether anything should be committed, pushed, or pulled before continuing.

Before handing work to the reviewer agent, prefer pushing the branch so the reviewer can inspect the same commit from GitHub.

Reviewer bootstrap on `mac-mini-pro-m4` with Antigravity:

If the human created an empty `guitar-ag` folder:

```sh
git clone https://github.com/ArneGleason/guitar-ag.git .
git switch main
git pull --ff-only
git status --short --branch
```

If the folder is already a Git checkout:

```sh
git fetch origin --prune
git switch main
git pull --ff-only
git status --short --branch
```

If the folder is not empty and not a Git checkout, do not delete files without human confirmation. Move aside stray local files or clone into the parent directory instead.

## Setup

External dependencies are not vendored into this repository.

```sh
git clone https://github.com/juce-framework/JUCE.git /Users/arnegleason/code/JUCE
cmake -S . -B build -DJUCE_PATH=/Users/arnegleason/code/JUCE -DCMAKE_BUILD_TYPE=Release
```

## Build

Build everything configured:

```sh
cmake --build build --config Release
```

Build the VST3 target:

```sh
cmake --build build --config Release --target GuitarAG_VST3
```

Build and install the VST3 to the user plugin folder:

```sh
scripts/install-vst3.sh --build
```

Installed macOS VST3 path:

```text
~/Library/Audio/Plug-Ins/VST3/Guitar AG.vst3
```

Windows canonical development install path:

```text
C:\Program Files\Common Files\VST3\Guitar AG.vst3
```

Do not keep an additional `Guitar AG.vst3` in `%LOCALAPPDATA%\Programs\Common\VST3`; Bitwig scans both locations and can list/load the wrong copy.

## Offline Render

Build the offline renderer:

```sh
cmake --build build --config Release --target GuitarAGOfflineRender
```

Render the calibration MIDI:

```sh
build/GuitarAGOfflineRender_artefacts/Release/GuitarAGOfflineRender \
  --midi tests/midi/single-note-calibration.mid \
  --output build/diagnostics/guitar-ag-test.wav \
  --sample-rate 48000 \
  --block-size 512 \
  --tail-seconds 2.0
```

The offline renderer uses the same `AudioEngine` as the plugin. It is useful for DSP regression checks, but it does not verify DAW scanning, UI behavior, or host/plugin lifecycle.

The default legacy-engine render uses the production EG-089 tone. To reproduce
the former EG-088 tone without changing the VST3, add:

```sh
--legacy-tone-recipe previous
```

## Validate

For a docs-only or workflow-only change:

```sh
git status --short
```

For code changes, run at least:

```sh
cmake --build build --config Release --target GuitarAG_VST3
```

For DSP or MIDI behavior changes, also run the offline renderer where practical and describe the DAW checks that still need human verification.

## Studio Pattern Workflow

Resume:

```sh
git fetch origin --prune
git status --short --branch
```

Then read:

```text
AGENTS.md
LOCAL_DEV_NOTES.md
.agent/session.json
.agent/PROJECT_LOG.md
.agent/REVIEW_QUEUE.md
latest .agent/handoffs/*.md when present
```

Suspend or hand off:

```sh
git status --short --branch
git rev-parse HEAD
```

Then update `.agent/session.json`, `.agent/PROJECT_LOG.md`, `.agent/REVIEW_QUEUE.md`, and create a handoff under `.agent/handoffs/` when another participant needs context.

## Operational Notes

- Compiled plugin binaries should go through GitHub Releases, not be committed directly.
- Build outputs live under `build/` and release zips under `dist/`; both are ignored by git.
- Local reference audio belongs outside the repo or under ignored `reference-audio/`.
- Bitwig can cache duplicate plugin entries after replacing the VST3. Use the editor identity line to confirm the loaded build.
- Keep audio/DSP code separate from UI code, and avoid allocation, blocking, file I/O, logging, or UI interaction on the audio thread.

## Known Gotchas

- If the DAW-side MPE bend range and `MPE Bend Range` disagree, drawn bends will not match the expected semitone movement.
- The offline renderer is not a substitute for testing the VST3 in a host such as Bitwig or Reaper.
- The Mac lead and reviewer machine handles are assigned. The human has confirmed a Windows PC build/deployment environment, but its canonical handle still needs to be assigned explicitly.
