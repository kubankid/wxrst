#include "PluginProcessor.h"
#include "PluginEditor.h"

static int findPresetByName (const char* name)
{
    const auto& p = getPresets();
    for (int i = 0; i < (int) p.size(); ++i) if (juce::String (p[(size_t) i].name) == name) return i;
    return 0;
}

IcewaterProcessor::IcewaterProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "ICEWATER", createParameterLayout())
{
    updater.checkNow();
    for (auto* p : getParameters())
        if (auto* rp = dynamic_cast<juce::RangedAudioParameter*> (p))
        {
            raw[rp->paramID] = apvts.getRawParameterValue (rp->paramID);
            Engine::defaults()[rp->paramID.toStdString()] = rp->convertFrom0to1 (rp->getDefaultValue());
        }
    for (auto& e : engines) e.setToDefaults();

    // sensible defaults for the other two layers
    setLayerPreset (melodyLayer, findPresetByName ("Glass Rhodes"));
    setLayerPreset (bassLayer, findPresetByName ("Ramzoid 808"));
    layerEnabled[bassLayer] = false;
    refreshUserPresets();
    // dev hook: preload a sample for screenshots / testing
    const auto envSample = juce::SystemStats::getEnvironmentVariable ("ICEWATER_SAMPLE", "");
    if (envSample.isNotEmpty() && juce::File (envSample).existsAsFile()) loadSampleFile (juce::File (envSample));
}

void IcewaterProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    for (auto& e : engines) e.prepare (sampleRate, samplesPerBlock);
    layerBuffer.setSize (2, samplesPerBlock);
    loopBuffer.setSize (2, samplesPerBlock);
    drumKit.prepare (sampleRate);
}

void IcewaterProcessor::auditionNote (int note, bool on)
{
    const auto w = uiFifo.write (1);
    if (w.blockSize1 == 1) uiQueue[(size_t) w.startIndex1] = on ? juce::MidiMessage::noteOn (1, note, (juce::uint8) 110) : juce::MidiMessage::noteOff (1, note);
}

bool IcewaterProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono();
}

void IcewaterProcessor::setLayerPreset (Layer l, int presetIndex)
{
    if (l == chordsLayer || ! juce::isPositiveAndBelow (presetIndex, (int) getPresets().size())) return;
    (l == melodyLayer ? melodyPreset : bassPreset).store (presetIndex);
    pendingLayerLoad[l].store (presetIndex);
}

void IcewaterProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    const int n = buffer.getNumSamples();
    buffer.clear();
    if (buffer.getNumChannels() < 2) return;

    // main layer follows the knobs
    for (auto& [id, ptr] : raw) engines[chordsLayer].values[id.toStdString()] = ptr->load();
    { const juce::ScopedTryLock sl (sampleLock); if (sl.isLocked()) engines[chordsLayer].sampler.setSample (sample); }
    // other layers: apply any preset change requested from the UI
    for (int l = 1; l < 3; ++l)
    {
        const int idx = pendingLayerLoad[l].exchange (-1);
        if (idx >= 0) { engines[l].loadPreset (getPresets()[(size_t) idx]); engines[l].allNotesOff(); }
    }

    if (auto* ph = getPlayHead())
        if (auto pos = ph->getPosition())
            if (auto bpm = pos->getBpm()) hostBpm.store (*bpm);

    // ---- Chord player ------------------------------------------------------------
    for (auto& lm : layerMidi) lm.clear();
    loopBuffer.setSize (2, n, false, false, true);
    loopBuffer.clear();
    layerMidi[chordsLayer].addEvents (midi, 0, n, 0);   // live MIDI always plays the main sound
    {   // pads on the SAMPLER page
        const auto r = uiFifo.read (uiFifo.getNumReady());
        for (int i = 0; i < r.blockSize1; ++i) layerMidi[chordsLayer].addEvent (uiQueue[(size_t) (r.startIndex1 + i)], 0);
        for (int i = 0; i < r.blockSize2; ++i) layerMidi[chordsLayer].addEvent (uiQueue[(size_t) (r.startIndex2 + i)], 0);
    }

    if (chordStopRequested.exchange (false))
    {
        for (int l = 0; l < numLayers; ++l)
        {
            for (int note : heldNotes[l]) layerMidi[l].addEvent (juce::MidiMessage::noteOff (1, note), 0);
            heldNotes[l].clear(); nextEvt[l] = 0;
        }
        drumKit.allNotesOff();
        playBeat = 0.0; drumLoopPos = 0.0; currentChord.store (-1); playPosition.store (0.0);
    }

    if (chordPlaying.load())
    {
        std::shared_ptr<const Progression> prog;
        std::shared_ptr<const SampleData> loop;
        { const juce::ScopedTryLock sl (progLock); if (sl.isLocked()) { prog = progression; loop = drumLoop; } }

        const bool hasNotes = prog != nullptr && (! prog->events.empty() || ! prog->melodyEvents.empty()
                                                  || ! prog->bassEvents.empty() || ! prog->drumEvents.empty());
        if (hasNotes && prog->totalBeats > 0.0)
        {
            const double beatsPerSample = hostBpm.load() / 60.0 / sr;
            const std::vector<NoteEvent>* lists[numLayers] { &prog->events, &prog->melodyEvents, &prog->bassEvents, &prog->drumEvents };
            const bool loopOn = loop != nullptr && layerEnabled[drumsLayer].load() && loop->audio.getNumSamples() > 1;
            const double loopInc = loopOn ? loop->sampleRate / sr : 0.0;
            const float* loopL = loopOn ? loop->audio.getReadPointer (0) : nullptr;
            const float* loopR = loopOn ? loop->audio.getReadPointer (loop->audio.getNumChannels() > 1 ? 1 : 0) : nullptr;
            const int loopLen = loopOn ? loop->audio.getNumSamples() : 0;

            auto emit = [&] (int l, const NoteEvent& e, int offset)
            {
                if (e.on)
                {
                    if (! layerEnabled[l].load()) return;
                    layerMidi[l].addEvent (juce::MidiMessage::noteOn (1, e.note, e.vel), offset);
                    heldNotes[l].push_back (e.note);
                }
                else
                {
                    layerMidi[l].addEvent (juce::MidiMessage::noteOff (1, e.note), offset);
                    heldNotes[l].erase (std::remove (heldNotes[l].begin(), heldNotes[l].end(), e.note), heldNotes[l].end());
                }
            };

            for (int i = 0; i < n; ++i)
            {
                if (playBeat >= prog->totalBeats)
                {
                    for (int l = 0; l < numLayers; ++l)
                    {
                        for (int note : heldNotes[l]) layerMidi[l].addEvent (juce::MidiMessage::noteOff (1, note), i);
                        heldNotes[l].clear(); nextEvt[l] = 0;
                    }
                    playBeat -= prog->totalBeats;
                    drumLoopPos = 0.0;
                }
                for (int l = 0; l < numLayers; ++l)
                    while (nextEvt[l] < (int) lists[l]->size() && playBeat >= (*lists[l])[(size_t) nextEvt[l]].beat)
                        emit (l, (*lists[l])[(size_t) nextEvt[l]++], i);
                if (loopOn)
                {
                    const int i0 = (int) drumLoopPos;
                    if (i0 + 1 < loopLen)
                    {
                        const float fr = (float) (drumLoopPos - i0);
                        loopBuffer.setSample (0, i, (loopL[i0] + (loopL[i0 + 1] - loopL[i0]) * fr) * 0.9f);
                        loopBuffer.setSample (1, i, (loopR[i0] + (loopR[i0 + 1] - loopR[i0]) * fr) * 0.9f);
                    }
                    drumLoopPos += loopInc;
                }
                playBeat += beatsPerSample;
            }

            int idx = -1;
            for (int c = 0; c < (int) prog->chords.size(); ++c)
                if (playBeat >= prog->chords[(size_t) c].startBeat) idx = c;
            currentChord.store (idx);
            playPosition.store (playBeat);
        }
    }

    // ---- Render the layers and sum --------------------------------------------------
    for (int l = 0; l < 3; ++l)
    {
        if (l == chordsLayer)
        {
            engines[l].process (buffer, layerMidi[l]);
        }
        else
        {
            layerBuffer.setSize (2, n, false, false, true);
            if (engines[l].process (layerBuffer, layerMidi[l]))
                for (int ch = 0; ch < 2; ++ch) buffer.addFrom (ch, 0, layerBuffer, ch, 0, n);
        }
    }
    // drums: built-in kit for MIDI hits + the synced loop
    layerBuffer.setSize (2, n, false, false, true);
    layerBuffer.clear();
    if (drumKit.process (layerBuffer, layerMidi[drumsLayer]))
        for (int ch = 0; ch < 2; ++ch) buffer.addFrom (ch, 0, layerBuffer, ch, 0, n);
    for (int ch = 0; ch < 2; ++ch) buffer.addFrom (ch, 0, loopBuffer, ch, 0, n);

    // ---- Scope + meter -------------------------------------------------------------
    auto* L = buffer.getReadPointer (0);
    auto* R = buffer.getReadPointer (1);
    int w = scopeWrite.load();
    for (int i = 0; i < n; ++i)
    {
        scope[(size_t) w].store (0.5f * (L[i] + R[i]));
        w = (w + 1) % scopeSize;
    }
    scopeWrite.store (w);
    levelL.store (buffer.getRMSLevel (0, 0, n));
    levelR.store (buffer.getRMSLevel (1, 0, n));
}

