#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <vector>
#include <random>
#include <algorithm>

struct NoteEvent
{
    double beat = 0.0;
    int note = 60;
    bool on = true;
    juce::uint8 vel = 90;
};

struct Chord
{
    juce::String name;
    std::vector<int> notes;   // MIDI note numbers (for display / melody generation)
    double startBeat = 0.0, lengthBeats = 4.0;
};

struct Progression
{
    std::vector<Chord> chords;
    std::vector<NoteEvent> events;        // chord events, sorted by beat
    std::vector<NoteEvent> melodyEvents;  // melody events, sorted by beat
    std::vector<NoteEvent> bassEvents;    // 808 / bass line, sorted by beat
    double totalBeats = 16.0;
    juce::String title;
    int key = 0, scale = 0;
};

namespace ChordGen
{
    inline const juce::StringArray keyNames    { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    inline const juce::StringArray scaleNames  { "Minor", "Major", "Dorian", "Harmonic Minor", "Lydian", "Phrygian" };
    inline const juce::StringArray styleNames  { "Drake / OVO", "Lo-Fi", "Dreamy", "Sad", "Uplifting", "PS2 Menu", "Dark Trap", "Neo-Soul", "Ambient" };
    inline const juce::StringArray barNames    { "4 bars", "8 bars" };
    inline const juce::StringArray rhythmNames { "Whole bars", "Mixed", "Pushed", "Half bars" };

    inline const int scales[6][7] = {
        { 0, 2, 3, 5, 7, 8, 10 },  // minor
        { 0, 2, 4, 5, 7, 9, 11 },  // major
        { 0, 2, 3, 5, 7, 9, 10 },  // dorian
        { 0, 2, 3, 5, 7, 8, 11 },  // harmonic minor
        { 0, 2, 4, 6, 7, 9, 11 },  // lydian
        { 0, 1, 3, 5, 7, 8, 10 },  // phrygian
    };

    inline const std::vector<std::vector<std::vector<int>>> templates = {
        { { 0, 5, 2, 6 }, { 0, 3, 5, 6 }, { 0, 6, 5, 6 }, { 0, 5, 0, 6 }, { 3, 5, 0, 6 }, { 0, 5, 3, 6 }, { 5, 6, 0, 0 }, { 0, 2, 5, 6 } },
        { { 1, 4, 0, 0 }, { 3, 2, 1, 0 }, { 0, 5, 3, 4 }, { 1, 4, 0, 3 }, { 3, 4, 2, 5 }, { 0, 2, 3, 4 } },
        { { 0, 3, 5, 3 }, { 3, 0, 5, 4 }, { 0, 2, 3, 5 }, { 5, 3, 0, 4 }, { 0, 4, 5, 3 } },
        { { 0, 6, 5, 4 }, { 0, 5, 3, 4 }, { 5, 3, 0, 4 }, { 0, 4, 5, 3 }, { 0, 2, 3, 6 } },
        { { 0, 4, 5, 3 }, { 5, 3, 0, 4 }, { 0, 3, 4, 3 }, { 0, 5, 3, 4 }, { 3, 4, 5, 0 } },
        { { 0, 3, 0, 3 }, { 0, 5, 0, 5 }, { 0, 2, 0, 2 }, { 0, 3, 5, 3 }, { 0, 4, 0, 4 } },
        // Dark Trap: minor, tense, half-step and tritone-ish motion (iv / bII flavoured via degree 1 in phrygian etc.)
        { { 0, 0, 3, 3 }, { 0, 3, 4, 4 }, { 0, 5, 4, 0 }, { 0, 1, 0, 1 }, { 0, 3, 0, 4 }, { 5, 4, 0, 0 } },
        // Neo-Soul: lots of ii-V, iii-vi, chromatic feel through 9ths
        { { 1, 4, 0, 5 }, { 2, 5, 1, 4 }, { 0, 2, 3, 4 }, { 3, 4, 2, 5 }, { 1, 2, 3, 4 }, { 0, 5, 1, 4 } },
        // Ambient: very slow, two or three chords
        { { 0, 3, 3, 0 }, { 0, 0, 5, 5 }, { 0, 2, 2, 0 }, { 3, 3, 0, 0 } },
    };

    inline juce::String qualityName (int third, int fifth, int seventh, bool ninth)
    {
        const bool minor = third == 3, dim = fifth == 6, maj7 = seventh == 11;
        if (dim) return minor ? (maj7 ? "dim(maj7)" : "m7b5") : "7b5";
        if (minor) return ninth ? "m9" : "m7";
        return maj7 ? (ninth ? "maj9" : "maj7") : (ninth ? "9" : "7");
    }

    inline void sortEvents (std::vector<NoteEvent>& ev)
    {
        std::stable_sort (ev.begin(), ev.end(), [] (const NoteEvent& a, const NoteEvent& b)
        {
            if (a.beat != b.beat) return a.beat < b.beat;
            return (! a.on) && b.on; // offs before ons at the same time
        });
    }

    struct Options
    {
        int key = 9, scale = 0, style = 0, bars = 4, rhythm = 0;
        bool strum = false;
        unsigned seed = 1;
    };

    inline Progression generate (const Options& o)
    {
        std::mt19937 rng (o.seed);
        auto chance = [&] (float p) { return std::uniform_real_distribution<float> (0.f, 1.f) (rng) < p; };
        auto pick   = [&] (int n) { return std::uniform_int_distribution<int> (0, juce::jmax (0, n - 1)) (rng); };

        const int scaleIdx = juce::jlimit (0, 5, o.scale);
        const int* scale = scales[scaleIdx];
        const auto& tpls = templates[(size_t) juce::jlimit (0, (int) templates.size() - 1, o.style)];

        // --- pick degrees and durations ------------------------------------
        struct Slot { int degree; double len; };
        std::vector<Slot> slots;
        std::vector<int> degrees;
        while ((int) degrees.size() < o.bars)
        {
            auto t = tpls[(size_t) pick ((int) tpls.size())];
            degrees.insert (degrees.end(), t.begin(), t.end());
        }
        degrees.resize ((size_t) o.bars);

        for (int b = 0; b < o.bars; ++b)
        {
            const int d = degrees[(size_t) b];
            const bool split = o.rhythm == 3 || (o.rhythm == 1 && chance (0.35f));
            if (split)
            {
                // second half: a neighbouring degree or a passing chord from the template pool
                int d2 = d;
                int tries = 0;
                while (d2 == d && tries++ < 8)
                    d2 = chance (0.5f) ? ((d + (chance (0.5f) ? 2 : 5)) % 7) : tpls[(size_t) pick ((int) tpls.size())][(size_t) pick (4)];
                slots.push_back ({ d, 2.0 });
                slots.push_back ({ d2, 2.0 });
            }
            else slots.push_back ({ d, 4.0 });
        }

        // --- voice each chord -------------------------------------------------
        Progression prog;
        prog.key = o.key; prog.scale = scaleIdx;
        double beat = 0.0;
        int prevTop = -1;

        for (auto& s : slots)
        {
            const int degree = s.degree;
            auto scaleNote = [&] (int idx) { return 12 * (idx / 7) + scale[((idx % 7) + 7) % 7]; };
            const int root = scaleNote (degree);
            const int third = scaleNote (degree + 2) - root;
            const int fifth = scaleNote (degree + 4) - root;
            const int seventh = scaleNote (degree + 6) - root;
            const int ninth = scaleNote (degree + 8) - root;
            const bool useNinth = chance (o.style == 1 || o.style == 7 ? 0.7f : 0.35f);

            const int base = 48 + ((o.key + root) % 12);
            std::vector<int> voicing { base, base + third, base + fifth, base + seventh };
            if (useNinth) voicing.push_back (base + ninth);

            int bestShift = 0, bestDist = 1000;
            for (int inv = 0; inv < (int) voicing.size(); ++inv)
            {
                std::vector<int> v = voicing;
                for (int k = 0; k < inv; ++k) v[(size_t) k] += 12;
                const int top = *std::max_element (v.begin(), v.end());
                const int dist = prevTop < 0 ? std::abs (top - 67) : std::abs (top - prevTop);
                if (dist < bestDist) { bestDist = dist; bestShift = inv; }
            }
            for (int k = 0; k < bestShift; ++k) voicing[(size_t) k] += 12;
            for (auto& n : voicing) while (n > 74) n -= 12;
            std::sort (voicing.begin(), voicing.end());
            prevTop = voicing.back();

            Chord c;
            c.notes = voicing;
            c.notes.insert (c.notes.begin(), 36 + ((o.key + root) % 12)); // bass root first
            c.name = keyNames[(o.key + root) % 12] + qualityName (third, fifth, seventh, useNinth);
            c.startBeat = beat;
            c.lengthBeats = s.len;
            beat += s.len;
            prog.chords.push_back (c);
        }
        prog.totalBeats = beat;

        // --- pushed feel: anticipate every change (except the first) by an 8th
        if (o.rhythm == 2)
            for (size_t i = 1; i < prog.chords.size(); ++i)
            {
                prog.chords[i].startBeat -= 0.5;
                prog.chords[i].lengthBeats += 0.5;
                prog.chords[i - 1].lengthBeats -= 0.5;
            }

        // --- flatten to events (with optional strum + humanised velocity) ------
        for (auto& c : prog.chords)
        {
            for (size_t i = 0; i < c.notes.size(); ++i)
            {
                const double strumOff = o.strum ? 0.045 * (double) i : 0.0;
                const int n = c.notes[i];
                const int baseVel = n < 48 ? 100 : 86;
                const juce::uint8 vel = (juce::uint8) juce::jlimit (1, 127, baseVel + pick (13) - 6);
                prog.events.push_back ({ c.startBeat + strumOff, n, true, vel });
                prog.events.push_back ({ c.startBeat + c.lengthBeats - 0.02, n, false, 0 });
            }
        }
        sortEvents (prog.events);

        // --- melody ---------------------------------------------------------------
        {
            int lastNote = -1;
            auto inScale = [&] (int n) { const int pc = ((n - o.key) % 12 + 12) % 12; for (int k = 0; k < 7; ++k) if (scale[k] == pc) return true; return false; };

            for (auto& c : prog.chords)
            {
                // chord tones (pitch classes) above the voicing, in the 72..86 window
                std::vector<int> tones;
                for (size_t i = 1; i < c.notes.size(); ++i) tones.push_back (c.notes[i] % 12);

                double t = c.startBeat;
                const double end = c.startBeat + c.lengthBeats;
                bool first = true;
                while (t < end - 0.01)
                {
                    // rhythm: mostly 8ths / quarters, some rests, sparser for pad styles
                    double len = chance (0.5f) ? 1.0 : (chance (0.6f) ? 0.5 : 2.0);
                    len = juce::jmin (len, end - t);
                    const float restProb = (o.style == 0 || o.style == 5 || o.style == 8) ? 0.5f : (o.style == 6 ? 0.4f : 0.3f);
                    if (! first && chance (restProb)) { t += len; continue; }

                    int note;
                    if (first || chance (0.55f))
                    {
                        // chord tone near the last note
                        int best = -1, bestDist = 100;
                        for (int oct = 72; oct <= 84; oct += 12)
                            for (int pc : tones)
                            {
                                const int cand = oct + pc;
                                if (cand > 86) continue;
                                const int target = lastNote < 0 ? 78 : lastNote;
                                const int d = std::abs (cand - target);
                                if (d < bestDist && (lastNote < 0 || cand != lastNote || chance (0.3f))) { bestDist = d; best = cand; }
                            }
                        note = best < 0 ? 72 + tones.front() : best;
                    }
                    else
                    {
                        // stepwise scale motion
                        const int dir = chance (0.5f) ? 1 : -1;
                        note = lastNote < 0 ? 76 : lastNote;
                        for (int step = 0; step < 3; ++step) { note += dir; if (inScale (note)) break; }
                        if (! inScale (note)) note = lastNote;
                        note = juce::jlimit (70, 88, note);
                    }

                    const juce::uint8 vel = (juce::uint8) juce::jlimit (1, 127, 84 + pick (17) - 8);
                    prog.melodyEvents.push_back ({ t, note, true, vel });
                    prog.melodyEvents.push_back ({ t + len * 0.9, note, false, 0 });
                    lastNote = note;
                    t += len;
                    first = false;
                }
            }
            sortEvents (prog.melodyEvents);
        }

        // --- 808 / bass line ----------------------------------------------------
        {
            const bool trap = o.style == 0 || o.style == 6;
            for (size_t ci = 0; ci < prog.chords.size(); ++ci)
            {
                const auto& c = prog.chords[ci];
                const int root = 24 + (c.notes.front() % 12);     // octave 1
                const int fifth = root + 7;
                const double start = c.startBeat, end = c.startBeat + c.lengthBeats;

                struct Hit { double t, len; int note; };
                std::vector<Hit> hits;
                if (c.lengthBeats >= 4.0)
                {
                    const int pattern = pick (trap ? 4 : 3);
                    switch (pattern)
                    {
                        case 0: hits = { { 0.0, 3.5, root } }; break;                                   // long 808
                        case 1: hits = { { 0.0, 2.0, root }, { 2.5, 1.5, root } }; break;                // push on the and of 2
                        case 2: hits = { { 0.0, 1.5, root }, { 1.5, 1.5, fifth }, { 3.0, 1.0, root } }; break;
                        default: hits = { { 0.0, 0.75, root }, { 0.75, 0.75, root }, { 1.5, 2.0, root }, { 3.5, 0.5, root + 12 } }; break; // trap roll
                    }
                }
                else hits = chance (0.5f) ? std::vector<Hit> { { 0.0, c.lengthBeats - 0.25, root } }
                                          : std::vector<Hit> { { 0.0, 1.0, root }, { 1.0, c.lengthBeats - 1.25, root } };

                // pickup into the next chord
                if (ci + 1 < prog.chords.size() && chance (trap ? 0.45f : 0.25f) && ! hits.empty())
                {
                    hits.back().len = juce::jmin (hits.back().len, (end - start) - hits.back().t - 0.5);
                    const int nextRoot = 24 + (prog.chords[ci + 1].notes.front() % 12);
                    hits.push_back ({ c.lengthBeats - 0.5, 0.45, chance (0.5f) ? nextRoot : root });
                }

                for (auto& h : hits)
                {
                    if (h.len <= 0.05) continue;
                    const juce::uint8 vel = (juce::uint8) juce::jlimit (1, 127, 110 + pick (11) - 5);
                    prog.bassEvents.push_back ({ start + h.t, h.note, true, vel });
                    prog.bassEvents.push_back ({ start + h.t + h.len - 0.02, h.note, false, 0 });
                }
            }
            sortEvents (prog.bassEvents);
        }

        prog.title = keyNames[o.key] + " " + scaleNames[scaleIdx] + " - " + styleNames[o.style];
        return prog;
    }

    inline juce::File writeMidiFile (const std::vector<NoteEvent>& events, const juce::String& fileName, double bpm)
    {
        const int tpq = 960;
        juce::MidiMessageSequence seq;
        seq.addEvent (juce::MidiMessage::tempoMetaEvent ((int) (60000000.0 / bpm)), 0.0);
        for (auto& e : events)
            seq.addEvent (e.on ? juce::MidiMessage::noteOn (1, e.note, e.vel) : juce::MidiMessage::noteOff (1, e.note),
                          juce::jmax (0.0, e.beat * tpq));
        seq.updateMatchedPairs();

        juce::MidiFile mf;
        mf.setTicksPerQuarterNote (tpq);
        mf.addTrack (seq);

        auto dir = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory).getChildFile ("ICEWATER Chords");
        dir.createDirectory();
        auto file = dir.getChildFile (juce::File::createLegalFileName (fileName + ".mid"));
        file.deleteFile();
        juce::FileOutputStream out (file);
        if (out.openedOk()) mf.writeTo (out);
        return file;
    }

    inline juce::String chordListString (const Progression& p)
    {
        juce::String s;
        for (auto& c : p.chords) s += (s.isEmpty() ? "" : " ") + c.name;
        return s;
    }
}
