# Low/High-E Endpoint Comparison — 2026-08-28

## Result

The open-high-E captures are usable and show that Plan 0105's low-E decay
correction cannot be applied uniformly. The real high E loses energy much
faster than the real low E, while EG-089 does the opposite tonal mistake: its
high E is substantially too bright, too loud, and too persistent.

An offline endpoint candidate keeps the accepted 0.14 pluck position and 1 ms
de-click, leaves the accepted low-E output byte-identical, and adds two smooth
E2-to-E4 interpolations:

- body decay time scale from 1.50x at E2 to 0.50x at E4;
- upper-partial damping of `exp(-0.25 * register * (harmonic - 1)^1.2)`.

This is a human listening candidate, not a production parameterization.

## Evidence Quality

- two approved 44.1 kHz/24-bit batches for high-E independent down, independent
  up, and 12-stroke down-first alternate picking;
- six detected attacks in every independent take and 12 in every alternate;
- zero clipped samples, no writer-overflow flags, and no exact-zero run longer
  than two samples in the reference WAVs;
- final independent tails of approximately 4–6 seconds;
- high-E fundamental medians of approximately 329.56–329.89 Hz.

The independent strokes were played faster than the written stop/reset recipe,
but they contain all requested attacks and adequate final tails. They are valid
for onset distributions and final-decay fitting.

## Main Measurements

All spectra use onset-aligned 60 ms attack windows. The table reports the median
for each group at the matched virtual humbucker/neck position.

| Context | Source | Centroid | Below 500 Hz | Above 1.5 kHz | Decay |
| --- | --- | ---: | ---: | ---: | ---: |
| High E down | reference | 1,015 Hz | 14.1% | 9.7% | -7.35 dB/s |
| High E down | production EG-089 | 1,728 Hz | 1.0% | 62.6% | -3.92 dB/s |
| High E down | uniform Plan 0105 | 1,753 Hz | 2.1% | 49.3% | -2.34 dB/s |
| High E down | endpoint candidate | 960 Hz | 14.2% | 12.1% | -6.90 dB/s |
| High E up | reference | 838 Hz | 20.2% | 4.3% | -7.13 dB/s |
| High E up | production EG-089 | 1,668 Hz | 2.3% | 53.9% | -3.66 dB/s |
| High E up | uniform Plan 0105 | 1,714 Hz | 3.5% | 47.1% | -2.31 dB/s |
| High E up | endpoint candidate | 849 Hz | 22.2% | 8.7% | -6.75 dB/s |
| High E alternate up | reference | 854 Hz | 19.5% | 5.3% | -5.35 dB/s |
| High E alternate up | production EG-089 | 1,676 Hz | 2.0% | 57.9% | -3.79 dB/s |
| High E alternate up | endpoint candidate | 841 Hz | 23.0% | 8.6% | -6.77 dB/s |

The endpoint candidate also moves median high-E attack peaks near the recording:
approximately -19.6 dBFS down and -17.1 dBFS up, versus -20.0/-17.4 dBFS in
the references. Production is roughly -8.8/-7.6 dBFS under the same render
conditions. These absolute values remain performance/velocity dependent; their
main use is rejecting the production model's extreme high-string excess.

## Interpretation

Pluck position alone cannot solve the high-E mismatch. Across fixed positions
0.08–0.28, the model retained approximately 47–74% above 1.5 kHz. Raising the
existing register envelope anchor to 100% and removing metal restoration still
left approximately 30–43% above 1.5 kHz. String Age at 100% reached only
approximately 38–41% and coupled the correction to decay.

The smallest useful hypothesis is therefore a string/register-dependent modal
loss above the fundamental, not more pickup-position guessing or a global
old-string control. A 1.2-power loss curve preserves the first few partials
better than a linear harmonic slope while removing the measured excess above
1.5 kHz.

The alternate reference decays more slowly than independent high-E strokes,
whereas the restart-based model gives nearly the same slope. The candidate does
not hide that residual. Plan 0106 removes the click but does not yet preserve
moving-string energy across a repick.

High-E muting is not needed for this decision. Request it later only if a
specific plain-string damping/contact question remains after the ringing
candidate is heard.

## Listening Gate

The ignored listening directory is:

```text
build-vs2022-x64/diagnostics/eg0108-low-high-e-endpoint/listening
```

Tracks 01–04 are 14.45-second, mono, 44.1 kHz/24-bit montages normalized to
-20 dBFS RMS: real reference, production EG-089, the uniform low-E candidate,
and the endpoint candidate. Tracks 05–08 provide order-balanced reference/
endpoint and production/endpoint comparisons. No file clips.

Listen for native high-string scale, remaining brittle fizz, loss of legitimate
metal/string identity, down/up balance, tail length, and whether alternate
picking sounds unnaturally reset despite the accepted de-click.

## Reproduction

Generate endpoint MIDI, render the named model sets, then run:

```powershell
python scripts\analyze-low-high-e-endpoints.py `
  --capture-root "$([Environment]::GetFolderPath('MyDocuments'))\Guitar AG Reference Captures" `
  --render-directory build-vs2022-x64\diagnostics\eg0108-low-high-e-endpoint `
  --output-directory build-vs2022-x64\diagnostics\eg0108-low-high-e-endpoint\final-analysis `
  --model-set production=production --model-set accepted=accepted --model-set endpoint=endpoint

python scripts\create-endpoint-candidate-listening-set.py `
  --capture-root "$([Environment]::GetFolderPath('MyDocuments'))\Guitar AG Reference Captures" `
  --render-directory build-vs2022-x64\diagnostics\eg0108-low-high-e-endpoint `
  --output-directory build-vs2022-x64\diagnostics\eg0108-low-high-e-endpoint\listening
```

Omitting the new controls and explicitly setting `auto`/zero both reproduce the
pre-change high-E control at SHA-256
`7590F5AC36C36E43F7C4E146FEE436C25E5B116741B6968BAE84CAC96C22EDBE`.
The endpoint candidate's three low-E renders exactly match the accepted Plan
0105/0106 low-E renders.