// ---------------------------------------------------------------------------
void IcewaterProcessor::setProgression (std::shared_ptr<const Progression> p)
{
    const bool wasPlaying = chordPlaying.load();
    if (wasPlaying) setChordPlaying (false);
    { const juce::ScopedLock sl (progLock); progression = std::move (p); }
    if (wasPlaying) setChordPlaying (true);
}

juce::File IcewaterProcessor::inboxDir (int which)
{
    static const char* names[] = { "Chords", "Melody", "Drums" };
    return juce::File::getSpecialLocation (juce::File::userDocumentsDirectory).getChildFile ("WXRST").getChildFile ("Inbox")
               .getChildFile (names[juce::jlimit (0, 2, which)]);
}

bool IcewaterProcessor::loadLayerMidi (Layer l, const juce::File& f)
{
    std::vector<NoteEvent> events; double len = 0.0;
    if (! ChordGen::readMidiFile (f, events, len)) return false;
    auto old = getProgression();
    auto np = std::make_shared<Progression> (old ? *old : Progression());
    if (! old) { np->totalBeats = 0.0; np->title = f.getFileNameWithoutExtension(); }
    (l == chordsLayer ? np->events : l == melodyLayer ? np->melodyEvents : l == bassLayer ? np->bassEvents : np->drumEvents) = events;
    const double bars = juce::jmax (4.0, std::ceil (len / 4.0) * 4.0);   // round up to whole bars
    const bool othersHaveNotes = (l != chordsLayer && ! np->events.empty()) || (l != melodyLayer && ! np->melodyEvents.empty())
                              || (l != bassLayer && ! np->bassEvents.empty()) || (l != drumsLayer && ! np->drumEvents.empty());
    np->totalBeats = othersHaveNotes ? juce::jmax (np->totalBeats, bars) : bars;
    layerFile[l] = f;
    setProgression (np);
    return true;
}

void IcewaterProcessor::clearLayer (Layer l)
{
    layerFile[l] = juce::File();
    auto old = getProgression();
    if (! old) return;
    auto np = std::make_shared<Progression> (*old);
    (l == chordsLayer ? np->events : l == melodyLayer ? np->melodyEvents : l == bassLayer ? np->bassEvents : np->drumEvents).clear();
    if (l == chordsLayer) np->chords.clear();
    setProgression (np);
}

