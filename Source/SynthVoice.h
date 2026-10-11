#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <random>

struct OscParams
{
    bool on = true;
    int wave = 0, oct = 0, semi = 0, unison = 1;
    float fine = 0.f, detune = 0.f, level = 0.f;
};

struct VoiceParams
{
    OscParams a, b;
    float noise = 0.f, sub = 0.f, drift = 0.f, glide = 0.f;
    bool mono = false;
    float pitchEnvAmt = 0.f, pitchEnvDec = 0.08f;
    mutable std::atomic<float> lastFreq { 0.f };   // last pitch of any voice (for mono glide)
    mutable std::atomic<int> activeCount { 0 };
    int filtType = 0;
    float cutoff = 2000.f, res = 0.2f, filtEnv = 0.f, keyTrack = 0.f, drive = 0.f;
    juce::ADSR::Parameters amp, filt;
    int lfoShape = 0;
    float lfoRate = 0.5f, lfoPitch = 0.f, lfoFilter = 0.f;
};

struct IceSound : public juce::SynthesiserSound
{
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

// Simple TPT state-variable lowpass (Zavalishin), one per channel.
struct SVF
{
    float ic1 = 0.f, ic2 = 0.f, g = 0.f, k = 1.f, a1 = 0.f, a2 = 0.f, a3 = 0.f;
    void set (float cutoff, float res, double sr)
    {
        cutoff = juce::jlimit (20.f, (float) (sr * 0.45), cutoff);
        g = std::tan (juce::MathConstants<float>::pi * cutoff / (float) sr);
        k = 2.f - 1.9f * juce::jlimit (0.f, 1.f, res);
        a1 = 1.f / (1.f + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
    }
    // type: 0 = LP, 1 = BP, 2 = HP
    float process (float v0, int type = 0)
    {
        float v3 = v0 - ic2;
        float v1 = a1 * ic1 + a2 * v3;
        float v2 = ic2 + a2 * ic1 + a3 * v3;
        ic1 = 2.f * v1 - ic1;
        ic2 = 2.f * v2 - ic2;
        if (type == 1) return v1;
        if (type == 2) return v0 - k * v1 - v2;
        return v2;
    }
    void reset() { ic1 = ic2 = 0.f; }
};

class IceVoice : public juce::SynthesiserVoice
{
public:
    IceVoice (const VoiceParams& p) : params (p)
    {
        std::mt19937 rng (std::random_device{}());
        std::uniform_real_distribution<float> d (0.f, 1.f);
        for (auto& ph : phaseA) ph = d (rng);
        for (auto& ph : phaseB) ph = d (rng);
        lfoPhase = d (rng);
        driftPhaseA = d (rng); driftPhaseB = d (rng);
        driftRateA = 0.08f + 0.2f * d (rng); driftRateB = 0.08f + 0.2f * d (rng);
    }

    bool canPlaySound (juce::SynthesiserSound* s) override { return dynamic_cast<IceSound*> (s) != nullptr; }

    void startNote (int midiNote, float velocity, juce::SynthesiserSound*, int) override
    {
        const double sr = getSampleRate();
        targetFreq = (float) juce::MidiMessage::getMidiNoteInHertz (midiNote);
        noteNumber = midiNote;
        if (params.mono && params.glide > 0.001f && params.activeCount.load() > 0 && params.lastFreq.load() > 0.f)
            currentFreq = params.lastFreq.load();
        else if (! wasActive || params.glide <= 0.001f)
            currentFreq = targetFreq;
        vel = 0.4f + 0.6f * velocity;
        pitchEnvLevel = 1.f;
        subPhase = 0.f;
        if (! wasActive) params.activeCount.fetch_add (1);

        ampEnv.setSampleRate (sr); filtEnv.setSampleRate (sr);
        ampEnv.setParameters (params.amp); filtEnv.setParameters (params.filt);
        ampEnv.noteOn(); filtEnv.noteOn();
        for (auto& f : filter) f.reset();
        wasActive = true;
    }

    void stopNote (float, bool allowTailOff) override
    {
        if (allowTailOff) { ampEnv.noteOff(); filtEnv.noteOff(); }
        else { clearCurrentNote(); ampEnv.reset(); filtEnv.reset(); if (wasActive) params.activeCount.fetch_sub (1); wasActive = false; }
    }

