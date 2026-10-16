# WXRST ("The Worst Plugin", formerly Denny Did It) — project handoff notes

Lo-fi / PS2-era / Drake-OVO pad, keys, 808 and bass synth with a built-in chord + melody + 808
MIDI generator. Written entirely with Claude Code for Denny (music producer, FL Studio 2025,
Apple M5 Max, macOS 27.2). Git repo since 2026-10-11, private remote https://github.com/kubankid/wxrst (renamed from denny-did-it) (JUCE is a git submodule). No tests. Last session: 2026-10-16 evening (SAMPLER page, resizable window, collapsible rows, exp envelopes, XTRACT MIDI cleanup) - NOT tagged/released yet; Denny has not tested any of it.

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
| `PluginEditor.h/.cpp` | `AnimatedBackground` (6 aurora blobs at 1/8 scale, 6 fps + ~90 header particles at 24 fps, ~4% of one core, toggled by "~" button, saved in state), `Knob`, `Panel`, `Scope`, `EnvDisplay`, `MidiDragPad` (kind 0/1/2, external file drag into FL), `DiceButton` (real die with pips, rolls a random face per click), `TabButton` ("MIDNIGHT" header tab), `IcewaterEditor` (DragAndDropContainer; `applyTheme()` swaps colours, refreshes label colours, `sendLookAndFeelChange`, re-renders background). Midnight background: magenta/violet/orange haze blobs, perspective grid scrolling below the header, striped sunset sun behind the logo, twinkling stars. Theme FOLLOWS THE PRESET (Denny's call): `syncThemeToPreset()` runs on every program change - Synthwave / 16-Bit category -> Midnight, anything else -> ICE. The MIDNIGHT tab is a manual override until the next preset change; turning it on jumps the category combo to Synthwave. Nothing theme-related is saved in state. Denny wants playable instruments only, no one-shot "hit" presets. Env `ICEWATER_THEME=0/1` overrides for screenshots. Design size 980 wide (scaled; see 2026-10-16 evening notes), SAMPLE row at y 642, chord engine below it. Logo text "WXRST" (34pt), subtitle "THE WORST PLUGIN / ..." (differs per theme). |

Editor layout: header (anim "~", category combo, </> , preset combo, dice icon, MIDNIGHT tab, SAVE, scope) → panels OSC A, OSC B, FILTER,
AMP ENV, FILTER ENV, LFO, PITCH/VOICE (Poly/Mono, P.ENV, DECAY, GLIDE), GLOBAL (SUB, NOISE, DRIFT, WIDTH, MASTER),
LO-FI (BITS, RATE, TONE, WOW, WOW RATE, HEAT, DIST, CRACKLE), CHORUS, DELAY, REVERB → SAMPLE (waveform drop zone, LOAD / TO MIDI / CHOP IT, LEVEL ROOT TUNE START END, LOOP REV CHOP toggles, DRAG NOTES / DRAG CHOPS pads) → CHORD ENGINE panel at bottom
(KEY/SCALE/STYLE/LEN/FEEL combos, STRUM, GENERATE, PLAY; three rows CHORDS / MELODY / 808, each = enable toggle +
drag pad + sound picker; chords row shows "main sound (knobs above)").

## WXRST XTRACT (companion plugin, 2026-10-15)

Separate repo `/Users/denny/Documents/XTRACT` (private remote https://github.com/kubankid/wxrst-xtract, CI builds Win + mac).
Denny's brief (voice messages 2026-10-15): isolate stems from any sample, convert any stem to MIDI, feed WXRST's rows.
- JUCE effect plugin "WXRST XTRACT" (code `Wxtr`, VST3 Fx / AU aufx / Standalone, universal). CMake target `XTRACT`,
  JUCE taken from `../VST1/JUCE`. Build: `cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"`
  then `cmake --build build -j12` (installs into ~/Library/Audio/Plug-Ins itself; AU `auval -v aufx Wxtr Dnny` passes).
- Stem separation = demucs.cpp (vendored `third_party/demucs`, Eigen + Accelerate BLAS on mac). 4-stem htdemucs default
  (drums/bass/other/vocals); 6-stem adds guitar/piano but its piano output was weak in testing (4-stem "other" caught an
  e-piano at 10 dB SDR). Weights are NOT in the repo: the plugin downloads them from huggingface (Retrobear/demucs.cpp) into
  `~/Library/Application Support/WXRST/models/` (Windows `%APPDATA%\WXRST\models`). ~15 s for a 16 s clip on the M5 Max.
- Audio->MIDI = Spotify Basic Pitch via basicpitch.cpp + prebuilt ONNX Runtime 1.20.1 (FetchContent; dylib copied into
  Contents/Frameworks with @loader_path rpath; Windows delay-loads onnxruntime.dll from the plugin folder - untested).
  Drums use my own 3-band spectral-flux onset detector (`Transcriber.cpp`: kick 36 / snare 38 / closed hat 42 / open 46),
  tuned on Apple Loops so a disco beat gives 4-on-the-floor + 8th hats and a hip hop loop gives alternating kick/snare.
- UI: SOURCE (LOAD / REC captures the insert's input / PLAY / LOOP / HEAR ORIGINAL-STEMS / region select on the waveform /
  model combo / SEPARATE or GET MODEL) + 6 stem rows (M, S, level, TO MIDI, DRAG WAV, DRAG MIDI, TO WXRST: CHORDS MELODY DRUMS).
  MIDI BPM box (blank = host). Stems + MIDI land in `~/Documents/WXRST/Xtract/<clip>/`; "TO WXRST" copies into
  `~/Documents/WXRST/Inbox/{Chords,Melody,Drums}/`.
- Test hooks: env `XTRACT_FILE=<audio>` preloads, `XTRACT_AUTO=1` separates + converts everything, `XTRACT_SNAPSHOT=<png>`
  renders the editor to a PNG (screen capture was black on this machine - use this instead). Console test
  `-DXTRACT_BUILD_TEST=ON` -> `build/XTRACT_TEST_artefacts/Release/XTRACT_TEST <file> [--6] [--drums] [outdir]`.
- WXRST side (same date): chord engine has a 4th DRUMS row (built-in `DrumKit.h` synth for GM drum MIDI + a WAV loop that
  plays in sync with PLAY, `Progression::drumEvents`, never generated), every row has a LOAD button (menu of the three Inbox
  folders newest-first, Browse..., Clear; `IcewaterProcessor::loadLayerMidi/loadDrumLoop`, `ChordGen::readMidiFile`),
  loaded files are saved in state (`layerFileN`, `drumLoop`). SAMPLE section got a HALF toggle (`smpHalf`: half speed, octave
  down - tape style, not pitch-preserving). Window is now 980x1030. Env `WXRST_AUTOLOAD=1` / `WXRST_SNAPSHOT=<png>` for tests.

## 2026-10-16 evening session (Denny's feedback: "MIDI extractor is kind of ass", chops unplayable, sample plays under the synth, window too big, 808 clips at note end)

- **XTRACT MIDI** (`Transcriber.cpp`): Basic Pitch's raw detection was fine (100% recall on synthetic tests); the mess was
  octave/harmonic ghost notes + held chord notes split at every melody attack + nothing on the grid. Now: cleanup pass
  (ghosts at +12/+19/+24/+28/+31/-12/-24 st that are clearly weaker get dropped; split notes re-joined when the onset is
  explained by a harmonic neighbour), `NoteOptions{cleanup, mono}` (MONO toggle; bass stem always mono), `snapToGrid`
  (SNAP 1/16 toggle at the MIDI BPM, default on), file names containing "140bpm"/"_140_" prefill the BPM box. The vendored
  basicpitch lib now exposes `infer_note_events()` + `frame_times()` and its std::cout spam is gone. Bench:
  `cmake -DXTRACT_BUILD_TEST=ON`, `build/NOTES_BENCH_artefacts/Release/NOTES_BENCH [-v]` (synthetic ground-truth suites) or
  `NOTES_BENCH <wav> <bpm> [out.mid] [--raw] [--mono] [--snap=4]`. Known: Basic Pitch onsets run ~10 ms early on sharp attacks.
- **WXRST SAMPLER page**: header tab SAMPLER (next to MIDNIGHT) swaps rows 1-4 for one big panel: big waveform with numbered
  slice markers, LOAD / TO MIDI / CHOP IT, LOOP REV CHOP HALF + **SAMPLE ONLY (mute synth)** (`smpOnly`, default ON = keys play
  just the sample while one is loaded; Engine feeds the synth an empty MidiBuffer), knobs LEVEL ROOT TUNE START END +
  the sampler's own ATTACK DECAY SUSTAIN RELEASE CUTOFF RES (`smpA/D/S/R/Cut/Res`; per-voice SVF LP in `SamplePlayer`,
  no longer borrows the synth amp env), 32 `SlicePads` (8x4; click = `auditionNote()` through a lock-free FIFO into the
  main layer; lit from `SamplePlayer::activeSlices()`). **Chops now start at MIDI 60 = FL's C5 and wrap** (`chopBaseNote`,
  `sliceForNote`) - the old C3 (48) base is why Denny "couldn't play it" after CHOP IT. `chopEvents` MIDI uses 60+i too.
  Loading/dropping a sample or hitting CHOP IT jumps to the SAMPLER page. The small SAMPLE row on the SYNTH page still exists
  but is collapsed by default.