bool IcewaterProcessor::loadDrumLoop (const juce::File& wav)
{
    auto data = SampleAnalysis::load (wav);
    if (! data) return false;
    const bool wasPlaying = chordPlaying.load();
    if (wasPlaying) setChordPlaying (false);
    { const juce::ScopedLock sl (progLock); drumLoop = data; }
    drumLoopFile = wav;
    if (wasPlaying) setChordPlaying (true);
    return true;
}

void IcewaterProcessor::clearDrumLoop()
{
    { const juce::ScopedLock sl (progLock); drumLoop = nullptr; }
    drumLoopFile = juce::File();
}

void IcewaterProcessor::setChordPlaying (bool shouldPlay)
{
    if (shouldPlay == chordPlaying.load()) return;
    if (! shouldPlay) chordStopRequested.store (true);
    chordPlaying.store (shouldPlay);
}

// Randomise the sound within "still sounds like a pad" ranges. Master/width untouched.
void IcewaterProcessor::randomiseSound()
{
    juce::Random r;
    auto set = [&] (const char* id, float value)
    {
        if (auto* p = apvts.getParameter (id)) p->setValueNotifyingHost (p->convertTo0to1 (value));
    };
    auto rf = [&] (float lo, float hi) { return lo + r.nextFloat() * (hi - lo); };
    auto ri = [&] (int lo, int hi) { return r.nextInt (juce::Range<int> (lo, hi + 1)); };

    set (ID::oscAOn, 1.f); set (ID::oscAWave, (float) ri (0, 5)); set (ID::oscAOct, (float) ri (-1, 0));
    set (ID::oscASemi, 0.f); set (ID::oscAFine, rf (-8.f, 8.f)); set (ID::oscAUnison, (float) ri (2, 7));
    set (ID::oscADetune, rf (8.f, 45.f)); set (ID::oscALevel, rf (0.5f, 0.85f));

    set (ID::oscBOn, r.nextFloat() < 0.85f ? 1.f : 0.f); set (ID::oscBWave, (float) ri (0, 5)); set (ID::oscBOct, (float) ri (-1, 1));
    const int bSemi[] = { 0, 0, 0, 7, 12, 5 };
    set (ID::oscBSemi, (float) bSemi[ri (0, 5)]); set (ID::oscBFine, rf (-8.f, 8.f)); set (ID::oscBUnison, (float) ri (1, 4));
    set (ID::oscBDetune, rf (5.f, 30.f)); set (ID::oscBLevel, rf (0.2f, 0.6f));

    set (ID::subLevel, rf (0.f, 0.6f)); set (ID::noiseLevel, rf (0.f, 0.15f)); set (ID::drift, rf (0.1f, 0.6f));
    set (ID::filtType, r.nextFloat() < 0.8f ? (float) ri (0, 1) : (float) ri (2, 3));
    set (ID::filtCutoff, std::exp (rf (std::log (400.f), std::log (6000.f))));
    set (ID::filtRes, rf (0.f, 0.45f)); set (ID::filtEnv, rf (0.f, 0.6f)); set (ID::filtKey, rf (0.2f, 0.6f)); set (ID::filtDrive, rf (0.f, 0.4f));

    set (ID::ampA, rf (0.2f, 2.5f)); set (ID::ampD, rf (1.f, 3.f)); set (ID::ampS, rf (0.6f, 1.f)); set (ID::ampR, rf (1.f, 4.f));
    set (ID::fA, rf (0.2f, 3.f)); set (ID::fD, rf (1.f, 4.f)); set (ID::fS, rf (0.2f, 0.7f)); set (ID::fR, rf (1.f, 4.f));
    set (ID::lfoShape, r.nextFloat() < 0.7f ? 0.f : (float) ri (1, 4)); set (ID::lfoRate, rf (0.08f, 0.8f));
    set (ID::lfoPitch, rf (0.f, 0.12f)); set (ID::lfoFilter, rf (-0.1f, 0.3f));

    set (ID::crushBits, (float) ri (8, 14)); set (ID::crushRate, std::exp (rf (std::log (11000.f), std::log (40000.f))));
    set (ID::lofiTone, std::exp (rf (std::log (3500.f), std::log (14000.f)))); set (ID::wow, rf (0.1f, 0.6f)); set (ID::wowRate, rf (0.2f, 1.2f));
    set (ID::heat, rf (0.f, 0.5f)); set (ID::crackle, rf (0.f, 0.4f)); set (ID::dist, 0.f);
    set (ID::voiceMode, 0.f); set (ID::pitchEnvAmt, r.nextFloat() < 0.3f ? rf (-3.f, 3.f) : 0.f); set (ID::pitchEnvDec, rf (0.05f, 0.5f));
    set (ID::chorusMix, rf (0.3f, 0.8f)); set (ID::chorusRate, rf (0.15f, 0.6f)); set (ID::chorusDepth, rf (0.3f, 0.8f));
    set (ID::delayMix, rf (0.f, 0.35f)); set (ID::delayTime, r.nextFloat() < 0.5f ? 0.375f : 0.5f); set (ID::delayFb, rf (0.3f, 0.6f));
    set (ID::verbMix, rf (0.3f, 0.65f)); set (ID::verbSize, rf (0.7f, 1.f)); set (ID::verbDamp, rf (0.3f, 0.7f));
}