    void pitchWheelMoved (int v) override { bend = (v - 8192) / 8192.f * 2.f; }
    void controllerMoved (int cc, int v) override { if (cc == 1) modWheel = v / 127.f; }

    void renderNextBlock (juce::AudioBuffer<float>& out, int start, int numSamples) override
    {
        if (! isVoiceActive()) return;

        const double sr = getSampleRate();
        ampEnv.setParameters (params.amp);
        filtEnv.setParameters (params.filt);

        const float glideCoef = params.glide > 0.001f ? std::exp (-1.f / (params.glide * (float) sr * 0.25f)) : 0.f;
        const float lfoInc = params.lfoRate / (float) sr;
        const float driftIncA = driftRateA / (float) sr, driftIncB = driftRateB / (float) sr;
        const float driftCents = params.drift * 7.f;
        const int ftype = params.filtType; // 0 LP12, 1 LP24, 2 BP, 3 HP
        const int svfMode = ftype == 2 ? 1 : (ftype == 3 ? 2 : 0);
        const float pEnvCoef = std::exp (-1.f / (juce::jmax (0.001f, params.pitchEnvDec) * (float) sr));
        const float pEnvAmt = params.pitchEnvAmt;

        auto* L = out.getWritePointer (0, start);
        auto* R = out.getNumChannels() > 1 ? out.getWritePointer (1, start) : nullptr;

        for (int n = 0; n < numSamples; ++n)
        {
            // glide
            currentFreq = glideCoef > 0.f ? targetFreq + (currentFreq - targetFreq) * glideCoef : targetFreq;

            // LFO
            lfoPhase += lfoInc; if (lfoPhase >= 1.f) { lfoPhase -= 1.f; lfoRand = rnd(); }
            const float lfo = lfoValue();

            const float a = ampEnv.getNextSample();
            const float fe = filtEnv.getNextSample();
            if (! ampEnv.isActive()) { clearCurrentNote(); if (wasActive) params.activeCount.fetch_sub (1); wasActive = false; break; }

            pitchEnvLevel *= pEnvCoef;
            const float pitchMod = std::pow (2.f, (bend + lfo * params.lfoPitch + pEnvAmt * pitchEnvLevel) / 12.f);
            float l = 0.f, r = 0.f;

            // analog drift: slow, independent wander per oscillator
            driftPhaseA += driftIncA; if (driftPhaseA >= 1.f) driftPhaseA -= 1.f;
            driftPhaseB += driftIncB; if (driftPhaseB >= 1.f) driftPhaseB -= 1.f;
            const float dA = std::pow (2.f, driftCents * std::sin (driftPhaseA * juce::MathConstants<float>::twoPi) / 1200.f);
            const float dB = std::pow (2.f, driftCents * std::sin (driftPhaseB * juce::MathConstants<float>::twoPi * 1.3f) / 1200.f);

            if (params.a.on && params.a.level > 0.f) renderOsc (params.a, phaseA, currentFreq * pitchMod * dA, (float) sr, l, r);
            if (params.b.on && params.b.level > 0.f) renderOsc (params.b, phaseB, currentFreq * pitchMod * dB, (float) sr, l, r);
            if (params.sub > 0.f)
            {
                subPhase += currentFreq * pitchMod * 0.5f / (float) sr; if (subPhase >= 1.f) subPhase -= 1.f;
                const float s = std::sin (subPhase * juce::MathConstants<float>::twoPi) * params.sub * 0.8f;
                l += s; r += s;
            }
            if (params.noise > 0.f)
            {
                const float nz = (rnd() * 2.f - 1.f) * params.noise * 0.3f;
                l += nz; r += nz;
            }

            // filter: cutoff * env * keytrack * lfo * modwheel * velocity (update every 8 samples for cost)
            if ((n & 7) == 0)
            {
                float semis = params.filtEnv * 60.f * fe + params.lfoFilter * 24.f * lfo
                            + params.keyTrack * (float) (noteNumber - 60)
                            + modWheel * 36.f + (vel - 0.7f) * 10.f;
                float cut = params.cutoff * std::pow (2.f, semis / 12.f);
                for (auto& f : filter) f.set (cut, ftype == 1 ? params.res * 0.7f : params.res, sr);
            }
            const float dr = 1.f + params.drive * 6.f;
            l = std::tanh (l * dr) / std::sqrt (dr);
            r = std::tanh (r * dr) / std::sqrt (dr);
            l = filter[0].process (l, svfMode);
            r = filter[1].process (r, svfMode);
            if (ftype == 1) { l = filter[2].process (l, 0); r = filter[3].process (r, 0); }

            const float g = a * vel * 0.35f;
            L[n] += l * g;
            if (R) R[n] += r * g;
        }
        if (wasActive) params.lastFreq.store (currentFreq * std::pow (2.f, pEnvAmt * pitchEnvLevel / 12.f));
    }

private:
    const VoiceParams& params;
    juce::ADSR ampEnv, filtEnv;
    SVF filter[4];
    float phaseA[7] {}, phaseB[7] {}, subPhase = 0.f;
    float lfoPhase = 0.f, lfoRand = 0.f;
    float driftPhaseA = 0.f, driftPhaseB = 0.f, driftRateA = 0.1f, driftRateB = 0.15f;
    float currentFreq = 440.f, targetFreq = 440.f, vel = 1.f, bend = 0.f, modWheel = 0.f, pitchEnvLevel = 0.f;
    int noteNumber = 60;
    bool wasActive = false;
    uint32_t seed = 0x1234567u;

