# WXRST ("The Worst Plugin", formerly Denny Did It) — project handoff notes

Lo-fi / PS2-era / Drake-OVO pad, keys, 808 and bass synth with a built-in chord + melody + 808
MIDI generator. Written entirely with Claude Code for Denny (music producer, FL Studio 2025,
Apple M5 Max, macOS 27.2). Git repo since 2026-10-11, private remote https://github.com/kubankid/wxrst (renamed from denny-did-it) (JUCE is a git submodule). No tests. Last session: 2026-10-11.

## Build & install

```
cd /Users/denny/Documents/VST1
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build --config Release -j12
auval -v aumu Wxrs Dnny          # AU validation, passes
```

- JUCE 8.0.10 lives in `./JUCE` (subdirectory, not a submodule). CMake 4.4.3, Apple clang, C++17.
- CMake target is still `ICEWATER` (original name). PRODUCT_NAME "WXRST" (renamed from "Denny Did It" on 2026-10-11 at Denny's request; PLUGIN_CODE changed to `Wxrs` so it coexists with old "Denny Did It" installs on Windows),
  manufacturer `Dnny`, company DennyAudio, bundle id com.DennyAudio.ICEWATER.
- Formats: VST3, AU, Standalone. COPY_PLUGIN_AFTER_BUILD installs to
  `~/Library/Audio/Plug-Ins/VST3/WXRST.vst3` and `~/Library/Audio/Plug-Ins/Components/WXRST.component` (old "Denny Did It" bundles were deleted).
- Artefacts: `build/ICEWATER_artefacts/Release/{VST3,AU,Standalone}/`. Standalone app is handy for quick visual checks
  (`open "build/ICEWATER_artefacts/Release/Standalone/WXRST.app"`), then screenshot with `screencapture`.
- Universal binary (arm64 + x86_64) is required: FL Studio reported "plugin was installed incorrectly" with the
  earlier arm64-only build. Old `ICEWATER.vst3/.component` bundles were deleted. User must rescan in FL:
  Options → Manage plugins → tick "Rescan previously verified plugins" → Start scan → find "WXRST" under W.
  **Whether this fixed the FL error is still unconfirmed by the user.** If not, check `~/Library/Application Support/Image-Line/` logs.
- Scripted UI clicks via osascript hit Terminal instead of the plugin; verify UI with screenshots.

### Windows build (via GitHub Actions)

- `.github/workflows/build.yml` builds Windows (default Visual Studio generator on windows-latest, x64, VST3 + Standalone .exe, static MSVC runtime) and macOS
  (universal VST3/AU/Standalone) on every push to `main`; artifacts download from the Actions tab. Pushing a `v*` tag
  also attaches zips to a GitHub Release.
- CMakeLists is platform-aware: AU + COPY_PLUGIN_AFTER_BUILD only on APPLE; falls back to FetchContent for JUCE 8.0.10
  if `./JUCE` is missing. Windows install = copy `WXRST.vst3` into `C:\Program Files\Common Files\VST3\`.
- Pushing workflow files needs the gh token to have the `workflow` scope: `gh auth refresh -h github.com -s workflow`.

## Source map (`Source/`, ~2,800 lines)

| File | What it holds |
|---|---|
| `Params.h` | `ID::` namespace of all APVTS parameter ids; `waveNames`, `lfoShapeNames`, `filterTypeNames`, `voiceModeNames`; `createParameterLayout()`. ~60 params. |
| `SynthVoice.h` | `VoiceParams` (has `mutable std::atomic<float> lastFreq`, `mutable std::atomic<int> activeCount` for mono glide), `SVF` (TPT, `process(v, type)` 0 LP / 1 BP / 2 HP), `IceVoice` (polyBLEP saw/square/pulse, tri, sine, "Glass"; up to 7-voice unison w/ detune + spread; sub; noise; drift; pitch env; glide; mono last-note priority). |
| `Engine.h` | `class Engine`: one complete synth + FX chain. `static defaults()` map (filled from the real processor's params in its ctor), `setToDefaults()`, `loadPreset(const Preset&)`, `prepare()`, `allNotesOff()`, `bool process(AudioBuffer&, MidiBuffer&)` (returns false when silent so tail can be skipped). Holds `std::unordered_map<std::string,float> values`. FX order: bitcrush+SR reduce → lo-fi tone LPF → tape wow → HEAT (tanh) → DIST (asym soft clip) → vinyl CRACKLE → juce::dsp::Chorus → filtered ping-pong delay → juce::dsp::Reverb → width → master. |
| `Presets.h` | `struct Preset { name, category, vector<pair<const char*,float>> values }`, `presetCategories {All, Pads, Keys, 808, Bass, Synth, Synthwave, 16-Bit, Drift, User}` ("Drift" is deliberately EMPTY - Denny wants it reserved for a future set; "User" lists saved presets, combo item ids = 10000 + index, factory ids = index + 1), `getPresets()` = 142 presets: ... + 41 "Midnight Collection" (2026-10-11): Synthwave 32 (Blade Runner "BR ..." x7, Terminator "T ..." x6, Drive OST x6, general 80s x13 incl. Africa Kalimba, Axel F, Upside Down Arp, Miami Vice, Enjoy The Silence, Moroder) and 16-Bit 9 (Mega Man X "X ..." SNES-style: 12-bit, 32 kHz, tone ~8.5k, heat+dist for the rock leads). Earlier: original 47 lo-fi set (Pads 25, Keys 8, 808 8, Bass 6) + 54 "Vintage Collection" presets added 2026-10-11 modelled on the Arturia V Collection 5 lineup (Mini, ARP 2600, CS-80, Jup-8, Prophet 5/VS, Matrix-12, SEM, Modular, Solina, Synclavier, Stage-73, Wurli, Piano V, B-3, VOX, Farfisa): Pads 15, Keys 13, 808 4, Bass 8, Synth 14 (leads/brass/strings/plucks). Vintage ones are mostly clean (16-bit/48k, crackle 0) with drift + heat; new presets are appended at the END so saved program indices stay valid. |
| `Sampler.h` | `SampleData` (audio, sampleRate, name, file, 512 peaks, slices). `SampleAnalysis`: `load()` (any JUCE format, capped 2 min), `extractNotes()` = mono pitch tracking (2048-pt FFT autocorrelation, hop 512, candidates >= 0.85 of max with octave continuity to the previous frame, median-5 smoothing, onset = first frame >= 60% of run peak, 1/32 quantise, octave fold to take median, dedupe), `detectSlices()` (pre-emphasised log-energy rise over 1024 window, hop 256, local-mean + 1.0 threshold, 80 ms min gap, max 64), `writeChops()` (24-bit WAVs to ~/Documents/WXRST/Chops/<name> chops/), `chopEvents()` (note 48+i per slice). `SamplePlayer`: 8 voices, linear interp, start/end/loop/reverse, CHOP mode = key selects slice unpitched, uses the synth's amp ADSR. Lives in `Engine::sampler`, rendered after the synth voices and BEFORE the FX chain. |
| `UserPresets.h` | `~/Documents/WXRST/Presets/*.wxrst` XML (name + every param id/value). `loadAll()`, `save()`. |
| `SampleTest.cpp` / `ChordTest.cpp` | Console tests behind `-DICEWATER_BUILD_TEST=ON` (targets SAMPLE_TEST / ICEWATER_TEST). SampleTest with no args = synthetic melody + drums (exact); with file args = reports notes + chops. Real Apple Loops in `/Library/Audio/Apple Loops/Apple/` are good test material. |
| `Chords.h` | `NoteEvent`, `Chord`, `Progression {chords, events, melodyEvents, bassEvents, totalBeats, title}`, `ChordGen` (keyNames, scaleNames, styleNames, barNames, rhythmNames, `Options{key,scale,style,bars,rhythm,strum,seed}`, `generate()`, `writeMidiFile()`, `chordListString()`). Scales: Minor, Major, Dorian, Harmonic Minor, Lydian, Phrygian. Styles: Drake/OVO, Lo-Fi, Dreamy, Sad, Uplifting, PS2 Menu, Dark Trap, Neo-Soul, Ambient. FEEL: Whole bars, Mixed, Pushed, Half bars. MIDI files written to `~/Documents/ICEWATER Chords/`. Melody rule (Denny, 2026-10-11): notes lean on the key's 1st/4th/5th degrees ("anchors"): chord starts always, downbeats 70%, stepwise runs settle on anchors 40%, final note forced to 1 or 5. Console test `ICEWATER_BUILD_TEST` (`Source/ChordTest.cpp`) reports the anchor percentages (~84% notes / ~94% downbeats). |
| `PluginProcessor.h/.cpp` | `IcewaterProcessor`: `apvts`, user presets (`userPresets`, `currentUserPreset`, `loadUserPreset`, `saveUserPreset`; state prop "userPreset" = name), sample (`loadSampleFile`, `getSample`, `extractNotesMidi`, `chopSample`; state prop "samplePath" reloads on open; env `ICEWATER_SAMPLE=<path>` preloads for screenshots), `loadPreset` resets all params EXCEPT `smp*`, `Engine engines[3]` (0 chords = main knobs, 1 melody, 2 808/bass with their own preset snapshots), `layerEnabled[3]`, `setLayerPreset/getLayerPreset` (applied on audio thread via `pendingLayerLoad`), progression playback synced to host BPM routing to `layerMidi[3]`, `loadPreset`, `randomiseSound` (DICE), programs API, state save/load (program, melodyPreset, bassPreset, layer0..2, anim flag), scope/meter atomics. |
| `LookAndFeel.h` | `IceColours` = MUTABLE inline globals + `applyTheme(0 ICE / 1 MIDNIGHT)` (Midnight = violet #0b0614 bg, magenta #ff3fa4 accent, purple #8a2be2 deep, sunset gold #ffc46b glow); `IceLookAndFeel` (`refreshColours()` re-applies after a theme switch; `drawTabButton` for the header tab). |
| `PluginEditor.h/.cpp` | `AnimatedBackground` (6 aurora blobs at 1/8 scale, 6 fps + ~90 header particles at 24 fps, ~4% of one core, toggled by "~" button, saved in state), `Knob`, `Panel`, `Scope`, `EnvDisplay`, `MidiDragPad` (kind 0/1/2, external file drag into FL), `DiceButton` (real die with pips, rolls a random face per click), `TabButton` ("MIDNIGHT" header tab), `IcewaterEditor` (DragAndDropContainer; `applyTheme()` swaps colours, refreshes label colours, `sendLookAndFeelChange`, re-renders background). Midnight background: magenta/violet/orange haze blobs, perspective grid scrolling below the header, striped sunset sun behind the logo, twinkling stars. Theme FOLLOWS THE PRESET (Denny's call): `syncThemeToPreset()` runs on every program change - Synthwave / 16-Bit category -> Midnight, anything else -> ICE. The MIDNIGHT tab is a manual override until the next preset change; turning it on jumps the category combo to Synthwave. Nothing theme-related is saved in state. Denny wants playable instruments only, no one-shot "hit" presets. Env `ICEWATER_THEME=0/1` overrides for screenshots. Window 980×988 (SAMPLE row at y 642, chord engine at 778). Logo text "WXRST" (34pt), subtitle "THE WORST PLUGIN / ..." (differs per theme). |

Editor layout: header (anim "~", category combo, </> , preset combo, dice icon, MIDNIGHT tab, SAVE, scope) → panels OSC A, OSC B, FILTER,
AMP ENV, FILTER ENV, LFO, PITCH/VOICE (Poly/Mono, P.ENV, DECAY, GLIDE), GLOBAL (SUB, NOISE, DRIFT, WIDTH, MASTER),
LO-FI (BITS, RATE, TONE, WOW, WOW RATE, HEAT, DIST, CRACKLE), CHORUS, DELAY, REVERB → SAMPLE (waveform drop zone, LOAD / TO MIDI / CHOP IT, LEVEL ROOT TUNE START END, LOOP REV CHOP toggles, DRAG NOTES / DRAG CHOPS pads) → CHORD ENGINE panel at bottom
(KEY/SCALE/STYLE/LEN/FEEL combos, STRUM, GENERATE, PLAY; three rows CHORDS / MELODY / 808, each = enable toggle +
drag pad + sound picker; chords row shows "main sound (knobs above)").

## Gotchas learned the hard way

- `setSize()` must stay at the END of the editor constructor (crashed with null deref when called before knobs existed).
- Use plain ASCII in UI strings ("·" rendered as "Â·"); use "/" or "-".
- `juce::TextButton layerBtn[3] { juce::TextButton("CHORDS"), ... }` — explicit ctor needed in the array init.
- Category combo: selecting a category loads the first preset in it; `rebuildPresetList()` must NOT force a resync
  (that was the "dropdown snaps back to All" bug).
- Animation: never let dirty-rect merging cause full-window repaints (was 16% CPU). Particles repaint header only.
- Dropping a sample sets smpLevel to 0.8 if it was 0 and clears CHOP mode; CHOP IT re-enables it. Sample panel is NOT bufferedToImage (waveform repaints).
- Theme colours are global statics: the editor destructor resets to ICE so a second instance doesn't inherit Midnight.
- `pkill -f WXRST` between standalone launches: `open` reuses a running instance, so env overrides don't apply.
- Engine defaults come from the real processor's parameters, not a dummy AudioProcessor.
- A plugin can't place MIDI onto an FL channel directly; the drag pads + in-plugin audition are the workaround.

## User preferences / style

- Denny speaks casually, wants things to "look fucking sick" but stay light on CPU. Likes the icy navy/cyan palette.
- References: Serum (layout/customizability), Drake/OVO/40 pads, Ramzoid 808s, PS2/Dreamcast menu music.
- Wants the plugin to feel finished and usable inside FL Studio, not a tech demo.

## Ideas offered but not yet requested

Melody density control; regenerate-melody-only / lock chords; save user presets; Windows build (needs a Windows machine);
rename the CMake target from ICEWATER to match the product name (would change artefact folder and MIDI output folder).
