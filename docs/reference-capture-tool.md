# Guitar AG Reference Capture

## Purpose

`Guitar AG Reference Capture` is a desktop companion for recording clean guitar
DI observations that guide the physical model. It is deliberately separate
from the VST3: recordings, disk writes, device setup, and approval state never
enter the plugin audio path.

The app implements a short research loop:

1. An agent creates a focused JSON request for one sound and comparison setup.
2. The human records and auditions several takes through the Focusrite.
3. The human marks one or more takes approved and may add listening notes.
4. The app writes the decision and capture measurements to `session.json`.
5. An agent validates the session and uses only the approved references for the
   next analysis or modeling hypothesis.

Captured audio is reference evidence only. Guitar AG remains a synthesized
physical model and does not ship or play these WAVs.

## Build on Windows

```powershell
cmake -S . -B build-vs2022-x64 -DJUCE_PATH=C:\code\JUCE -A x64
cmake --build build-vs2022-x64 --config Release --target GuitarAGReferenceCapture
```

The executable is:

```text
build-vs2022-x64\GuitarAGReferenceCapture_artefacts\Release\Guitar AG Reference Capture.exe
```

## First Focusrite Setup

1. Connect the guitar directly to the Focusrite instrument input.
2. Open the app and choose `ASIO` as the device type in Audio Settings, then
   choose `Focusrite USB ASIO`. The Windows capture build enables JUCE's ASIO
   backend; the Focusrite USB driver must be installed separately.
3. Enable only the one physical ASIO input carrying the guitar. The MVP records
   the first enabled input channel.
4. Prefer 48 or 96 kHz. The app writes the active device rate without
   resampling.
5. Disable direct monitoring if hearing the dry input is distracting. App
   playback uses the selected output, but live input is not software-monitored.
6. Adjust interface gain so the hardest requested strokes stay at or below
   roughly -12 dBFS. The meter turns orange above that reference target.

The selected audio-device configuration is restored on the next launch.

On Windows, prefer the Focusrite ASIO device over `Windows Audio` or `Windows
Audio (Exclusive Mode)`. ASIO gives the capture app a direct interface-specific
path and keeps its input/output clock, sample rate, and buffer configuration
together. Close DAWs, dictation, conferencing, and other audio applications
before selecting the ASIO driver because many ASIO drivers allow only one
client at a time. Use the `Control Panel` button in Audio Settings, when shown,
to open Focusrite Device Settings and choose the diagnostic buffer size.

If `ASIO` or `Focusrite USB ASIO` is missing, install or repair the current
Focusrite Windows driver and relaunch the app. The app does not bundle a
Focusrite driver.

## Guided Inventory

The app automatically loads the focused low/high-E endpoint inventory from
`Documents\Guitar AG Reference Captures\capture-inventory.json`. Its selector
shows every requested sound in order, marks items `[done]` after the required
approved take count, and opens the first unfinished item on launch.

The noise floor and six low-E items are complete. Record only the three Phase 2
high-E ringing items, then stop for endpoint comparison before A/D/G/B capture.
See `docs/reference-capture-inventory.md` for the exact 6/6/12 stroke order,
natural-timing guidance, reset timing, final decay, and setup description.

Generate or deliberately refresh the inventory definitions with:

```powershell
python scripts\create-reference-capture-inventory.py
python scripts\create-reference-capture-inventory.py --force
```

`--force` replaces request and inventory definitions but does not delete session
folders or captured audio. Use it only when intentionally adopting a revised
inventory.

## Create and Load a Request

The dependency-free request generator keeps agent briefs consistent:

```powershell
python scripts\create-reference-capture-request.py `
  --title "Low E medium downstroke ringing" `
  --instructions "Record four independent medium downstrokes in one WAV batch, using the documented ring/stop/reset pattern." `
  --string "low E" `
  --direction down `
  --dynamics medium `
  --pick-depth shallow `
  --pick "medium plastic" `
  --muting ringing `
  --comparison-group "pick-contact-low-e-medium" `
  --takes 2
```

`--takes` is the suggested maximum number of WAV batches, not the number of
strokes inside one WAV. Put the stroke count and order in `--instructions`.

The script prints the request path. Load it with the app's `Load request...`
button when doing an ad-hoc capture outside the inventory, or start the app with:

```powershell
& "build-vs2022-x64\GuitarAGReferenceCapture_artefacts\Release\Guitar AG Reference Capture.exe" `
  --request "C:\path\to\request.json"
```

By default requests and sessions live outside Git under the operating system's
Documents known folder (including OneDrive redirection on Windows), for example:

```text
C:\Users\name\OneDrive\Documents\Guitar AG Reference Captures\
```

## Record and Approve

- Press Space to start a take and press Space again to stop. The shortcut works
  while the capture app is foreground, but is deliberately disabled while the
  take-notes editor or Audio Settings has keyboard focus. Clicking `Record take`
  once returns focus to the main capture surface.
- During recording, press Delete or Backspace to stop immediately and discard
  the partial WAV. The aborted recording is not added to the take list or
  `session.json`.
- Record as many takes as useful; one WAV is created per take.
- `Stop` flushes the file and adds peak, RMS, duration, device, sample-rate, and
  overflow evidence to the session.
- Select any take and use `Play selected` to audition it.
- The selected take's waveform is drawn at an absolute full-scale amplitude,
  not normalized to fill the panel. It includes peak, RMS, duration, a -12 dBFS
  guide, and `HOT`/`POSSIBLE CLIP` cues so levels can be compared visually.
- Mark useful takes `Approved`. More than one take may be approved because
  repeat-to-repeat distribution is part of the model target.
- Mark an unusable performance `Rejected` to retain it with that verdict. With
  the rejected take selected, press Delete or Backspace to permanently erase
  its WAV and remove it from the take list and manifest. Candidate and approved
  takes are protected from deletion until explicitly rejected.
- Use `Reset candidate` when a verdict is not settled.
- Reopening the same request restores existing takes, statuses, and notes.
- Inventory progress refreshes immediately when a take is approved or reset.
- Delete, Backspace, and Space never trigger capture actions while editing take
  notes, using Audio Settings or a file chooser, or while another application is
  foreground.

Validate and summarize a completed session:

```powershell
python scripts\summarize-reference-capture-session.py "C:\path\to\session.json"
python scripts\summarize-reference-capture-session.py "C:\path\to\session.json" --approved-paths
```

## Muted Versus Ringing Comparisons

Do not treat a damped recording as pure pick noise and do not expect a direct
waveform null. Muting changes boundary conditions, pickup-visible string
motion, timing, and the act of picking itself.

For any future ringing/damped comparison, keep guitar, pickup, controls, pick,
string, direction, dynamics, depth, interface gain, and physical picking
location fixed, then compare requests sharing one `comparison_group`:

1. normal ringing;
2. fretting-hand damping at non-harmonic touch points.

Foam or cloth damping can remain a later optional intervention, but it is not
required by the active inventory.

Analysis should onset-align repetitions and compare distributions: contact
duration, crest factor, zero-crossing/event density, fixed-Hz band energy and
decay, tonal-versus-stochastic energy, and muted-to-ringing transfer. A signed
residual can still be a diagnostic listening view, but it is not an isolated
physical source and should not be copied into the instrument.

## Later Articulations

The same request/session format can capture finger and nail attacks, palm mute,
finger lift, wound-string slide/squeak, fret contact, body handling, and other
reference gestures. Keep each request narrow enough that approved takes answer
one modeling question.
