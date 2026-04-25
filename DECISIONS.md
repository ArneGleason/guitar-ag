# Decisions

Use this file to record important project decisions so they do not get lost in chat history.

## Template

```markdown
## YYYY-MM-DD — Decision title

Decision:
Short statement of the decision.

Reason:
Why this choice was made.

Status:
Accepted / Rejected / Superseded / Under Review

Consequences:
What this means for implementation.
```

## 2026-04-25 — Use MPE as the primary expressive control model

Decision:
The instrument will prioritize MPE for independent per-note pitch bend and expression.

Reason:
Independent bending per note is central to the project. Standard MIDI pitch bend is normally channel/global and is not sufficient for the intended playing style.

Status:
Accepted

Consequences:
The MIDI layer must track channel-to-voice assignment and route pitch bend, pressure, and CC74/timbre per active note.

## 2026-04-25 — Start with a simple physical string model

Decision:
The first sound engine should use a simple Karplus-Strong or digital waveguide-style model rather than a complete scientific guitar simulation.

Reason:
This keeps the MVP achievable and lets the project prove the plugin, polyphony, and MPE behavior before deeper modeling.

Status:
Accepted

Consequences:
The first version should focus on musical playability and stable voice behavior. More advanced guitar realism can be added later.

## 2026-04-25 — Exclude amp and cabinet simulation from the MVP

Decision:
The MVP should output a clean DI-style electric guitar tone and rely on external amp/cab sims.

Reason:
Amp and cabinet modeling would expand the scope and distract from the core physical-model/MPE problem.

Status:
Accepted

Consequences:
The tone section should focus on string, pickup, tone, and output shaping only.

## 2026-04-25 — Use CMake with external JUCE for the plugin shell

Decision:
The initial VST3 plugin shell uses CMake and JUCE, with JUCE supplied externally through `JUCE_PATH` or an installed CMake package.

Reason:
JUCE is the preferred framework for the MVP, and keeping it external avoids vendoring a large framework into the project before the plugin architecture is proven.

Status:
Accepted

Consequences:
The repository now contains a small JUCE/CMake plugin structure. A developer must install CMake and provide JUCE before the VST3 target can be configured and built.

## 2026-04-25 — Keep placeholder synthesis separate from plugin wrapper

Decision:
The milestone-one test tone lives in `src/dsp/`, while JUCE lifecycle and editor code live in `src/plugin/`.

Reason:
This keeps the first shell simple while preserving the intended boundaries for later string-model and MPE-routing work.

Status:
Accepted

Consequences:
Future milestones can replace the test-tone voice with a modeled string voice without moving DAW lifecycle or editor code.
