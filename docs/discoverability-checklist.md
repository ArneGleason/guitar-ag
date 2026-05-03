# Guitar AG Discoverability Checklist

Purpose: make the project findable by people who are already looking for a lightweight physical-model electric guitar instrument, without turning it into a promotional campaign.

## Repository Basics

- [ ] Choose and add an explicit license before inviting people to reuse or build on the code.
  - Until then, describe the project as source-available rather than open-source.
  - Because the project uses JUCE, review the current JUCE licence/AGPL/commercial terms before choosing the project license.
- [ ] Add GitHub repository topics:
  - `vst3`
  - `juce`
  - `audio-plugin`
  - `physical-modeling`
  - `mpe`
  - `guitar-synth`
  - `virtual-instrument`
  - `synthesizer`
  - `music-technology`
  - `dsp`
  - `cpp`
  - `electric-guitar`
  - `sample-free`
  - `plugin`
  - `audio`
- [ ] Add a GitHub social preview image.
  - Suggested image: the Bitwig screenshot or a simple title card that says `Guitar AG - physical-model electric guitar VST3`.
- [ ] Keep GitHub release assets named consistently:
  - `Guitar-AG-vX.Y.Z-macOS-VST3.zip`
  - `Guitar-AG-vX.Y.Z-Windows-VST3.zip`
- [ ] Add short install notes to each release.

## Demo Assets

- [ ] Clean DI demo:
  - Path: `assets/demo/guitar-ag-v0.2.0-clean-di.mp3`
  - Goal: prove it is a modeled electric guitar source, not an amp sim and not a sample library.
- [ ] Amp-sim context demo:
  - Path: `assets/demo/guitar-ag-v0.2.0-through-amp-sim.mp3`
  - Goal: show how the DI tone sits before a normal amp/cab simulator.
- [ ] MPE independent bend demo:
  - Path: `assets/demo/guitar-ag-v0.2.0-mpe-bend.mp3`
  - Goal: hold a chord and bend one note while the others stay stable.
- [ ] Player-articulation demo:
  - Path: `assets/demo/guitar-ag-v0.2.0-articulation.mp3`
  - Goal: show picked notes, hammer-ons, pull-offs, and taps.
- [ ] Amp feedback demo:
  - Path: `assets/demo/guitar-ag-v0.2.0-feedback.mp3`
  - Goal: demonstrate the dominant-band feedback/takeover behavior.

Keep each demo short. The first 10 seconds should answer what the plugin is.

## Suggested Low-Key Posts

Recommended post title:

```text
Guitar AG: early physical-model electric guitar VST3 for piano-roll composition and MPE
```

Short post body:

```text
I've been looking for a lightweight physical-model electric guitar VST for years, especially one that can be composed in a DAW piano roll without a large sample library or a keyswitch-heavy workflow. I did not find quite the thing I wanted, so I started a small source-available JUCE/C++ VST3 project.

It is early and experimental, not a finished commercial instrument. Current builds are available for macOS and Windows. It generates a clean DI-style modeled electric guitar tone, supports independent modeled string voices, MPE pitch bend, MPE pressure/CC74 routing, automatable articulation and performance controls, pickup controls, and a first feedback-like sustain model.

It can be played live, but my main use case is authored guitar parts: write notes in the piano roll, draw expression/automation curves, and use a tunable amount of built-in player interpretation instead of programming everything with keyswitches.

I'm mainly interested in practical feedback from people who care about physical modeling, expressive controllers, or audio plugin development:

- Does it load in your DAW?
- Does MPE bend only the intended note?
- Does the modeled DI tone seem worth developing further?
- Does the automatable, keyswitch-free articulation approach make sense for piano-roll composition?
- Are there obvious DSP or plugin-hosting issues?

Repo/releases:
https://github.com/ArneGleason/guitar-ag
```

## Places To Share

- [ ] KVR Audio Instruments forum: start a new topic.
  - Forum: https://www.kvraudio.com/forum/viewforum.php?f=1
  - Good fit because KVR has long-running VST/plugin and physical-modeling discussion.
  - Similar reference thread style: RipplerX open-source physical-model synth thread: https://www.kvraudio.com/forum/viewtopic.php?t=618505
- [ ] KVR Audio product/developer database: submit a product listing later, after license and demos are ready.
  - Developer/product submission: https://www.kvraudio.com/submissions/
- [ ] JUCE Forum, Audio Plugins category: start a new topic asking for developer/host feedback.
  - Category: https://forum.juce.com/c/audio-plugins/9
  - Keep the ask technical: CMake/JUCE/VST3 build, host compatibility, real-time safety, MPE routing.
- [ ] JUCE Forum, General JUCE discussion: optionally reply only if there is an active physical-modeling discussion.
  - Relevant thread: https://forum.juce.com/t/physically-modeling-instruments/60224
  - Do not drive-by promote; reply only if the thread context still fits.
- [ ] Reddit `r/AudioProgramming`: post as a technical project/feedback request.
  - Subreddit: https://www.reddit.com/r/AudioProgramming/
  - Best angle: real-time modeled guitar DSP, JUCE/C++, MPE routing, open project notes.
- [ ] Reddit `r/JUCE`: post as a JUCE plugin project seeking build/host feedback.
  - Subreddit: https://www.reddit.com/r/JUCE/
  - Similar style: people share VST/JUCE projects and ask for feedback.
- [ ] Reddit `r/musicprogramming`: post as a music-tech coding project.
  - Subreddit: https://www.reddit.com/r/musicprogramming/
- [ ] Reddit `r/linnstrument`: consider a careful reply or new post after the MPE demo exists.
  - Highly relevant old thread: https://www.reddit.com/r/linnstrument/comments/11tg54e
  - This thread is specifically about the lack of physically modeled bass/guitar-style MPE instruments. Since it is old, a new post may be cleaner than reviving it.
- [ ] The Audio Programmer community: share after the README/license/demo basics are ready.
  - Community page: https://www.theaudioprogrammer.com/learn
  - Best angle: early JUCE/C++ audio plugin, source-available, looking for DSP/build feedback.
- [ ] Roger Linn / LinnStrument-adjacent KVR discussion: consider after an MPE-focused demo exists.
  - Relevant thread about MPE and physical modeling scarcity: https://www.kvraudio.com/forum/viewtopic.php?p=8332869
  - Use restraint; reply only if the discussion context fits.

## Suggested Order

1. Add license.
2. Add GitHub topics.
3. Add clean DI and MPE bend demos.
4. Add social preview image.
5. Post once on KVR Instruments.
6. Post once on JUCE Audio Plugins.
7. Post once on `r/AudioProgramming` or `r/JUCE`, not every subreddit at once.
8. Watch for actual feedback before posting elsewhere.
