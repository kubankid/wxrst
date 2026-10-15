#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"

// Animated aurora / ice-dust background. Glow is rendered at 1/8 scale and stretched (cheap + soft).
class AnimatedBackground : public juce::Component, private juce::Timer
{
public:
    AnimatedBackground();
    void paint (juce::Graphics& g) override;
    void resized() override;
    void setAnimating (bool on) { animating = on; if (on) startTimerHz (24); else stopTimer(); repaint(); }
    bool isAnimating() const { return animating; }
    void themeChanged() { renderGlow(); repaint(); }
    // Only the areas not covered by panels get repainted each frame
    void setExclusions (const std::vector<juce::Rectangle<int>>& panels)
    {
        dirty.clear(); dirty.add (getLocalBounds());
        for (auto& r : panels) dirty.subtract (r.reduced (2));
    }
private:
    void timerCallback() override;
    void renderGlow();
    struct Blob { float x, y, r, dx, dy, phase; juce::Colour c; };
    struct Particle { float x, y, vy, vx, size, phase, speed; };
    std::vector<Blob> blobs;
    std::vector<Particle> particles;
    juce::Image glow;
    juce::RectangleList<int> dirty;
    float t = 0.f;
    int frame = 0;
    bool animating = true;
};

// A real dice: rounded cube with pips. Rolls a new face every click.
class DiceButton : public juce::Button
{
public:
    DiceButton() : juce::Button ("DICE") {}
    void paintButton (juce::Graphics& g, bool over, bool down) override;
    void clicked() override { face = 1 + juce::Random::getSystemRandom().nextInt (6); repaint(); }
private:
    int face = 5;
};

// Header tab (ICE / MIDNIGHT theme switch)
class TabButton : public juce::Button
{
public:
    TabButton (const juce::String& name) : juce::Button (name) { setClickingTogglesState (true); }
    void paintButton (juce::Graphics& g, bool over, bool) override
    {
        if (auto* l = dynamic_cast<IceLookAndFeel*> (&getLookAndFeel())) l->drawTabButton (g, *this, over);
    }
};

// Rotary knob with label underneath and value readout on hover
struct Knob : public juce::Component
{
    Knob (juce::AudioProcessorValueTreeState& s, const juce::String& id, const juce::String& text);
    void resized() override;
    juce::Slider slider;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> att;
};

struct Panel : public juce::Component
{
    Panel (const juce::String& title) : title (title) {}
    void paint (juce::Graphics& g) override;
    juce::String title;
};

class Scope : public juce::Component, private juce::Timer
{
public:
    Scope (IcewaterProcessor& p) : proc (p) { startTimerHz (30); }
    void paint (juce::Graphics& g) override;
private:
    void timerCallback() override { repaint(); }
    IcewaterProcessor& proc;
};

// ADSR curve display
class EnvDisplay : public juce::Component, private juce::Timer
{
public:
    EnvDisplay (juce::AudioProcessorValueTreeState& s, const char* a, const char* d, const char* su, const char* r)
        : A (s.getRawParameterValue (a)), D (s.getRawParameterValue (d)), S (s.getRawParameterValue (su)), R (s.getRawParameterValue (r))
    { startTimerHz (15); }
    void paint (juce::Graphics& g) override;
private:
    void timerCallback() override
    {
        const float v = A->load() * 1.f + D->load() * 7.f + S->load() * 31.f + R->load() * 131.f;
        if (v != lastSig) { lastSig = v; repaint(); }
    }
    float lastSig = -1.f;
    std::atomic<float> *A, *D, *S, *R;
};

// Drag this onto FL's piano roll / playlist to drop the generated MIDI in
class MidiDragPad : public juce::Component, private juce::Timer
{
public:
    MidiDragPad (IcewaterProcessor& p, int kind) : proc (p), kind (kind) { startTimerHz (20); }
    void setFile (const juce::File& f) { file = f; repaint(); }
    void paint (juce::Graphics& g) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseDown (const juce::MouseEvent&) override { dragging = false; }
private:
    void timerCallback() override
    {
        const bool playing = proc.isChordPlaying();
        const bool en = proc.layerEnabled[kind].load();
        if (playing || wasPlaying || en != lastEnabled) repaint();
        wasPlaying = playing; lastEnabled = en;
    }
    bool wasPlaying = false, lastEnabled = true;
    IcewaterProcessor& proc;
    int kind; // 0 chords, 1 melody, 2 bass
    juce::File file;
    bool dragging = false;
};

// Waveform + drop zone for the SAMPLE section
class WaveformView : public juce::Component, public juce::FileDragAndDropTarget, private juce::Timer
{
public:
    WaveformView (IcewaterProcessor& p) : proc (p) { startTimerHz (4); }
    void paint (juce::Graphics& g) override;
    bool isInterestedInFileDrag (const juce::StringArray& files) override
    {
        for (auto& f : files) if (juce::File (f).hasFileExtension ("wav;aif;aiff;mp3;flac;ogg;m4a")) return true;
        return false;
    }
    void fileDragEnter (const juce::StringArray&, int, int) override { hover = true; repaint(); }
    void fileDragExit (const juce::StringArray&) override { hover = false; repaint(); }
    void filesDropped (const juce::StringArray& files, int, int) override
    {
        hover = false;
        for (auto& f : files) if (proc.loadSampleFile (juce::File (f))) break;
        repaint();
    }
private:
    void timerCallback() override { auto s = proc.getSample(); if (s.get() != lastShown) { lastShown = s.get(); repaint(); } }
    IcewaterProcessor& proc;
    const SampleData* lastShown = nullptr;
    bool hover = false;
};