- **Slicer** `detectSlices(s, max, bpm)` is adaptive: thresholds 1.6 -> 0.32 until ~2 slices/sec (4..32), beat-grid fallback
  when the audio is too smooth. The Splice lead loop went from 4 to 45 chops.
- **Window**: everything lives in `content` (980 wide design size) scaled by `uiScale` via AffineTransform; `setResizable`,
  aspect locked, limits 0.5x..2x, scale saved in state ("uiScale"). CHORD ENGINE and SAMPLE rows collapse to a 28 px title
  bar via the -/+ button at the right of the title (state props "uiChordsOpen", "uiSampleOpen", page = "uiPage"). The window
  height changes with page + collapse state (`baseHeight()`); `applyWindowSize()` is the only place that calls setSize.
  Env `WXRST_PAGE=1` opens the sampler page, `WXRST_AUTOCHOP=1` chops the preloaded sample after 10 ticks (snapshots).
- **Envelopes**: `ExpADSR` (SynthVoice.h) replaces juce::ADSR for synth voices and the sampler: linear attack, exponential
  decay / release. juce::ADSR is linear, which is why the 808s (sustain 0, release 0.25 s) sounded like they clipped at the
  end of the note. Same `juce::ADSR::Parameters`, so presets are untouched (they will sound a touch longer-tailed).
