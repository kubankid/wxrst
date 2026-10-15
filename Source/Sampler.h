#pragma once
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>
#include <memory>
#include <vector>
#include "Chords.h"

// A loaded sample + things derived from it (peaks for drawing, slices from CHOP).
struct SampleData
{
    juce::AudioBuffer<float> audio;        // as loaded (1 or 2 channels)
    double sampleRate = 44100.0;
    juce::String name;
    juce::File file;
    std::vector<float> peaks;              // 512 bins, max |x| of the mono mix
    std::vector<std::pair<int, int>> slices; // [start, end) sample ranges from CHOP (empty until chopped)

    int length() const { return audio.getNumSamples(); }
};

namespace SampleAnalysis
{
    inline std::vector<float> monoMix (const SampleData& s)
    {
        std::vector<float> m ((size_t) s.length(), 0.f);
        const float g = 1.f / (float) juce::jmax (1, s.audio.getNumChannels());
        for (int c = 0; c < s.audio.getNumChannels(); ++c)
        {
            auto* p = s.audio.getReadPointer (c);
            for (int i = 0; i < s.length(); ++i) m[(size_t) i] += p[i] * g;
        }
        return m;
    }

    inline std::shared_ptr<SampleData> load (const juce::File& file)
    {
        juce::AudioFormatManager fm; fm.registerBasicFormats();
        std::unique_ptr<juce::AudioFormatReader> reader (fm.createReaderFor (file));
        if (reader == nullptr || reader->lengthInSamples <= 0) return nullptr;
        auto s = std::make_shared<SampleData>();
        const int len = (int) juce::jmin<juce::int64> (reader->lengthInSamples, (juce::int64) (reader->sampleRate * 120.0)); // cap at 2 min
        s->audio.setSize ((int) juce::jmin (2u, reader->numChannels), len);
        reader->read (&s->audio, 0, len, 0, true, true);
        s->sampleRate = reader->sampleRate;
        s->name = file.getFileNameWithoutExtension();
        s->file = file;
        auto m = monoMix (*s);
        s->peaks.assign (512, 0.f);
        for (int i = 0; i < len; ++i)
        {
            auto& pk = s->peaks[(size_t) juce::jmin (511, (int) ((juce::int64) i * 512 / len))];
            pk = juce::jmax (pk, std::abs (m[(size_t) i]));
        }
        return s;
    }