// Drag a generated file (MIDI clip) out into FL
class FileDragPad : public juce::Component
{
public:
    FileDragPad (const juce::String& label) : label (label) {}
    void setFile (const juce::File& f) { file = f; repaint(); }
    void paint (juce::Graphics& g) override;
    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (dragging || ! file.existsAsFile() || e.getDistanceFromDragStart() < 8) return;
        dragging = true;
        if (auto* c = juce::DragAndDropContainer::findParentDragContainerFor (this))
            c->performExternalDragDropOfFiles ({ file.getFullPathName() }, false, this, [this] { dragging = false; });
    }
    void mouseDown (const juce::MouseEvent&) override { dragging = false; }
private:
    juce::String label;
    juce::File file;
    bool dragging = false;
};

class IcewaterEditor : public juce::AudioProcessorEditor, public juce::DragAndDropContainer, private juce::Timer
{
public:
    explicit IcewaterEditor (IcewaterProcessor&);
    ~IcewaterEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void layoutKnobs (juce::Rectangle<int> area, std::initializer_list<Knob*> knobs);
    Knob& addKnob (Panel& panel, const char* id, const juce::String& text);
    void setupCombo (juce::ComboBox& box, const juce::StringArray& items, const char* id,
                     std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>& att, Panel& parent);

    IcewaterProcessor& proc;
    IceLookAndFeel lnf;
    AnimatedBackground background;
    juce::TextButton animBtn { "~" };
    juce::OwnedArray<Knob> knobs;
    std::map<juce::String, Knob*> knobById;

    Panel oscA { "OSC A" }, oscB { "OSC B" }, filter { "FILTER" }, ampEnv { "AMP ENV" }, filtEnv { "FILTER ENV" },
          lfo { "LFO" }, pitchP { "PITCH / VOICE" }, global { "GLOBAL" }, lofi { "LO-FI" }, chorusP { "CHORUS" }, delayP { "DELAY" }, verbP { "REVERB" };

    juce::ComboBox oscAWave, oscBWave, lfoShape, filtType, voiceMode, presetBox, categoryBox;
    juce::TextButton saveBtn { "SAVE" };
    void saveUserPresetDialog();
    void loadPresetById (int itemId);   // factory: index + 1, user: kUserIdBase + index
    static constexpr int kUserIdBase = 10000;
    int lastUserPreset = -1;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> voiceModeAtt;
    void rebuildPresetList();
    std::vector<int> visiblePresets; // combo item ids for the current category
    juce::ToggleButton oscAOn { "ON" }, oscBOn { "ON" };
    juce::TextButton prevBtn { "<" }, nextBtn { ">" };
    DiceButton diceBtn;
    TabButton midnightTab { "MIDNIGHT" };
    void applyTheme (int which);
    void syncThemeToPreset (int program); // Synthwave / 16-Bit presets -> MIDNIGHT, everything else -> ICE
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oscAWaveAtt, oscBWaveAtt, lfoShapeAtt, filtTypeAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> oscAOnAtt, oscBOnAtt;
    Scope scope { proc };
    EnvDisplay ampEnvDisp { proc.apvts, ID::ampA, ID::ampD, ID::ampS, ID::ampR };
    EnvDisplay filtEnvDisp { proc.apvts, ID::fA, ID::fD, ID::fS, ID::fR };
    int lastProgram = -1;

    // Sample section
    Panel sampleP { "SAMPLE  -  drop a WAV / MP3 here, play it on the keys, run it through the lo-fi chain" };
    WaveformView waveform { proc };
    juce::TextButton loadSampleBtn { "LOAD" }, toMidiBtn { "TO MIDI" }, chopBtn { "CHOP IT" };
    juce::ToggleButton smpLoopT { "LOOP" }, smpRevT { "REV" }, smpChopT { "CHOP" }, smpHalfT { "HALF" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> smpLoopAtt, smpRevAtt, smpChopAtt, smpHalfAtt;
    FileDragPad notesPad { "DRAG NOTES >>" }, chopsPad { "DRAG CHOPS >>" };
    std::unique_ptr<juce::FileChooser> chooser;

    // Chord generator
    void generateChords();
    Panel chordsP { "CHORD ENGINE" };
    juce::ComboBox keyBox, scaleBox, styleBox, barsBox, rhythmBox;
    juce::TextButton generateBtn { "GENERATE" }, playBtn { "PLAY" }, strumBtn { "STRUM" };
    juce::Label keyLbl { {}, "KEY" }, scaleLbl { {}, "SCALE" }, styleLbl { {}, "STYLE" }, barsLbl { {}, "LEN" }, rhythmLbl { {}, "FEEL" };
    MidiDragPad chordPad { proc, 0 }, melodyPad { proc, 1 }, bassPad { proc, 2 }, drumPad { proc, 3 };
    juce::TextButton layerBtn[4] { juce::TextButton ("CHORDS"), juce::TextButton ("MELODY"), juce::TextButton ("808"), juce::TextButton ("DRUMS") };
    juce::TextButton layerLoad[4] { juce::TextButton ("LOAD"), juce::TextButton ("LOAD"), juce::TextButton ("LOAD"), juce::TextButton ("LOAD") };
    juce::Label drumSoundLbl { {}, "built-in kit" };
    void showLoadMenu (int layer);      // MIDI (and loops, for DRUMS) from Documents/WXRST/Inbox or any file
    void refreshLayerPads();
    std::vector<juce::File> menuFiles;
    int snapshotTicks = 0;
    juce::ComboBox layerSound[2];       // melody / 808 sound pickers
    juce::Label chordSoundLbl { {}, "main sound (knobs above)" };
    unsigned chordSeed = (unsigned) juce::Time::currentTimeMillis();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IcewaterEditor)
};