- Not done / still weak: WXRST's own TO MIDI (`SampleAnalysis::extractNotes`) is still the mono autocorrelation tracker - for
  chords/polyphony use XTRACT. Denny's "Mega Dragon Dropable" remark (voice-to-text) was read as "flip it like ..." -> the
  sampler sound controls; ask him if he meant something else.

## In-plugin updater + public downloads (2026-10-16, late)

- Denny asked to "push updates to the versions installed on my boys' computers". Answer: `Source/Updater.h` (IDENTICAL copy in
  VST1, XTRACT and KeyFinder - edit one, copy to the others) + `UpdateBanner`. On construction the processor calls
  `updater.checkNow()`: at most every 6 h it fetches `https://raw.githubusercontent.com/kubankid/denny-plugins-releases/main/version.json`
  (PUBLIC repo, binaries only - Denny OK'd this; source repos stay private), compares `DENNY_VERSION` (CMake
  `project(... VERSION)` passed as a compile definition) with the manifest, and the editor shows a banner strip
  (WXRST: header at (330,56,506,22) next to the subtitle; XTRACT: where the status label sits; Key Finder: under the header).
  Click = download zip to ~/Downloads, unzip, macOS: copy bundles into ~/Library/Audio/Plug-Ins/{VST3,Components} and say
  "restart FL"; Windows: launch `install-windows.bat` from the zip (UAC prompt, xcopy into Common Files\VST3). "x" dismisses
  that version. Cache/throttle file: `~/Library/DennyPlugins/updates-<key>.json` (note: juce userApplicationDataDirectory is
  ~/Library on mac, not Application Support). The tiny version label under the scope (click = force check).
