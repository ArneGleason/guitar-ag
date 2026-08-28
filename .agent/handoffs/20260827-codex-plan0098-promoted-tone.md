# Plan 0098 Promoted Tone Handoff — 2026-08-27

## Outcome

- Branch: `codex/0098-promote-accepted-tone`
- Model: `StringVoice EG-089 ModalFormantTone`
- The exact human-approved Plan 0097 recipe is now the production VST3 and
  no-flags offline-render default.
- No VST parameters, preset fields, or state format changed.
- `--legacy-tone-recipe previous` restores the former EG-088 tone offline.

## Exact Transfer Evidence

The new default matches the accepted Plan 0097 files byte for byte in all three
sections, and the previous recipe matches all three former-production files:

- ordinary: new `1904D2B3BAA6CC39123DBA9B0AF6E3D73916C780A695C5244C02C00DAE16432B`;
  previous `3B1675EE14BE4CBC172ECC4566A885447B311FD558AA00816001F4D3479B81AE`
- deep: new `ABF261ECD386B652755244D6A63786E6DB4A8899E1948DDF263750EC770AC041`;
  previous `96B2B0EB6B77C415128F9D2DAD83F6700073FB5D56070A7B5E9B9053745D7112`
- phrase: new `F7E17BB93FBC5507B9B8F7DAFFAFA6D00B05748722B8B5C26532C52641651C11`;
  previous `93C89AB747E71BE995FFC394FB8E740C3D9BA69AB261A599A4F88225B48DF9FC`

Repeated new single-note renders match at
`32AF0A4AE9A04B3E77F74161E963D4AE3E8431433012D642EA13A96C2C5C47D7`.
The previous recipe reproduces canonical EG-088 at
`C67DCE0C59AA6D0A903BA887E2C55953B5842CAF1CA3160C035D0704BF0BD48B`.

## Human Verification

Build artifacts:

- `C:\code\github\guitar-ag\build-vs2022-x64\GuitarAG_artefacts\Release\VST3\Guitar AG.vst3`
- `C:\code\github\guitar-ag\build-vs2022-x64\GuitarAGOfflineRender_artefacts\Release\GuitarAGOfflineRender.exe`

Install the Release VST3 through the documented Windows workflow, rescan/reload
it in the DAW, and confirm:

1. the header reads `v0.3.1 / StringVoice EG-089 ModalFormantTone`
2. the same ordinary/deep/phrase material has the approved track-2 tone
3. note release, polyphony, and MPE behavior remain normal

The current Codex process does not have write access to `C:\Program Files`.
Its install attempt was denied before the existing EG-088 binary changed, and no
user-local duplicate was created. Run the replacement from an elevated shell.
