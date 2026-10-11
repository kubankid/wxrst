#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <unordered_map>
#include <string>
#include "Params.h"
#include "SynthVoice.h"
#include "Presets.h"

// One complete synth voice-stack + FX chain, driven by a flat id -> value map.
// The main layer's map is refreshed from the APVTS every block; the melody / 808 layers
// hold a snapshot of a preset.
class Engine
{
public:
    Engine()
    {
        for (int i = 0; i < 16; ++i) synth.addVoice (new IceVoice (vp));
        synth.addSound (new IceSound());
        synth.setNoteStealingEnabled (true);
    }

    // Called once by the processor with every parameter's default (id -> real-world value)
    static std::unordered_map<std::string, float>& defaults()
    {
        static std::unordered_map<std::string, float> d;
        return d;
    }

    void setToDefaults() { values = defaults(); }

    void loadPreset (const Preset& preset)
    {
        setToDefaults();
        for (auto& [id, v] : preset.values) values[id] = v;
    }

    void set (const std::string& id, float v) { values[id] = v; }
    float get (const char* id) const { auto it = values.find (id); return it == values.end() ? 0.f : it->second; }

    void prepare (double sampleRate, int samplesPerBlock)
    {
        sr = sampleRate;
        synth.setCurrentPlaybackSampleRate (sampleRate);
        juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock, 2 };
        toneFilter.prepare (spec); toneFilter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        wowLine.prepare (spec); wowLine.setMaximumDelayInSamples ((int) (sampleRate * 0.05));
        chorus.prepare (spec);
        delayLine.prepare (spec); delayLine.setMaximumDelayInSamples ((int) (sampleRate * 2.0));
        delayTone.prepare (spec); delayTone.setType (juce::dsp::StateVariableTPTFilterType::lowpass); delayTone.setCutoffFrequency (4000.f);
        delaySamplesSmoothed.reset (sampleRate, 0.1);
        reverb.prepare (spec);
        masterGain.prepare (spec); masterGain.setRampDurationSeconds (0.02);
        holdCounter = 0.f; tailSamples = 0;
    }

    void allNotesOff() { synth.allNotesOff (0, true); monoHeld.clear(); }

    // Renders into `buffer` (which is cleared first). Returns false if the layer is silent and nothing was rendered.
    bool process (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
    {
        const int n = buffer.getNumSamples();
        buffer.clear();
        updateVoiceParams();

        bool anyActive = false;
        for (int i = 0; i < synth.getNumVoices(); ++i) if (synth.getVoice (i)->isVoiceActive()) { anyActive = true; break; }
        if (anyActive || ! midi.isEmpty()) tailSamples = (int) (sr * 8.0);
        if (tailSamples <= 0) return false;
        tailSamples -= n;

        // ---- Mono mode: last-note priority with return-to-held-note -------------
        if (vp.mono)
        {
            monoBuffer.clear();
            for (const auto meta : midi)
            {
                const auto m = meta.getMessage();
                const int t = meta.samplePosition;
                if (m.isNoteOn())
                {
                    for (int held : monoHeld) monoBuffer.addEvent (juce::MidiMessage::noteOff (m.getChannel(), held), t);
                    monoHeld.erase (std::remove (monoHeld.begin(), monoHeld.end(), m.getNoteNumber()), monoHeld.end());
                    monoHeld.push_back (m.getNoteNumber());
                    monoBuffer.addEvent (m, t);
                }
                else if (m.isNoteOff())
                {
                    const bool wasTop = ! monoHeld.empty() && monoHeld.back() == m.getNoteNumber();
                    monoHeld.erase (std::remove (monoHeld.begin(), monoHeld.end(), m.getNoteNumber()), monoHeld.end());
                    monoBuffer.addEvent (m, t);
                    if (wasTop && ! monoHeld.empty())
                        monoBuffer.addEvent (juce::MidiMessage::noteOn (m.getChannel(), monoHeld.back(), (juce::uint8) 100), t);
                }
                else monoBuffer.addEvent (m, t);
            }
            midi.swapWith (monoBuffer);
        }
        else monoHeld.clear();

        synth.renderNextBlock (buffer, midi, 0, n);
        if (buffer.getNumChannels() < 2) return true;
        auto* L = buffer.getWritePointer (0);
        auto* R = buffer.getWritePointer (1);

        // ---- Lo-fi: sample-rate reduction + bit crush -------------------------
        {
            const float bits = get (ID::crushBits);
            const float steps = std::pow (2.f, bits - 1.f);
            const float ratio = (float) sr / juce::jmax (1000.f, get (ID::crushRate));
            for (int i = 0; i < n; ++i)
            {
                holdCounter += 1.f;
                if (holdCounter >= ratio)
                {
                    holdCounter -= ratio;
                    holdL = std::round (L[i] * steps) / steps;
                    holdR = std::round (R[i] * steps) / steps;
                }
                L[i] = holdL; R[i] = holdR;
            }
        }

        // ---- Tone ------------------------------------------------------------
        toneFilter.setCutoffFrequency (get (ID::lofiTone));
        toneFilter.setResonance (0.6f);
        { juce::dsp::AudioBlock<float> blk (buffer); juce::dsp::ProcessContextReplacing<float> ctx (blk); toneFilter.process (ctx); }

        // ---- Wow -------------------------------------------------------------
        {
            const float depth = get (ID::wow), rate = get (ID::wowRate);
            const float base = (float) sr * 0.012f, inc = rate / (float) sr;
            for (int i = 0; i < n; ++i)
            {
                wowPhase += inc; if (wowPhase >= 1.f) wowPhase -= 1.f;
                const float m = std::sin (wowPhase * juce::MathConstants<float>::twoPi)
                              + 0.35f * std::sin (wowPhase * juce::MathConstants<float>::twoPi * 3.7f);
                const float d = base * (1.f + 0.6f * depth * m);
                wowLine.pushSample (0, L[i]); wowLine.pushSample (1, R[i]);
                const float wl = wowLine.popSample (0, d), wr = wowLine.popSample (1, d);
                if (depth > 0.001f) { L[i] = wl; R[i] = wr; }
            }
        }

        // ---- Heat + Dist + Crackle -------------------------------------------
        {
            const float heat = get (ID::heat);
            const float drive = 1.f + heat * 5.f, comp = 1.f / std::pow (drive, 0.6f);
            const float dist = get (ID::dist);
            const float dDrive = 1.f + dist * 24.f, dComp = 1.f / std::pow (dDrive, 0.75f);
            const float crackleAmt = get (ID::crackle);
            const float density = crackleAmt * crackleAmt * 0.004f;
            for (int i = 0; i < n; ++i)
            {
                if (heat > 0.001f) { L[i] = std::tanh (L[i] * drive) * comp; R[i] = std::tanh (R[i] * drive) * comp; }
                if (dist > 0.001f)
                {
                    auto clip = [] (float x) { x = juce::jlimit (-1.2f, 1.2f, x); return x - 0.25f * x * x * x + 0.08f * x * x; };
                    L[i] = clip (L[i] * dDrive) * dComp; R[i] = clip (R[i] * dDrive) * dComp;
                }
                if (crackleAmt > 0.001f)
                {
                    crackleSeed = crackleSeed * 1664525u + 1013904223u;
                    const float r1 = (crackleSeed >> 8) * (1.f / 16777216.f);
                    float imp = 0.f;
                    if (r1 < density)
                    {
                        crackleSeed = crackleSeed * 1664525u + 1013904223u;
                        imp = ((crackleSeed >> 8) * (1.f / 16777216.f) * 2.f - 1.f) * crackleAmt * 0.5f;
                    }
                    crackleLP += 0.25f * (imp - crackleLP);
                    L[i] += crackleLP; R[i] += crackleLP;
                }
            }
        }

        // ---- Chorus ------------------------------------------------------------
        chorus.setRate (get (ID::chorusRate)); chorus.setDepth (get (ID::chorusDepth));
        chorus.setCentreDelay (12.f); chorus.setFeedback (0.1f); chorus.setMix (get (ID::chorusMix));
        { juce::dsp::AudioBlock<float> blk (buffer); juce::dsp::ProcessContextReplacing<float> ctx (blk); chorus.process (ctx); }

        // ---- Delay ---------------------------------------------------------------
        {
            const float mix = get (ID::delayMix), fb = get (ID::delayFb);
            delaySamplesSmoothed.setTargetValue (get (ID::delayTime) * (float) sr);
            for (int i = 0; i < n; ++i)
            {
                const float d = juce::jlimit (1.f, (float) sr * 1.95f, delaySamplesSmoothed.getNextValue());
                float dl = delayLine.popSample (0, d), dr = delayLine.popSample (1, d);
                dl = delayTone.processSample (0, dl); dr = delayTone.processSample (1, dr);
                delayLine.pushSample (0, L[i] + dr * fb);
                delayLine.pushSample (1, R[i] + dl * fb);
                L[i] += dl * mix; R[i] += dr * mix;
            }
        }

        // ---- Reverb ----------------------------------------------------------------
        {
            juce::dsp::Reverb::Parameters rp;
            const float mix = get (ID::verbMix);
            rp.roomSize = get (ID::verbSize); rp.damping = get (ID::verbDamp);
            rp.wetLevel = mix * 0.9f; rp.dryLevel = 1.f - mix * 0.5f; rp.width = 1.f; rp.freezeMode = 0.f;
            reverb.setParameters (rp);
            juce::dsp::AudioBlock<float> blk (buffer); juce::dsp::ProcessContextReplacing<float> ctx (blk); reverb.process (ctx);
        }

        // ---- Width + master ---------------------------------------------------------
        {
            const float w = get (ID::width);
            for (int i = 0; i < n; ++i)
            {
                const float mid = 0.5f * (L[i] + R[i]), side = 0.5f * (L[i] - R[i]) * w * 1.4f;
                L[i] = mid + side; R[i] = mid - side;
            }
            masterGain.setGainDecibels (get (ID::master));
            juce::dsp::AudioBlock<float> blk (buffer); juce::dsp::ProcessContextReplacing<float> ctx (blk); masterGain.process (ctx);
        }
        return true;
    }

    std::unordered_map<std::string, float> values;

private:
    void updateVoiceParams()
    {
        auto osc = [&] (OscParams& o, const char* on, const char* wave, const char* oct, const char* semi,
                        const char* fine, const char* uni, const char* det, const char* lvl)
        {
            o.on = get (on) > 0.5f; o.wave = (int) get (wave); o.oct = (int) get (oct); o.semi = (int) get (semi);
            o.fine = get (fine); o.unison = (int) get (uni); o.detune = get (det); o.level = get (lvl);
        };
        osc (vp.a, ID::oscAOn, ID::oscAWave, ID::oscAOct, ID::oscASemi, ID::oscAFine, ID::oscAUnison, ID::oscADetune, ID::oscALevel);
        osc (vp.b, ID::oscBOn, ID::oscBWave, ID::oscBOct, ID::oscBSemi, ID::oscBFine, ID::oscBUnison, ID::oscBDetune, ID::oscBLevel);
        vp.noise = get (ID::noiseLevel); vp.sub = get (ID::subLevel); vp.drift = get (ID::drift); vp.glide = get (ID::glide);
        vp.filtType = (int) get (ID::filtType);
        vp.mono = get (ID::voiceMode) > 0.5f;
        vp.pitchEnvAmt = get (ID::pitchEnvAmt); vp.pitchEnvDec = get (ID::pitchEnvDec);
        vp.cutoff = get (ID::filtCutoff); vp.res = get (ID::filtRes); vp.filtEnv = get (ID::filtEnv);
        vp.keyTrack = get (ID::filtKey); vp.drive = get (ID::filtDrive);
        vp.amp = { get (ID::ampA), get (ID::ampD), get (ID::ampS), get (ID::ampR) };
        vp.filt = { get (ID::fA), get (ID::fD), get (ID::fS), get (ID::fR) };
        vp.lfoShape = (int) get (ID::lfoShape); vp.lfoRate = get (ID::lfoRate);
        vp.lfoPitch = get (ID::lfoPitch); vp.lfoFilter = get (ID::lfoFilter);
    }

    juce::Synthesiser synth;
    VoiceParams vp;
    double sr = 44100.0;
    int tailSamples = 0;
    std::vector<int> monoHeld;
    juce::MidiBuffer monoBuffer;

    float holdL = 0.f, holdR = 0.f, holdCounter = 0.f;
    juce::dsp::StateVariableTPTFilter<float> toneFilter;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> wowLine { 4800 };
    float wowPhase = 0.f;
    juce::dsp::Chorus<float> chorus;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine { 96000 * 2 };
    juce::dsp::StateVariableTPTFilter<float> delayTone;
    juce::SmoothedValue<float> delaySamplesSmoothed;
    juce::dsp::Reverb reverb;
    juce::dsp::Gain<float> masterGain;
    float crackleLP = 0.f;
    uint32_t crackleSeed = 0x9e3779b9u;
};
