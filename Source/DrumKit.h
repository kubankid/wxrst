#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

// Tiny built-in drum kit so drum MIDI (from XTRACT or anywhere) can be auditioned inside WXRST.
// GM mapping: 35/36 kick, 38/40 snare, 39 clap, 42/44 closed hat, 46 open hat, 41-50 toms, anything else = perc.
class DrumKit
{
public:
    void prepare (double sampleRate) { sr = sampleRate; for (auto& v : voices) v.active = false; }
    void allNotesOff() { for (auto& v : voices) v.active = false; }

    // Adds into `out` (does not clear). Returns true if anything was rendered.
    bool process (juce::AudioBuffer<float>& out, const juce::MidiBuffer& midi)
    {
        const int n = out.getNumSamples();
        int pos = 0;
        bool any = false;
        for (const auto meta : midi)
        {
            const int t = juce::jlimit (0, n, meta.samplePosition);
            if (t > pos) { any |= render (out, pos, t - pos); pos = t; }
            const auto m = meta.getMessage();
            if (m.isNoteOn()) trigger (m.getNoteNumber(), m.getFloatVelocity());
        }
        if (pos < n) any |= render (out, pos, n - pos);
        return any;
    }

private:
    enum Type { Kick, Snare, Clap, Hat, OpenHat, Tom, Perc, NumTypes };
    struct Voice { bool active = false; Type type = Kick; double t = 0.0, phase = 0.0; float vel = 1.f, lp = 0.f, lp2 = 0.f; float pitch = 1.f; };
    Voice voices[NumTypes];
    double sr = 44100.0;
    juce::Random rng;

    static Type typeFor (int note)
    {
        if (note == 35 || note == 36) return Kick;
        if (note == 38 || note == 40 || note == 37) return Snare;
        if (note == 39) return Clap;
        if (note == 42 || note == 44) return Hat;
        if (note == 46) return OpenHat;
        if (note >= 41 && note <= 50) return Tom;
        return Perc;
    }

    void trigger (int note, float vel)
    {
        const Type ty = typeFor (note);
        auto& v = voices[ty];
        v.active = true; v.type = ty; v.t = 0.0; v.phase = 0.0; v.vel = juce::jlimit (0.05f, 1.f, vel); v.lp = v.lp2 = 0.f;
        v.pitch = ty == Tom ? std::pow (2.f, (float) (note - 45) / 12.f) : 1.f;
        if (ty == Hat) voices[OpenHat].active = false;   // closed hat chokes the open one
    }

    bool render (juce::AudioBuffer<float>& out, int start, int num)
    {
        bool any = false;
        float* L = out.getWritePointer (0) + start;
        float* R = out.getWritePointer (juce::jmin (1, out.getNumChannels() - 1)) + start;
        const double dt = 1.0 / sr;
        for (auto& v : voices)
        {
            if (! v.active) continue;
            any = true;
            for (int i = 0; i < num; ++i)
            {
                const double t = v.t;
                float s = 0.f;
                switch (v.type)
                {
                    case Kick:
                    {
                        const double f = 48.0 + 160.0 * std::exp (-t * 28.0);
                        v.phase += f * dt;
                        const float body = std::sin ((float) (v.phase * juce::MathConstants<double>::twoPi));
                        const float click = (float) std::exp (-t * 400.0) * (rng.nextFloat() * 2.f - 1.f) * 0.3f;
                        s = std::tanh ((body + click) * 1.8f) * (float) std::exp (-t * 5.5) * 0.9f;
                        if (t > 0.9) v.active = false;
                        break;
                    }
                    case Snare:
                    {
                        const double f = 185.0 + 60.0 * std::exp (-t * 40.0);
                        v.phase += f * dt;
                        const float tone = std::sin ((float) (v.phase * juce::MathConstants<double>::twoPi)) * (float) std::exp (-t * 22.0) * 0.5f;
                        const float noise = rng.nextFloat() * 2.f - 1.f;
                        v.lp += 0.25f * (noise - v.lp);
                        const float hp = noise - v.lp;
                        s = (tone + hp * (float) std::exp (-t * 13.0) * 0.8f) * 0.8f;
                        if (t > 0.45) v.active = false;
                        break;
                    }
                    case Clap:
                    {
                        const float noise = rng.nextFloat() * 2.f - 1.f;
                        v.lp += 0.18f * (noise - v.lp);
                        const float bp = noise - v.lp;
                        const double burst = std::fmod (t, 0.011);
                        const float env = t < 0.033 ? (float) std::exp (-burst * 180.0) : (float) std::exp (-(t - 0.033) * 20.0);
                        s = bp * env * 0.8f;
                        if (t > 0.4) v.active = false;
                        break;
                    }
                    case Hat:
                    case OpenHat:
                    {
                        const float noise = rng.nextFloat() * 2.f - 1.f;
                        v.lp += 0.55f * (noise - v.lp);
                        v.lp2 += 0.55f * ((noise - v.lp) - v.lp2);
                        const float hp = (noise - v.lp) - v.lp2;
                        const double decay = v.type == Hat ? 55.0 : 7.0;
                        s = hp * (float) std::exp (-t * decay) * 0.55f;
                        if (t > (v.type == Hat ? 0.12 : 0.8)) v.active = false;
                        break;
                    }
                    case Tom:
                    case Perc:
                    default:
                    {
                        const double f = (v.type == Tom ? 110.0 : 240.0) * v.pitch * (1.0 + 0.6 * std::exp (-t * 30.0));
                        v.phase += f * dt;
                        s = std::sin ((float) (v.phase * juce::MathConstants<double>::twoPi)) * (float) std::exp (-t * (v.type == Tom ? 9.0 : 18.0)) * 0.7f;
                        if (t > 0.6) v.active = false;
                        break;
                    }
                }
                s *= v.vel;
                L[i] += s; R[i] += s;
                v.t += dt;
                if (! v.active) break;
            }
        }
        return any;
    }
};
