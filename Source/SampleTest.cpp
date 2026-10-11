// Tests audio->MIDI and CHOP on a synthetic melody + drum loop, or on files passed as arguments.
#include "Sampler.h"
#include <cstdio>

static void report (const SampleData& s, double bpm)
{
    auto notes = SampleAnalysis::extractNotes (s, bpm);
    std::printf ("%s: %.1fs @ %.0f Hz\n  notes:", s.name.toRawUTF8(), s.length() / s.sampleRate, s.sampleRate);
    for (auto& e : notes) if (e.on) std::printf (" %s%d@%.2f", ChordGen::keyNames[(e.note % 12 + 12) % 12].toRawUTF8(), e.note / 12 - 1, e.beat);
    auto slices = SampleAnalysis::detectSlices (s);
    std::printf ("\n  chops: %d  at", (int) slices.size());
    for (auto& sl : slices) std::printf (" %.2fs", sl.first / s.sampleRate);
    std::printf ("\n");
}

int main (int argc, char** argv)
{
    if (argc > 1)
    {
        for (int i = 1; i < argc; ++i) if (auto s = SampleAnalysis::load (juce::File (argv[i]))) report (*s, 140.0);
        return 0;
    }
    // synthetic: C4 E4 G4 C5 saw notes, 0.4s each with 0.1s gaps at 44.1k, plus a drum loop of 8 hits at 120 BPM
    SampleData mel; mel.sampleRate = 44100; mel.name = "synthetic melody";
    const int notes[4] = { 60, 64, 67, 72 };
    mel.audio.setSize (1, (int) (44100 * 2.0));
    mel.audio.clear();
    for (int k = 0; k < 4; ++k)
    {
        const double f = 440.0 * std::pow (2.0, (notes[k] - 69) / 12.0);
        const int a = (int) (44100 * 0.5 * k), b = a + (int) (44100 * 0.4);
        for (int i = a; i < b; ++i)
        {
            const double t = (i - a) / 44100.0;
            const double ph = std::fmod (f * t, 1.0);
            mel.audio.setSample (0, i, (float) ((2.0 * ph - 1.0) * 0.4 * std::exp (-t * 2.0)));
        }
    }
    report (mel, 120.0);

    SampleData drums; drums.sampleRate = 44100; drums.name = "synthetic drums";
    drums.audio.setSize (1, (int) (44100 * 2.0)); drums.audio.clear();
    juce::Random r (7);
    for (int h = 0; h < 8; ++h)
    {
        const int a = (int) (44100 * 0.25 * h);
        for (int i = 0; i < 6000; ++i)
        {
            const double t = i / 44100.0;
            const float kick = (float) (std::sin (2 * 3.14159 * (50 + 120 * std::exp (-t * 40)) * t) * std::exp (-t * 12));
            const float hat = (h % 2 == 1) ? (r.nextFloat() * 2.f - 1.f) * (float) std::exp (-t * 80) * 0.4f : 0.f;
            drums.audio.addSample (0, a + i, (kick + hat) * 0.7f);
        }
    }
    report (drums, 120.0);
    return 0;
}
