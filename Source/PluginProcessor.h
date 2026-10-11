#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Params.h"
#include "Presets.h"
#include "Chords.h"
#include "Engine.h"

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

    // ---- Layers (melody / 808 have their own sound) --------------------------
    enum Layer { chordsLayer = 0, melodyLayer = 1, bassLayer = 2 };
    void setLayerPreset (Layer l, int presetIndex);   // melody / bass only
    int getLayerPreset (Layer l) const { return l == melodyLayer ? melodyPreset.load() : bassPreset.load(); }
    std::atomic<bool> layerEnabled[3] { true, true, true };

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
    juce::MidiBuffer layerMidi[3];
    std::atomic<int> melodyPreset { -1 }, bassPreset { -1 };
    std::atomic<int> pendingLayerLoad[3] { -1, -1, -1 };   // preset index to load on the audio thread
    std::atomic<int> currentProgram { 0 };

    std::map<juce::String, std::atomic<float>*> raw;

    juce::CriticalSection progLock;
    std::shared_ptr<const Progression> progression;
    std::atomic<bool> chordPlaying { false }, chordStopRequested { false };
    std::atomic<double> hostBpm { 120.0 };
    double sr = 44100.0;
    double playBeat = 0.0;
    int nextEvt[3] { 0, 0, 0 };
    std::vector<int> heldNotes[3];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IcewaterProcessor)
};