void IcewaterProcessor::loadPreset (int index)
{
    const auto& presets = getPresets();
    if (index < 0 || index >= (int) presets.size()) return;
    currentProgram = index;
    currentUserPreset = -1;

    // reset everything except the sample section (so you can flip synth presets while playing with a sample)
    for (auto* p : getParameters())
        if (auto* rp = dynamic_cast<juce::RangedAudioParameter*> (p))
            if (! rp->paramID.startsWith ("smp"))
                rp->setValueNotifyingHost (rp->getDefaultValue());

    for (auto& [id, value] : presets[(size_t) index].values)
        if (auto* rp = apvts.getParameter (id))
            rp->setValueNotifyingHost (rp->convertTo0to1 (value));

    apvts.state.setProperty ("program", index, nullptr);
    apvts.state.setProperty ("userPreset", "", nullptr);
}

void IcewaterProcessor::loadUserPreset (int index)
{
    if (! juce::isPositiveAndBelow (index, (int) userPresets.size())) return;
    for (auto* p : getParameters())
        if (auto* rp = dynamic_cast<juce::RangedAudioParameter*> (p))
            rp->setValueNotifyingHost (rp->getDefaultValue());
    for (auto& [id, value] : userPresets[(size_t) index].values)
        if (auto* rp = apvts.getParameter (id))
            rp->setValueNotifyingHost (rp->convertTo0to1 (value));
    currentUserPreset = index;
    apvts.state.setProperty ("userPreset", userPresets[(size_t) index].name, nullptr);
}

int IcewaterProcessor::saveUserPreset (const juce::String& name)
{
    UserPresets::save (name, apvts);
    refreshUserPresets();
    for (int i = 0; i < (int) userPresets.size(); ++i)
        if (userPresets[(size_t) i].name == name) { currentUserPreset = i; apvts.state.setProperty ("userPreset", name, nullptr); return i; }
    return -1;
}

// ---------------------------------------------------------------------------
void IcewaterProcessor::setSample (std::shared_ptr<const SampleData> s)
{
    const juce::ScopedLock sl (sampleLock);
    sample = std::move (s);
}

