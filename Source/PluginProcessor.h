#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Params.h"
#include "Presets.h"
#include "Chords.h"
#include "Engine.h"
#include "Sampler.h"
#include "UserPresets.h"
#include "DrumKit.h"

class IcewaterProcessor : public juce::AudioProcessor
{
public:
    IcewaterProcessor();
    ~IcewaterProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 6.0; }

    int getNumPrograms() override { return (int) getPresets().size(); }
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadPreset (int index);
    void randomiseSound();

    // ---- User presets (~/Documents/WXRST/Presets) -----------------------------
    std::vector<UserPreset> userPresets;            // message thread only
    std::atomic<int> currentUserPreset { -1 };       // -1 = a factory preset is current
    void refreshUserPresets() { userPresets = UserPresets::loadAll(); }
    void loadUserPreset (int index);
    int  saveUserPreset (const juce::String& name);  // returns its index in userPresets

    // ---- Sample section -------------------------------------------------------
    bool loadSampleFile (const juce::File& f);       // message thread
    std::shared_ptr<const SampleData> getSample() const { const juce::ScopedLock sl (sampleLock); return sample; }
    juce::File extractNotesMidi();                   // pitch-track -> .mid in the chords folder
    juce::File chopSample (juce::File& chopDirOut);  // slice -> WAVs + .mid; enables CHOP mode

    // ---- Layers (melody / 808 have their own sound) --------------------------
    enum Layer { chordsLayer = 0, melodyLayer = 1, bassLayer = 2, drumsLayer = 3 };
    static constexpr int numLayers = 4;
    void setLayerPreset (Layer l, int presetIndex);   // melody / bass only
    int getLayerPreset (Layer l) const { return l == melodyLayer ? melodyPreset.load() : bassPreset.load(); }
    std::atomic<bool> layerEnabled[numLayers] { true, true, true, true };

    // ---- Loading MIDI / loops into a layer (from XTRACT's inbox or any file) ----
    static juce::File inboxDir (int which);          // 0 Chords, 1 Melody, 2 Drums  (~/Documents/WXRST/Inbox/...)
    bool loadLayerMidi (Layer l, const juce::File& midiFile);   // replaces that layer's notes in the progression
    void clearLayer (Layer l);
    void clearLayerFileOnly (Layer l) { layerFile[l] = juce::File(); }
    juce::File getLayerFile (Layer l) const { return layerFile[l]; }
    bool loadDrumLoop (const juce::File& wav);        // audio loop that plays in sync with the progression
    void clearDrumLoop();
    juce::String getDrumLoopName() const { const juce::ScopedLock sl (progLock); return drumLoop ? drumLoop->name : juce::String(); }

    // ---- Chord generator -------------------------------------------------
    void setProgression (std::shared_ptr<const Progression> p);
    std::shared_ptr<const Progression> getProgression() const { const juce::ScopedLock sl (progLock); return progression; }
    void setChordPlaying (bool shouldPlay);
    bool isChordPlaying() const { return chordPlaying.load(); }
    std::atomic<int> currentChord { -1 };
    std::atomic<double> playPosition { 0.0 };
    double getHostBpm() const { return hostBpm.load(); }

    juce::AudioProcessorValueTreeState apvts;

    static constexpr int scopeSize = 512;
    std::array<std::atomic<float>, scopeSize> scope {};
    std::atomic<int> scopeWrite { 0 };
    std::atomic<float> levelL { 0.f }, levelR { 0.f };

private:
    Engine engines[3];
    juce::AudioBuffer<float> layerBuffer;
    juce::MidiBuffer layerMidi[numLayers];
    DrumKit drumKit;
    juce::AudioBuffer<float> loopBuffer;
    std::shared_ptr<const SampleData> drumLoop;      // guarded by progLock
    double drumLoopPos = 0.0;
    juce::File layerFile[numLayers], drumLoopFile;    // message thread
    std::atomic<int> melodyPreset { -1 }, bassPreset { -1 };
    std::atomic<int> pendingLayerLoad[3] { -1, -1, -1 };   // preset index to load on the audio thread
    std::atomic<int> currentProgram { 0 };

    std::map<juce::String, std::atomic<float>*> raw;

    juce::CriticalSection sampleLock;
    std::shared_ptr<const SampleData> sample;
    void setSample (std::shared_ptr<const SampleData> s);

    juce::CriticalSection progLock;
    std::shared_ptr<const Progression> progression;
    std::atomic<bool> chordPlaying { false }, chordStopRequested { false };
    std::atomic<double> hostBpm { 120.0 };
    double sr = 44100.0;
    double playBeat = 0.0;
    int nextEvt[numLayers] { 0, 0, 0, 0 };
    std::vector<int> heldNotes[numLayers];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IcewaterProcessor)
};