- Env for tests: `DENNY_UPDATE_URL=<manifest url or path>`, `DENNY_UPDATE_FORCE=1`, `DENNY_NO_UPDATE=1`,
  `DENNY_UPDATE_AUTOINSTALL=1` (clicks the banner). Tested end-to-end on this Mac with a fake 9.9.9 manifest served by
  `python3 -m http.server` (banner, download, install into ~/Library/Audio/Plug-Ins verified).
- Gotcha: `UpdateBanner updateBanner { proc.updater }` MUST be declared after `proc` in the editor class (dangling ref crash).
- CI (`.github/workflows/build.yml` in all three repos): `ci/sign-mac.sh dist <zip>` does Developer ID codesign (runtime,
  timestamp, nested dylibs first) + `notarytool submit --wait` + `stapler staple` when secrets `MACOS_CERT_P12` (base64 .p12),
  `MACOS_CERT_PASSWORD`, `APPLE_ID`, `APPLE_TEAM_ID`, `APPLE_APP_PASSWORD` exist, else it just zips (unsigned) with a warning.
  Windows zips now contain `installer/install-windows.bat`. The release job fails if the git tag != `v<CMake VERSION>`.
  Denny has a paid Apple Developer account but the secrets were NOT set yet as of 2026-10-16 - the v1.2.0 / v1.1.0 / v1.0.1
  builds are unsigned; after he adds the secrets, re-cut tags (x.y.z+1) so the published zips are notarized. No Windows
  Authenticode cert (SmartScreen only hits the standalone .exe; VST3 in FL is fine).
- Versions now: WXRST 1.2.0, XTRACT 1.1.0, Key Finder 1.0.1 (tags pushed 2026-10-16). `~/Documents/DennyPlugins/publish.sh`
  (replaces bundle.sh) pulls each repo's latest release, uploads per-plugin zips (XTRACT models stripped) to
  `denny-plugins-releases` release `<key>-v<ver>`, rebuilds the combined bundles under `latest`, and PUTs `version.json` via
  `gh api`. Run it after every re-tag. Friends' existing installs (pre-updater) must install once by hand.

## Releases + download hub (2026-10-16)

- Tags so far: wxrst `v1.2.0`, wxrst-xtract `v1.1.0`, denny-key-finder `v1.0.1` (earlier: v1.1.0 / v1.0.0 / v1.0.0) (each repo's CI attaches Windows + macOS zips
  to a GitHub Release on a `v*` tag). To ship a new version: commit, `git tag vX.Y.Z && git push --tags`, `gh run watch`.
- `~/Documents/DennyPlugins` (PRIVATE repo https://github.com/kubankid/denny-plugins) = scripts + READMEs; downloads now live on the
  PUBLIC binaries-only repo kubankid/denny-plugins-releases (see updater section). Old notes: `bundle.sh` (now `publish.sh`) pulled the latest release zips from the three repos,
  unzips into work/win + work/mac, STRIPS the Windows models folder (158 MB upload stalled at ~55 KB/s; XTRACT downloads
  models itself on first use anyway), zips `Denny-Plugins-Windows.zip` (29 MB) + `Denny-Plugins-macOS.zip` (85 MB) and
  uploads them with `--clobber` to the release tagged `latest` (`gh release edit latest --draft=false` afterwards or it
  stays an "untagged" draft). README links: `.../denny-plugins/releases/latest/download/Denny-Plugins-{Windows,macOS}.zip`.
  Re-run `bundle.sh` after any plugin re-tag. `.github/workflows/bundle.yml` does the same via workflow_dispatch but needs a
  `BUNDLE_TOKEN` secret (not set up).
- Private repo = only GitHub collaborators can download. Denny asked about "anyone with the link"; GitHub can't do that
  for private repos. He dropped it, but if it comes back: upload the two zips from `work/` to Google Drive / Dropbox
  (`/mcp` -> "claude.ai Google Drive" to authenticate) and share "anyone with the link".
- Windows builds of XTRACT (ONNX Runtime delay-load) and the HALF/DRUMS WXRST build have NOT been run on Denny's PC yet.

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
