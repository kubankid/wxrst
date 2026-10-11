// Generates progressions across keys / scales / styles and reports how melody notes sit on the key's 1 / 4 / 5 degrees.
#include "Chords.h"
#include <cstdio>

int main()
{
    int total = 0, anchors = 0, strongTotal = 0, strongAnchors = 0, finals = 0, finalsOk = 0;
    for (int key = 0; key < 12; ++key)
        for (int scale = 0; scale < 6; ++scale)
            for (int style = 0; style < 9; ++style)
            {
                ChordGen::Options o; o.key = key; o.scale = scale; o.style = style; o.bars = 4; o.rhythm = style % 4; o.seed = (unsigned) (key * 100 + scale * 10 + style + 1);
                auto prog = ChordGen::generate (o);
                // rebuild the scale pitch classes the same way the generator sees them
                const int* sc = ChordGen::scales[scale];
                const int a1 = (key + sc[0]) % 12, a4 = (key + sc[3]) % 12, a5 = (key + sc[4]) % 12;
                int lastOnNote = -1;
                for (auto& e : prog.melodyEvents)
                {
                    if (! e.on) continue;
                    const int pc = (e.note % 12 + 12) % 12;
                    const bool anchor = pc == a1 || pc == a4 || pc == a5;
                    ++total; anchors += anchor;
                    if (std::abs (e.beat - std::round (e.beat)) < 0.01) { ++strongTotal; strongAnchors += anchor; }
                    lastOnNote = e.note;
                }
                if (lastOnNote >= 0) { ++finals; const int pc = (lastOnNote % 12 + 12) % 12; finalsOk += (pc == a1 || pc == a5); }
                if (key == 9 && scale == 0 && style == 0)
                {
                    std::printf ("Example (A minor, Drake/OVO): %s\n  melody:", ChordGen::chordListString (prog).toRawUTF8());
                    for (auto& e : prog.melodyEvents) if (e.on) std::printf (" %s%d", ChordGen::keyNames[(e.note % 12 + 12) % 12].toRawUTF8(), e.note / 12 - 1);
                    std::printf ("\n");
                }
            }
    std::printf ("melody notes on 1/4/5: %d/%d (%.0f%%)   downbeats on 1/4/5: %d/%d (%.0f%%)   final note on 1/5: %d/%d\n",
                 anchors, total, 100.0 * anchors / total, strongAnchors, strongTotal, 100.0 * strongAnchors / strongTotal, finalsOk, finals);
    return 0;
}