    float rnd() { seed = seed * 1664525u + 1013904223u; return (seed >> 8) * (1.f / 16777216.f); }

    float lfoValue() const
    {
        switch (params.lfoShape)
        {
            case 0: return std::sin (lfoPhase * juce::MathConstants<float>::twoPi);
            case 1: return 1.f - 4.f * std::abs (lfoPhase - 0.5f);
            case 2: return 2.f * lfoPhase - 1.f;
            case 3: return lfoPhase < 0.5f ? 1.f : -1.f;
            default: return lfoRand * 2.f - 1.f;
        }
    }

    static float polyBlep (float t, float dt)
    {
        if (t < dt) { t /= dt; return t + t - t * t - 1.f; }
        if (t > 1.f - dt) { t = (t - 1.f) / dt; return t * t + t + t + 1.f; }
        return 0.f;
    }

    static float waveform (int wave, float ph, float dt)
    {
        switch (wave)
        {
            case 0: // saw
                return 2.f * ph - 1.f - polyBlep (ph, dt);
            case 1: { // square
                float v = ph < 0.5f ? 1.f : -1.f;
                v += polyBlep (ph, dt);
                v -= polyBlep (std::fmod (ph + 0.5f, 1.f), dt);
                return v;
            }
            case 2: // triangle
                return 1.f - 4.f * std::abs (ph - 0.5f);
            case 3: // sine
                return std::sin (ph * juce::MathConstants<float>::twoPi);
            case 4: { // narrow pulse
                float v = ph < 0.25f ? 1.f : -1.f;
                v += polyBlep (ph, dt);
                v -= polyBlep (std::fmod (ph + 0.75f, 1.f), dt);
                return v * 0.8f;
            }
            default: { // "glass": sine + 3rd harmonic FM-ish, bell-like
                float s = std::sin (ph * juce::MathConstants<float>::twoPi);
                return 0.7f * s + 0.3f * std::sin (ph * juce::MathConstants<float>::twoPi * 3.f + s * 1.5f);
            }
        }
    }

    void renderOsc (const OscParams& o, float* phases, float baseFreq, float sr, float& l, float& r)
    {
        const float freq = baseFreq * std::pow (2.f, o.oct + (o.semi + o.fine * 0.01f) / 12.f);
        const int n = juce::jlimit (1, 7, o.unison);
        const float norm = o.level / std::sqrt ((float) n);

        for (int u = 0; u < n; ++u)
        {
            // spread -1..1 across unison voices
            const float spread = n == 1 ? 0.f : (2.f * u / (float) (n - 1) - 1.f);
            const float det = std::pow (2.f, spread * o.detune * 0.5f / 1200.f); // detune in cents
            const float f = freq * det;
            const float dt = f / sr;
            if (dt >= 0.5f) continue;

            phases[u] += dt;
            if (phases[u] >= 1.f) phases[u] -= 1.f;

            const float s = waveform (o.wave, phases[u], dt) * norm;
            const float pan = 0.5f + spread * 0.5f; // 0..1
            l += s * std::sqrt (1.f - pan);
            r += s * std::sqrt (pan);
        }
    }
};
