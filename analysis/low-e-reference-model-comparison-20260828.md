# Low-E Reference versus EG-089 Model — 2026-08-28

## Result

The Plan 0103 capture set is usable and identifies four model gaps clearly.
Pickup matching is essential: changing the comparison render from EG-089's
default single-coil/39% position to Humbucker/100% neck position removes most
of the apparent spectral and decay mismatch. A smaller source/pickup-geometry
gap remains, and human variation is much wider and more contextual than the
current deterministic scatter.

Do not promote a production change directly from this report. The next safe
step is an offline pluck-position calibration at the matched pickup setting,
followed by a separate decay bracket and human listening gate.

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

The human confirmed that every take used the neck humbucker, with picking
halfway between bridge and neck. Every damped take used fretting-hand damping,
not bridge/palm muting. Guitar make/model and exact pick remain unspecified, so
absolute tone matching is still specific to this instrument and capture chain;
the pickup, picking position, and damping boundary are now known.

## Main Measurements

### 1. Pickup matching explains most—but not all—of the spectral gap

For 60 ms onset windows:

| Material | Attack centroid | Energy below 500 Hz | Energy above 1.5 kHz |
|---|---:|---:|---:|
| Reference ringing, across groups | 220-262 Hz | 90.8-95.0% | 0.4-0.7% |
| EG-089 default single coil / 39% position | 1,593-1,625 Hz | 3.7-4.7% | 45.0-62.3% |
| EG-089 humbucker / 100% neck position | 486-537 Hz | 64.0-78.3% | 3.5-9.0% |

The 55-255 ms early-body centroid is 190-221 Hz in the reference,
1,409-1,480 Hz with the default virtual pickup, and 463-534 Hz with the matched
humbucker/neck setting. Virtual humbucker positions from 70-100% bracketed
attack centroids of 484-537 Hz, so pickup position alone cannot close the
remaining difference.

Disabling the direct pick mix, attack modes, pick transient, or contact layer
does not materially change these aggregate measurements. Each ablation's signed
residual is 51-77 dB below the production render. A combined matched-pickup
ablation is 51.7 dB below its control and leaves the same aggregate result. The
remaining mismatch is therefore in string excitation/modal/pickup geometry,
not an independently mixed pick click.

At the matched pickup setting, `String Age = 100%` moves the attack centroid to
316 Hz and energy above 1.5 kHz to 1.1%, much closer to the reference, but it
still leaves only 77.8% below 500 Hz and shortens decay further. That shows the
target region is reachable, but String Age couples spectral and decay changes
that should not be assumed for an unidentified real string.

At MIDI velocity 100 the current model derives a pluck position near 0.10 of
the speaking length. The confirmed real picking location—halfway between the
bridge and neck pickups—is plausibly farther from the bridge, though exact
normalized position requires the guitar geometry. Pluck position must be
bracketed offline before changing the universal modal envelope.

### 2. The real low E decays more slowly

Long final-event decay fits:

| Material | Down | Up / final alternate |
|---|---:|---:|
| Reference | -1.02 dB/s | -2.06 dB/s independent; -1.72 dB/s alternate |
| EG-089 default virtual pickup | -4.30 dB/s | -4.84 dB/s independent; -4.67 dB/s alternate |
| EG-089 humbucker / neck | -2.75 dB/s | -2.72 dB/s independent; -2.66 dB/s alternate |

The pickup-matched model is still faster, but the gap is now modest: about 1.3
to 2.7 times over this observed low-E range. The matched ringing early-body /
attack-peak ratios (-8.3 to -9.5 dB) closely reproduce the reference
(-8.8 to -10.0 dB), so the primary remaining time-domain issue is the long tail.

### 3. Fretting-hand damping must preserve the onset and remove the body

The confirmed fretting-hand-damped events retain attack peaks close to ringing
peaks, then place their 55-255 ms body approximately 24-28 dB below the attack
peak.
Production `Palm Mute = 100%` instead drives the body about 85 dB below the peak
and also reduces the attack heavily. With the pickup matched, a 95% palm mute
reproduces the reference body/peak ratio closely (-25.2 to -25.6 dB versus
-24.4 to -27.5 dB), but it makes damped attacks roughly 4.0-4.6 dB quieter than
modeled ringing. The real damped attacks range from 1.9 dB quieter to 1.2 dB
louder than their same-direction ringing medians.

Fretting-hand damping should therefore become a separate excitation/boundary
intervention: preserve the first contact/attack moment, then impose strong
distributed modal loss. Reusing the current palm-mute curve couples those two
effects incorrectly.

### 4. Direction and human variation are contextual

Median upstroke-minus-downstroke attack-peak differences:

| Context | Reference | Pickup-matched EG-089 |
|---|---:|---:|
| Ringing independent | +0.55 dB | +0.22 dB |
| Ringing alternate | +2.76 dB | +0.11 dB |
| Damped independent | +3.61 dB | -0.35 dB |
| Damped alternate | +2.81 dB | -0.60 dB |

The reference's 10th-to-90th-percentile attack spread is 2.9-6.0 dB depending
on direction and performance pattern. Pickup-matched EG-089 produces only
0.2-1.3 dB in the matching groups. The earlier default-pickup control reached
2.47 dB at `Player Feel = 100%` for independent downstrokes, but that does not
reproduce the wider upstroke and alternate distributions at the actual pickup.

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

First add an offline-only fixed pluck-position override while preserving EG-089
exactly as the no-override control. Keep `Pickup Model = Humbucker` and bracket
the neck-side pickup position while rendering approximately 0.10, 0.14, 0.18,
and 0.22 speaking-length pluck positions. This directly tests the confirmed
real picking-location difference without retuning the universal string.

After selecting the best geometry, build a small decay comparison:

1. pickup/pluck-matched control;
2. moderate low-E decay lift;
3. stronger low-E decay lift.

Only if pluck/pickup geometry and a plausible string-age bracket cannot close
the residual spectrum should the next experiment alter the modal envelope or
pickup electrical response. Use the analyzer to reject implausible candidates,
then prepare loudness-matched WAVs for human listening.

After the ringing foundation passes, add a separate offline fretting-hand-
damping intervention. State-preserving repick and context-dependent human
scatter should follow; widening variation before geometry/decay calibration
would only make the remaining mismatch less consistent.

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