bool IcewaterProcessor::loadSampleFile (const juce::File& f)
{
    auto s = SampleAnalysis::load (f);
    if (s == nullptr) return false;
    setSample (s);
    apvts.state.setProperty ("samplePath", f.getFullPathName(), nullptr);
    if (auto* p = apvts.getParameter (ID::smpLevel))
        if (p->getValue() <= 0.001f) p->setValueNotifyingHost (0.8f);   // dropping a sample turns it on
    if (auto* p = apvts.getParameter (ID::smpChop)) p->setValueNotifyingHost (0.f);
    return true;
}

juce::File IcewaterProcessor::extractNotesMidi()
{
    auto s = getSample();
    if (s == nullptr) return {};
    auto events = SampleAnalysis::extractNotes (*s, getHostBpm());
    if (events.empty()) return {};
    return ChordGen::writeMidiFile (events, s->name + " notes", getHostBpm());
}

juce::File IcewaterProcessor::chopSample (juce::File& chopDirOut)
{
    auto s = getSample();
    if (s == nullptr) return {};
    auto chopped = std::make_shared<SampleData> (*s);
    chopped->slices = SampleAnalysis::detectSlices (*chopped, 64, getHostBpm());
    chopDirOut = SampleAnalysis::writeChops (*chopped, juce::File::getSpecialLocation (juce::File::userDocumentsDirectory).getChildFile ("WXRST").getChildFile ("Chops"));
    setSample (chopped);
    if (auto* p = apvts.getParameter (ID::smpChop)) p->setValueNotifyingHost (1.f);
    return ChordGen::writeMidiFile (SampleAnalysis::chopEvents (*chopped, getHostBpm()), s->name + " chops", getHostBpm());
}

void IcewaterProcessor::setCurrentProgram (int index) { loadPreset (index); }

const juce::String IcewaterProcessor::getProgramName (int index)
{
    const auto& presets = getPresets();
    return juce::isPositiveAndBelow (index, (int) presets.size()) ? presets[(size_t) index].name : "";
}

void IcewaterProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    apvts.state.setProperty ("program", currentProgram.load(), nullptr);
    apvts.state.setProperty ("melodyPreset", melodyPreset.load(), nullptr);
    apvts.state.setProperty ("bassPreset", bassPreset.load(), nullptr);
    for (int l = 0; l < numLayers; ++l)
    {
        apvts.state.setProperty ("layer" + juce::String (l), layerEnabled[l].load(), nullptr);
        apvts.state.setProperty ("layerFile" + juce::String (l), layerFile[l].getFullPathName(), nullptr);
    }
    apvts.state.setProperty ("drumLoop", drumLoopFile.getFullPathName(), nullptr);
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void IcewaterProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
            currentProgram = (int) apvts.state.getProperty ("program", 0);
            setLayerPreset (melodyLayer, (int) apvts.state.getProperty ("melodyPreset", melodyPreset.load()));
            setLayerPreset (bassLayer, (int) apvts.state.getProperty ("bassPreset", bassPreset.load()));
            for (int l = 0; l < numLayers; ++l)
            {
                layerEnabled[l] = (bool) apvts.state.getProperty ("layer" + juce::String (l), layerEnabled[l].load());
                juce::File lf (apvts.state.getProperty ("layerFile" + juce::String (l), "").toString());
                if (lf.existsAsFile()) loadLayerMidi ((Layer) l, lf);
            }
            juce::File dl (apvts.state.getProperty ("drumLoop", "").toString());
            if (dl.existsAsFile()) loadDrumLoop (dl);
            const juce::String up = apvts.state.getProperty ("userPreset", "").toString();
            currentUserPreset = -1;
            for (int i = 0; i < (int) userPresets.size(); ++i) if (userPresets[(size_t) i].name == up) currentUserPreset = i;
            const juce::String sp = apvts.state.getProperty ("samplePath", "").toString();
            if (sp.isNotEmpty() && juce::File (sp).existsAsFile())
                if (auto smp = SampleAnalysis::load (juce::File (sp))) setSample (smp);
        }
}

juce::AudioProcessorEditor* IcewaterProcessor::createEditor() { return new IcewaterEditor (*this); }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new IcewaterProcessor(); }
