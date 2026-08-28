# Transient-Preserving High-E Endpoint — 2026-08-28

## Human Correction

Plan 0108's amplitude-damped endpoint is rejected. In the reference/endpoint
pair, the real guitar was brighter and more convincing; the candidate sounded
dark, large, flat, and insufficiently bright. In the production/endpoint pair,
production EG-089 was preferred despite its familiar glassy/new-string edge.

This verdict outranks Plan 0108's close 60 ms centroid and band-energy match.
Those aggregates rewarded removal of upper-modal amplitude but did not represent
the perceptually useful contact brightness and temporal evolution.

## Revised Hypothesis

Keep initial modal amplitude intact and shorten only upper-partial persistence.
The new offline control applies extra modal decay above a fixed 1.2 kHz hinge:

```text
decaySeconds *= exp(-amount * E2ToE4RegisterBlend
                    * max(0, (modeFrequency - 1200) / 1200))
```

The first listening candidate uses:

- Plan 0105's 0.14 pluck position and 1.50x low-E body decay;
- a 1.00x high-E body-decay endpoint, leaving high-E fundamental decay close to
  production rather than forcing Plan 0108's very short 0.50x endpoint;
- upper-partial decay amount 2.0;
- no Plan 0108 amplitude damping;
- Plan 0106's accepted 1 ms de-click.

At exact onset this changes decay coefficients, not modal amplitudes. Low E is
byte-identical to the accepted Plan 0105/0106 candidate because the register
blend is zero there.

## High-E Bracket

| Context | Source | Initial centroid / >1.5 kHz | 60 ms centroid / >1.5 kHz | Final decay |
| --- | --- | ---: | ---: | ---: |
| Down | real reference | 1,068 Hz / 11.6% | 1,015 Hz / 9.7% | -7.35 dB/s |
| Down | production | 1,704 Hz / 62.3% | 1,728 Hz / 62.6% | -3.92 dB/s |
| Down | rejected dark endpoint | 966 Hz / 12.3% | 960 Hz / 12.1% | -6.90 dB/s |
| Down | transient candidate | 1,463 Hz / 40.5% | 1,333 Hz / 35.1% | -4.00 dB/s |
| Up | real reference | 853 Hz / 5.2% | 838 Hz / 4.3% | -7.13 dB/s |
| Up | production | 1,623 Hz / 53.3% | 1,668 Hz / 53.9% | -3.66 dB/s |
| Up | rejected dark endpoint | 865 Hz / 9.1% | 849 Hz / 8.7% | -6.75 dB/s |
| Up | transient candidate | 1,430 Hz / 38.5% | 1,292 Hz / 32.8% | -3.89 dB/s |

The revised candidate deliberately does not chase the reference aggregate all
the way. It is an audible midpoint designed around the human statement
“production is better, but a little glassy.” If it remains glassy, amount 4.0
and/or a 0.75x high-body endpoint are already bracketed. If it is again flat,
reject this modal-decay family rather than increasing it.

## Missing Low-E Reference Gate

Plan 0105's original listening set compared production and candidate but did not
include the player's real low-E recording. Plan 0109 adds the missing actual
reference comparison under:

```text
build-vs2022-x64/diagnostics/eg0109-transient-preserving-endpoint/listening-low-e
```

Tracks 04 and 05 contain reference/accepted in both orders. This gate asks
whether the already-preferred low-E candidate is actually closer to the guitar,
not merely preferable to production.

## Revised High-E Gate

The high-E set is under:

```text
build-vs2022-x64/diagnostics/eg0109-transient-preserving-endpoint/listening-high-e
```

Tracks 07 and 08 compare production and the transient candidate in both orders;
tracks 05 and 06 compare the real reference and transient candidate. Tracks 09
and 10 retain the rejected dark endpoint only as a contrast. All montages are
mono 44.1 kHz/24-bit and RMS-matched.

No production transfer or new capture should occur until both low- and high-E
reference gates receive a human verdict.
