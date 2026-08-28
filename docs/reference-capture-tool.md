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
2. Open the app and choose the Focusrite device type/device in Audio Settings.
3. Enable only the one physical input carrying the guitar. The MVP records the
   first enabled input channel.
4. Prefer 48 or 96 kHz. The app writes the active device rate without
   resampling.
5. Disable direct monitoring if hearing the dry input is distracting. App
   playback uses the selected output, but live input is not software-monitored.
6. Adjust interface gain so the hardest requested strokes stay at or below
   roughly -12 dBFS. The meter turns orange above that reference target.

The selected audio-device configuration is restored on the next launch.

## Guided Inventory

The app automatically loads the default phased inventory from
`Documents\Guitar AG Reference Captures\capture-inventory.json`. Its selector
shows every requested sound in order, marks items `[done]` after the required
approved take count, and opens the first unfinished item on launch.

Complete Phase 0 and Phase 1 first, then stop for analysis. See
`docs/reference-capture-inventory.md` for the human-readable checklist and why
the later phases are intentionally deferred.

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
  --instructions "Record eight isolated medium downstrokes on open low E, shallow depth, with about half a second between strokes." `
  --string "low E" `
  --direction down `
  --dynamics medium `
  --pick-depth shallow `
  --pick "medium plastic" `
  --muting ringing `
  --comparison-group "pick-contact-low-e-medium" `
  --takes 8
```

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
- Record as many takes as useful; one WAV is created per take.
- `Stop` flushes the file and adds peak, RMS, duration, device, sample-rate, and
  overflow evidence to the session.
- Select any take and use `Play selected` to audition it.
- Mark useful takes `Approved`. More than one take may be approved because
  repeat-to-repeat distribution is part of the model target.
- Mark unusable performances `Rejected`; the app retains their audio so a
  mistaken decision is recoverable and rejected artifacts remain explainable.
- Use `Reset candidate` when a verdict is not settled.
- Reopening the same request restores existing takes, statuses, and notes.
- Inventory progress refreshes immediately when a take is approved or reset.

Validate and summarize a completed session:

```powershell
python scripts\summarize-reference-capture-session.py "C:\path\to\session.json"
python scripts\summarize-reference-capture-session.py "C:\path\to\session.json" --approved-paths
```

## Muted Versus Ringing Comparisons

Do not treat a damped recording as pure pick noise and do not expect a direct
waveform null. Muting changes boundary conditions, pickup-visible string
motion, timing, and the act of picking itself.

For the first pick-contact comparison, keep guitar, pickup, controls, pick,
string, direction, dynamics, depth, interface gain, and physical picking
location fixed, then create three requests sharing one `comparison_group`:

1. normal ringing;
2. fretting-hand damping at non-harmonic touch points;
3. soft foam/cloth damping near the fretboard, away from the pick.

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