    // ---- audio -> MIDI (monophonic pitch tracking) -------------------------------------
    // Works for melodies, bass lines, vocals, one-note-at-a-time stuff. Chords come out as their strongest note.
    inline std::vector<NoteEvent> extractNotes (const SampleData& s, double bpm)
    {
        std::vector<NoteEvent> out;
        auto m = monoMix (s);
        const int N = (int) m.size();
        constexpr int order = 12, frame = 2048, hop = 512, fftN = 1 << order;
        if (N < frame) return out;
        juce::dsp::FFT fft (order);
        std::vector<float> buf ((size_t) fftN * 2);
        const double sr = s.sampleRate;
        const int minLag = (int) (sr / 1200.0), maxLag = (int) (sr / 50.0);

        struct Frame { int note; float rms; };
        std::vector<Frame> frames;
        int prevNote = -1;
        for (int pos = 0; pos + frame <= N; pos += hop)
        {
            float e = 0.f;
            for (int i = 0; i < frame; ++i) e += m[(size_t) (pos + i)] * m[(size_t) (pos + i)];
            const float rms = std::sqrt (e / frame);
            int note = -1;
            if (rms > 0.004f)
            {
                std::fill (buf.begin(), buf.end(), 0.f);
                for (int i = 0; i < frame; ++i)
                {
                    const float w = 0.5f - 0.5f * std::cos (juce::MathConstants<float>::twoPi * i / (frame - 1));
                    buf[(size_t) i] = m[(size_t) (pos + i)] * w;
                }
                fft.performRealOnlyForwardTransform (buf.data(), true);
                // power spectrum -> autocorrelation
                for (int k = 0; k < fftN; ++k)
                {
                    const float re = buf[(size_t) k * 2], im = buf[(size_t) k * 2 + 1];
                    buf[(size_t) k * 2] = re * re + im * im; buf[(size_t) k * 2 + 1] = 0.f;
                }
                fft.performRealOnlyInverseTransform (buf.data());
                const float r0 = buf[0];
                if (r0 > 1e-9f)
                {
                    // pick the smallest lag whose normalised autocorrelation is near the maximum (octave-error guard)
                    float best = 0.f;
                    for (int lag = minLag; lag <= juce::jmin (maxLag, fftN / 2); ++lag) best = juce::jmax (best, buf[(size_t) lag] / r0);
                    if (best > 0.45f)
                    {
                        // candidates: autocorrelation peaks near the maximum (these are the octave-related lags).
                        // Prefer the one closest to the previous frame's pitch so a note doesn't flip octaves mid-way;
                        // with no history, take the smallest lag (highest pitch) which avoids sub-harmonic errors.
                        double bestLagF = -1; int bestDiff = 1000;
                        for (int lag = minLag; lag <= juce::jmin (maxLag, fftN / 2); ++lag)
                        {
                            const float v = buf[(size_t) lag] / r0;
                            if (! (v >= best * 0.85f && v >= buf[(size_t) lag - 1] / r0 && v >= buf[(size_t) lag + 1] / r0)) continue;
                            double lagF = lag;
                            const float y0 = buf[(size_t) lag - 1], y1 = buf[(size_t) lag], y2 = buf[(size_t) lag + 1];
                            const float den = y0 - 2.f * y1 + y2;
                            if (std::abs (den) > 1e-9f) lagF += 0.5 * (y0 - y2) / den;
                            const int cand = (int) std::round (69.0 + 12.0 * std::log2 ((sr / lagF) / 440.0));
                            const int diff = prevNote >= 0 ? std::abs (cand - prevNote) : 0;
                            if (bestLagF < 0 || diff < bestDiff) { bestLagF = lagF; bestDiff = diff; }
                            if (prevNote < 0) break; // smallest lag wins when there's no history
                        }
                        if (bestLagF > 0)
                        {
                            const double f0 = sr / bestLagF;
                            note = (int) std::round (69.0 + 12.0 * std::log2 (f0 / 440.0));
                            if (note < 24 || note > 108) note = -1;
                        }
                    }
                }
            }
            if (note >= 0) prevNote = note; else if (rms < 0.004f) prevNote = -1; // silence resets the octave memory
            frames.push_back ({ note, rms });
        }

        // median-smooth the note track (5 frames) to kill single-frame glitches
        std::vector<int> sm (frames.size());
        for (size_t i = 0; i < frames.size(); ++i)
        {
            std::vector<int> win;
            for (int d = -2; d <= 2; ++d)
            {
                const int j = (int) i + d;
                if (j >= 0 && j < (int) frames.size()) win.push_back (frames[(size_t) j].note);
            }
            std::sort (win.begin(), win.end());
            sm[i] = win[win.size() / 2];
        }

        // segment runs of the same note into events
        const double secPerFrame = hop / sr, beatsPerSec = bpm / 60.0;
        auto q = [] (double b) { return std::round (b * 8.0) / 8.0; }; // snap to 1/32 notes
        size_t i = 0;
        while (i < sm.size())
        {
            if (sm[i] < 0) { ++i; continue; }
            size_t j = i; float peak = 0.f;
            while (j < sm.size() && sm[j] == sm[i]) { peak = juce::jmax (peak, frames[j].rms); ++j; }
            if (j - i >= 3) // >= ~35 ms
            {
                // the pitch shows up while the analysis window is still sliding onto the note: put the
                // onset where the level has actually risen (first frame at >= 60% of the run's peak)
                size_t k = i;
                while (k + 1 < j && frames[k].rms < peak * 0.6f) ++k;
                const double startB = q (k * secPerFrame * beatsPerSec);
                const double endB = juce::jmax (startB + 0.125, q (j * secPerFrame * beatsPerSec));
                const juce::uint8 vel = (juce::uint8) juce::jlimit (40, 127, (int) (60 + peak * 400.f));
                out.push_back ({ startB, sm[i], true, vel });
                out.push_back ({ endB - 0.01, sm[i], false, 0 });
            }
            i = j;
        }
        // fold stray octaves toward the take's median pitch (sub-osc / overtone confusion), drop doubled onsets
        if (! out.empty())
        {
            std::vector<int> ns;
            for (auto& e : out) if (e.on) ns.push_back (e.note);
            std::sort (ns.begin(), ns.end());
            const int median = ns[ns.size() / 2];
            for (auto& e : out)
            {
                while (e.note < median - 8) e.note += 12;
                while (e.note > median + 8) e.note -= 12;
            }
            std::vector<NoteEvent> clean;
            double lastOn = -1.0;
            for (size_t k = 0; k < out.size(); ++k)
            {
                const auto& e = out[k];
                if (e.on)
                {
                    if (lastOn >= 0.0 && e.beat - lastOn < 0.07) { ++k; continue; } // skip this on + its off
                    lastOn = e.beat;
                }
                clean.push_back (e);
            }
            out.swap (clean);
        }
        ChordGen::sortEvents (out);
        return out;
    }

