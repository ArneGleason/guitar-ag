# Low-E Reference versus EG-089 Model — 2026-08-28

## Result

The Plan 0103 capture set is usable and identifies four model gaps clearly.
The largest mismatch is not an isolated pick-noise layer: it is the production
modal string's low-E spectral balance and decay. Human variation is also wider
and more context-dependent than the current deterministic scatter.

Do not promote a production change directly from this report. The next safe
step is an offline 2x2 modal-envelope/decay experiment followed by a human
listening gate.

## Evidence Quality

- 12 approved exercise WAVs plus two approved noise-floor WAVs;
- 84 detected and onset-aligned reference strokes;
- mono 24-bit PCM at 44.1 kHz throughout;
- zero clipped samples, no writer-overflow flags, and no exact-zero runs longer
  than two samples in any reference WAV;
- connected-input noise floor approximately -80 dBFS RMS;
- reference ringing fundamental medians from 83.127 to 83.168 Hz, showing that
  the played string stayed very stable despite natural timing variation;
- the guitar was approximately 15 cents sharp of standard E2, so the model's
  exact 82.41 Hz is not itself a defect.

The independent ringing batches contain six clear strokes rather than the
requested four. The damped alternate batches contain six strong alternating
events rather than 12. Both deviations add usable examples: the set still has
six damped alternate downstrokes and six upstrokes across two takes.

The selected-take notes do not identify guitar, pickup, or pick. Absolute tone
matching therefore applies only to this capture chain. Within-set contrasts
(direction, damping, decay, repicking, and variation) remain strong evidence.

## Main Measurements

### 1. The production modal string is far brighter than this real low E

For 60 ms onset windows:

| Material | Attack centroid | Energy below 500 Hz | Energy above 1.5 kHz |
|---|---:|---:|---:|
| Reference ringing, across groups | 220-262 Hz | 90.8-95.0% | 0.4-0.7% |
| EG-089 ringing | 1,593-1,625 Hz | 3.7-4.7% | 45.0-62.3% |

The 55-255 ms early-body centroid is 190-221 Hz in the reference and
1,409-1,480 Hz in EG-089.

Disabling the direct pick mix, attack modes, pick transient, or contact layer
does not materially change these aggregate measurements. Each ablation's signed
residual is 51-77 dB below the production render. The mismatch is therefore in
the modal string/excitation balance, not an independently mixed pick click.

`String Age = 100%` is the closest existing darkening control, but its attack
centroid remains 1,197 Hz with 19.8% above 1.5 kHz. Existing performance/tone
controls cannot reach the reference region without new modal calibration.

### 2. The real low E decays more slowly

Long final-event decay fits:

| Material | Down | Up / final alternate |
|---|---:|---:|
| Reference | -1.02 dB/s | -2.06 dB/s independent; -1.72 dB/s alternate |
| EG-089 | -4.30 dB/s | -4.84 dB/s independent; -4.67 dB/s alternate |

The current model loses energy roughly two to four times faster over this
observed low-E range even with `Sustain = 100%`.

### 3. Fretting-hand damping must preserve the onset and remove the body

The reference damped events retain attack peaks close to ringing peaks, then
place their 55-255 ms body approximately 24-28 dB below the attack peak.
Production `Palm Mute = 100%` instead drives the body about 85 dB below the peak
and also reduces the attack heavily. A 95% palm mute approximately matches the
reference body/peak ratio (-26.8 dB) but attenuates the modeled attack by about
6.6 dB relative to modeled ringing and leaves the same excessive modal
brightness.

Fretting-hand damping should therefore become a separate excitation/boundary
intervention: preserve the first contact/attack moment, then impose strong
distributed modal loss. Reusing the current palm-mute curve couples those two
effects incorrectly.

### 4. Direction and human variation are contextual

Median upstroke-minus-downstroke attack-peak differences:

| Context | Reference | EG-089 |
|---|---:|---:|
| Ringing independent | +0.55 dB | +2.69 dB |
| Ringing alternate | +2.76 dB | +2.33 dB |
| Damped independent | +3.61 dB | -0.64 dB |
| Damped alternate | +2.81 dB | -1.81 dB |

The reference's 10th-to-90th-percentile attack spread is 2.9-6.0 dB depending
on direction and performance pattern. EG-089 produces only 0.3-1.9 dB in the
matching groups. `Player Feel = 100%` reaches a 2.47 dB spread for independent
downstrokes, which is useful but still does not reproduce the wider upstroke and
alternate distributions.

Natural timing itself is clean evidence. Median alternate intervals are
0.53-0.78 seconds for ringing and 0.58-0.66 seconds for damping. Independent
damped strokes are especially consistent at roughly 1.15-1.23 seconds.

## Repick Limitation

The human alternate takes re-excite an already moving string. The production
legacy modal engine restarts the assigned `StringVoice` for every note-on, so it
cannot preserve the pre-pick phase/energy state. The alternate captures are a
good future calibration set, but increasing random scatter cannot replace this
missing state transition.

## Recommended Next Experiment

Build an offline-only 2x2 low-E comparison while preserving EG-089 exactly as
the control:

1. current production;
2. empirical low-E modal-envelope correction only;
3. slower low-E modal decay only;
4. envelope plus decay.

The envelope candidate should move energy from the current upper modal bank
toward the first few E2 harmonics without adding a low-pass effect after the
pickup. The decay candidate should target the observed -1 to -2 dB/s region.
Use the current reference/model analyzer to reject numerically implausible
candidates, then prepare loudness-matched WAVs for human listening.

After the ringing foundation passes, add a separate offline fretting-hand-
damping intervention. State-preserving repick and context-dependent human
scatter should follow; widening variation around the current wrong modal
envelope would only make the mismatch less consistent.

## Reproduction

The ignored diagnostic directory is:

```text
build-vs2022-x64/diagnostics/eg0104-low-e-reference-model
```

It contains the generated MIDI, EG-089/control/ablation WAVs, onset plots,
`validation.csv`, `events.csv`, and `summary.json`. The tracked generators and
analyzer are:

```text
scripts/create-low-e-model-evaluation-midi.py
scripts/analyze-low-e-model-reference.py
```