    // ---- transient slicing ("CHOP") ------------------------------------------------------
    inline std::vector<std::pair<int, int>> detectSlices (const SampleData& s, int maxSlices = 64)
    {
        std::vector<std::pair<int, int>> slices;
        auto m = monoMix (s);
        const int N = (int) m.size();
        const int hop = 256;
        if (N < hop * 4) { slices.push_back ({ 0, N }); return slices; }
        // onset strength: rise in log energy of a pre-emphasised signal, 1024-sample window so
        // low kicks don't make the envelope wobble at their own pitch
        const int win = 1024;
        std::vector<float> pre ((size_t) N, 0.f);
        for (int i = 1; i < N; ++i) pre[(size_t) i] = m[(size_t) i] - 0.97f * m[(size_t) i - 1];
        std::vector<float> env;
        for (int pos = 0; pos + win <= N; pos += hop)
        {
            float e = 0.f;
            for (int i = 0; i < win; ++i) e += pre[(size_t) (pos + i)] * pre[(size_t) (pos + i)];
            env.push_back (std::log (e / win + 1e-7f));
        }
        std::vector<float> onset (env.size(), 0.f);
        for (size_t i = 2; i < env.size(); ++i) onset[i] = juce::jmax (0.f, env[i] - juce::jmin (env[i - 1], env[i - 2]));

        const int minGap = (int) (s.sampleRate * 0.08 / hop); // 80 ms
        std::vector<int> onsets;
        for (size_t i = 1; i + 1 < onset.size(); ++i)
        {
            float localMean = 0.f; int cnt = 0;
            for (int d = -16; d <= 16; ++d) { const int j = (int) i + d; if (j >= 0 && j < (int) onset.size()) { localMean += onset[(size_t) j]; ++cnt; } }
            localMean /= (float) juce::jmax (1, cnt);
            const bool peak = onset[i] >= onset[i - 1] && onset[i] >= onset[i + 1] && onset[i] > localMean + 1.0f && onset[i] > 1.6f;
            if (peak && (onsets.empty() || (int) i - onsets.back() >= minGap)) onsets.push_back ((int) i);
        }
        if (onsets.empty() || onsets.front() * hop > (int) (s.sampleRate * 0.05)) onsets.insert (onsets.begin(), 0);
        if ((int) onsets.size() > maxSlices) onsets.resize ((size_t) maxSlices);
        for (size_t i = 0; i < onsets.size(); ++i)
        {
            const int a = onsets[i] * hop;
            const int b = i + 1 < onsets.size() ? onsets[i + 1] * hop : N;
            if (b - a > hop) slices.push_back ({ a, b });
        }
        return slices;
    }

    // Writes each slice as its own WAV; returns the folder.
    inline juce::File writeChops (const SampleData& s, const juce::File& baseDir)
    {
        auto dir = baseDir.getChildFile (juce::File::createLegalFileName (s.name + " chops"));
        dir.deleteRecursively(); dir.createDirectory();
        juce::WavAudioFormat wav;
        for (size_t i = 0; i < s.slices.size(); ++i)
        {
            auto [a, b] = s.slices[i];
            auto f = dir.getChildFile (juce::String::formatted ("%02d ", (int) i + 1) + juce::File::createLegalFileName (s.name) + ".wav");
            std::unique_ptr<juce::FileOutputStream> os (f.createOutputStream());
            if (os == nullptr) continue;
            std::unique_ptr<juce::AudioFormatWriter> w (wav.createWriterFor (os.get(), s.sampleRate, (unsigned) s.audio.getNumChannels(), 24, {}, 0));
            if (w == nullptr) continue;
            os.release();
            w->writeFromAudioSampleBuffer (s.audio, a, b - a);
        }
        return dir;
    }

    // MIDI clip that re-triggers the chops in order (note 48 + slice index), at the host tempo.
    inline std::vector<NoteEvent> chopEvents (const SampleData& s, double bpm)
    {
        std::vector<NoteEvent> out;
        const double beatsPerSample = bpm / 60.0 / s.sampleRate;
        for (size_t i = 0; i < s.slices.size() && i < 64; ++i)
        {
            auto [a, b] = s.slices[i];
            out.push_back ({ a * beatsPerSample, 48 + (int) i, true, 100 });
            out.push_back ({ b * beatsPerSample - 0.01, 48 + (int) i, false, 0 });
        }
        return out;
    }
}

// ---- real-time sample playback (lives inside Engine, renders before the FX chain) -------------------
struct SampleParams
{
    float level = 0.f, tune = 0.f, start = 0.f, end = 1.f;
    int root = 60;
    bool loop = false, reverse = false, chop = false, half = false;   // half = tape-style half speed (one octave down)
    juce::ADSR::Parameters amp;
};

class SamplePlayer
{
public:
    void prepare (double sampleRate) { sr = sampleRate; for (auto& v : voices) { v.env.setSampleRate (sr); v.active = false; } }

    void setSample (std::shared_ptr<const SampleData> s)
    {
        if (s != data) { data = std::move (s); for (auto& v : voices) v.active = false; }
    }

    void allNotesOff() { for (auto& v : voices) v.active = false; }

    void noteOn (int note, float vel, const SampleParams& p)
    {
        if (data == nullptr || p.level <= 0.0001f) return;
        const int N = data->length();
        int a = (int) (p.start * N), b = (int) (p.end * N);
        double ratio = std::pow (2.0, (note - p.root + p.tune) / 12.0) * data->sampleRate / sr;
        if (p.chop && ! data->slices.empty())
        {
            const int idx = note - 48;
            if (idx < 0 || idx >= (int) data->slices.size()) return;
            a = data->slices[(size_t) idx].first; b = data->slices[(size_t) idx].second;
            ratio = data->sampleRate / sr;
        }
        if (p.half) ratio *= 0.5;
        if (b <= a + 1) return;
        // steal: free voice, else the oldest
        Voice* v = nullptr;
        for (auto& c : voices) if (! c.active) { v = &c; break; }
        if (v == nullptr) { v = &voices[0]; for (auto& c : voices) if (c.age > v->age) v = &c; }
        v->active = true; v->note = note; v->vel = vel; v->start = a; v->end = b; v->age = 0;
        v->inc = p.reverse ? -ratio : ratio;
        v->pos = p.reverse ? (double) (b - 1) : (double) a;
        v->env.setParameters (p.amp); v->env.reset(); v->env.noteOn();
    }

    void noteOff (int note) { for (auto& v : voices) if (v.active && v.note == note) v.env.noteOff(); }

    void render (juce::AudioBuffer<float>& out, int startSample, int num, const SampleParams& p)
    {
        if (data == nullptr) return;
        const int nch = data->audio.getNumChannels();
        const float* src[2] { data->audio.getReadPointer (0), data->audio.getReadPointer (nch > 1 ? 1 : 0) };
        auto* L = out.getWritePointer (0, startSample);
        auto* R = out.getNumChannels() > 1 ? out.getWritePointer (1, startSample) : nullptr;
        const float gain = p.level * 0.9f;
        for (auto& v : voices)
        {
            if (! v.active) continue;
            v.env.setParameters (p.amp);
            v.age += num;
            for (int i = 0; i < num; ++i)
            {
                if (v.pos >= v.end - 1 || v.pos < v.start)
                {
                    if (p.loop) v.pos = v.inc > 0 ? (double) v.start : (double) (v.end - 1);
                    else { v.active = false; break; }
                }
                const int i0 = (int) v.pos; const float frac = (float) (v.pos - i0);
                const int i1 = juce::jmin (v.end - 1, i0 + 1);
                const float e = v.env.getNextSample() * v.vel * gain;
                if (! v.env.isActive()) { v.active = false; break; }
                const float l = (src[0][i0] + (src[0][i1] - src[0][i0]) * frac) * e;
                const float r = (src[1][i0] + (src[1][i1] - src[1][i0]) * frac) * e;
                L[i] += l; if (R != nullptr) R[i] += r;
                v.pos += v.inc;
            }
        }
    }

    bool anyActive() const { for (auto& v : voices) if (v.active) return true; return false; }

private:
    struct Voice { bool active = false; int note = -1; float vel = 1.f; double pos = 0, inc = 1; int start = 0, end = 0, age = 0; juce::ADSR env; };
    Voice voices[8];
    std::shared_ptr<const SampleData> data;
    double sr = 44100.0;
};
