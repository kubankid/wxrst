#include "PluginEditor.h"

using namespace IceColours;

// ---------------------------------------------------------------------------
AnimatedBackground::AnimatedBackground()
{
    juce::Random r;
    const juce::Colour cols[] = { juce::Colour (0xff2b8cff), juce::Colour (0xff3fd8d0), juce::Colour (0xff6a5cff),
                                  juce::Colour (0xff6fe3ff), juce::Colour (0xff2457c9) };
    for (int i = 0; i < 6; ++i)
        blobs.push_back ({ r.nextFloat(), r.nextFloat(), 0.25f + r.nextFloat() * 0.3f,
                           (r.nextFloat() - 0.5f) * 0.0025f, (r.nextFloat() - 0.5f) * 0.0025f,
                           r.nextFloat() * 6.28f, cols[i % 5] });
    for (int i = 0; i < 90; ++i)
        particles.push_back ({ r.nextFloat(), r.nextFloat() * 0.1f, 0.0004f + r.nextFloat() * 0.0012f,
                               (r.nextFloat() - 0.5f) * 0.0003f, 1.f + r.nextFloat() * 2.2f,
                               r.nextFloat() * 6.28f, 0.5f + r.nextFloat() * 1.5f });
    setInterceptsMouseClicks (false, false);
    startTimerHz (24);
}

void AnimatedBackground::resized()
{
    glow = juce::Image (juce::Image::ARGB, juce::jmax (1, getWidth() / 8), juce::jmax (1, getHeight() / 8), true);
    renderGlow();
}

void AnimatedBackground::timerCallback()
{
    t += 1.f / 24.f;
    ++frame;

    // particles live in the header strip only: small dirty rect, cheap to composite at 24 fps
    const float headerFrac = 82.f / (float) juce::jmax (1, getHeight());
    for (auto& p : particles)
    {
        p.y -= p.vy * 0.35f; p.x += p.vx + 0.0002f * std::sin (t * p.speed + p.phase);
        if (p.y < -0.01f) { p.y = headerFrac; p.x = juce::Random::getSystemRandom().nextFloat(); }
        if (p.x < 0.f) p.x += 1.f; if (p.x > 1.f) p.x -= 1.f;
    }
    repaint (0, 0, getWidth(), 82);

    // the aurora drifts at 6 fps (it's blurred, so this is invisible) and repaints the whole window
    if ((frame & 3) == 0)
    {
        for (auto& b : blobs)
        {
            b.x += b.dx * 4.f; b.y += b.dy * 4.f;
            if (b.x < -0.2f || b.x > 1.2f) b.dx = -b.dx;
            if (b.y < -0.2f || b.y > 1.2f) b.dy = -b.dy;
        }
        renderGlow();
        repaint();
    }
}

void AnimatedBackground::renderGlow()
{
    if (! glow.isValid()) return;
    juce::Graphics g (glow);
    g.fillAll (IceColours::bg);
    const float w = (float) glow.getWidth(), h = (float) glow.getHeight();
    const bool midnight = IceColours::theme == 1;
    // MIDNIGHT swaps the aurora for magenta / violet / sunset-orange haze
    const juce::Colour nightCols[] = { juce::Colour (0xffff2fa0), juce::Colour (0xff7b2cff), juce::Colour (0xffff7a3c),
                                       juce::Colour (0xffb400ff), juce::Colour (0xff3b1a8a) };
    int i = 0;
    for (auto& b : blobs)
    {
        const float pulse = 0.34f + 0.12f * std::sin (t * 0.35f + b.phase);
        const float rad = b.r * w;
        const auto c = midnight ? nightCols[i++ % 5] : b.c;
        juce::ColourGradient grad (c.withAlpha (pulse * (midnight ? 0.8f : 1.f)), b.x * w, b.y * h, juce::Colours::transparentBlack, b.x * w + rad, b.y * h, true);
        g.setGradientFill (grad);
        g.fillEllipse (b.x * w - rad, b.y * h - rad, rad * 2.f, rad * 2.f);
    }
}

void AnimatedBackground::paint (juce::Graphics& g)
{
    const bool midnight = IceColours::theme == 1;
    const float w = (float) getWidth(), h = (float) getHeight();

    g.setImageResamplingQuality (juce::Graphics::lowResamplingQuality);
    g.drawImage (glow, getLocalBounds().toFloat());

    if (midnight)
    {
        // --- synthwave: perspective grid scrolling toward you, below the header ---
        const float horizon = 82.f;
        const float vx = w * 0.5f;
        g.setColour (juce::Colour (0xffff2fa0).withAlpha (0.10f));
        for (int i = -9; i <= 9; ++i)
        {
            const float xBottom = vx + (float) i * w * 0.16f;
            g.drawLine (vx, horizon, xBottom, h, 1.f);
        }
        const float scroll = std::fmod (t * 0.25f, 1.f);
        for (int i = 0; i < 12; ++i)
        {
            const float u = std::fmod ((float) i / 12.f + scroll, 1.f);
            const float y = horizon + (h - horizon) * u * u;
            g.setColour (juce::Colour (0xffff2fa0).withAlpha (0.04f + 0.14f * u));
            g.drawLine (0.f, y, w, y, 1.f);
        }
        // --- sunset sun behind the logo, with the classic horizontal cuts ---
        const float sr = 54.f, sx = 150.f, sy = 70.f;
        juce::ColourGradient sun (juce::Colour (0xffffd36b), sx, sy - sr, juce::Colour (0xffff2fa0), sx, sy + sr * 0.4f, false);
        g.setGradientFill (sun);
        g.fillEllipse (sx - sr, sy - sr, sr * 2.f, sr * 2.f);
        g.setColour (IceColours::bg);
        for (int i = 0; i < 6; ++i)
        {
            const float yy = sy - 4.f + (float) i * 7.f;
            g.fillRect (sx - sr - 1.f, yy, sr * 2.f + 2.f, 1.5f + (float) i * 0.6f);
        }
        g.setColour (IceColours::bg.withAlpha (0.0f));
        // stars (particles become fixed twinkling stars)
        for (auto& p : particles)
        {
            const float tw = 0.5f + 0.5f * std::sin (t * p.speed * 2.f + p.phase);
            g.setColour (juce::Colours::white.withAlpha (0.15f + 0.6f * tw));
            g.fillEllipse (p.x * w, p.phase / 6.3f * 78.f, p.size * 0.7f, p.size * 0.7f);
        }
    }
    else
    {
        // vignette so the edges stay dark
        juce::ColourGradient vig (juce::Colours::transparentBlack, w * 0.5f, h * 0.5f,
                                  juce::Colour (0xff05070b).withAlpha (0.45f), 0.f, 0.f, true);
        g.setGradientFill (vig);
        g.fillAll();

        // ice dust
        for (auto& p : particles)
        {
            const float tw = 0.35f + 0.65f * (0.5f + 0.5f * std::sin (t * p.speed + p.phase));
            g.setColour (IceColours::iceGlow.withAlpha (0.2f + 0.6f * tw));
            g.fillEllipse (p.x * w, p.y * h, p.size, p.size);
        }
    }

    // logo with a slow shimmer (chrome-pink in MIDNIGHT)
    g.setFont (juce::Font (juce::FontOptions (34.f, juce::Font::bold)));
    const float sh = 0.5f + 0.5f * std::sin (t * 0.6f);
    juce::ColourGradient logo (midnight ? juce::Colour (0xffffe08a) : IceColours::iceGlow, 20.f + sh * 60.f, 10.f,
                               midnight ? juce::Colour (0xffff2fa0) : IceColours::iceDeep, 260.f + sh * 60.f, 60.f, false);
    if (midnight)
    {
        // dark halo so the chrome lettering stays readable over the sun
        g.setColour (IceColours::bg.withAlpha (0.75f));
        for (auto off : { juce::Point<int> (2, 2), juce::Point<int> (-1, 1), juce::Point<int> (1, -1), juce::Point<int> (0, 3) })
            g.drawText ("WXRST", 22 + off.x, 10 + off.y, 300, 40, juce::Justification::centredLeft);
    }
    g.setGradientFill (logo);
    g.drawText ("WXRST", 22, 10, 300, 40, juce::Justification::centredLeft);
    g.setColour (IceColours::dimText);
    g.setFont (juce::Font (juce::FontOptions (10.f)));
    g.drawText (midnight ? "THE WORST PLUGIN  /  MIDNIGHT MODE  /  SYNTHWAVE  /  16-BIT" : "THE WORST PLUGIN  /  LO-FI PADS  /  808s  /  CHORD ENGINE",
                24, 48, 300, 14, juce::Justification::centredLeft);
}

// ---------------------------------------------------------------------------
void DiceButton::paintButton (juce::Graphics& g, bool over, bool down)
{
    using namespace IceColours;
    auto r = getLocalBounds().toFloat().reduced (2.f);
    const float sz = juce::jmin (r.getWidth(), r.getHeight());
    auto cube = r.withSizeKeepingCentre (sz, sz);
    if (down) cube = cube.reduced (1.5f);

    if (over) { g.setColour (ice.withAlpha (0.25f)); g.fillRoundedRectangle (cube.expanded (3.f), 8.f); }
    juce::ColourGradient body (iceGlow, cube.getX(), cube.getY(), iceDeep, cube.getRight(), cube.getBottom(), false);
    g.setGradientFill (body);
    g.fillRoundedRectangle (cube, sz * 0.22f);
    g.setColour (juce::Colours::white.withAlpha (0.35f));
    g.drawRoundedRectangle (cube.reduced (0.5f), sz * 0.22f, 1.f);

    // pips
    const float c = sz * 0.5f, o = sz * 0.26f, pr = sz * 0.085f;
    const float cx = cube.getX() + c, cy = cube.getY() + c;
    auto pip = [&] (float dx, float dy) { g.fillEllipse (cx + dx - pr, cy + dy - pr, pr * 2.f, pr * 2.f); };
    g.setColour (juce::Colour (0xff0a0d14));
    switch (face)
    {
        case 1: pip (0, 0); break;
        case 2: pip (-o, -o); pip (o, o); break;
        case 3: pip (-o, -o); pip (0, 0); pip (o, o); break;
        case 4: pip (-o, -o); pip (o, -o); pip (-o, o); pip (o, o); break;
        case 5: pip (-o, -o); pip (o, -o); pip (0, 0); pip (-o, o); pip (o, o); break;
        default: pip (-o, -o); pip (o, -o); pip (-o, 0); pip (o, 0); pip (-o, o); pip (o, o); break;
    }
}

// ---------------------------------------------------------------------------
Knob::Knob (juce::AudioProcessorValueTreeState& s, const juce::String& id, const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider.setPopupDisplayEnabled (true, true, nullptr);
    slider.setDoubleClickReturnValue (true, s.getParameter (id)->convertFrom0to1 (s.getParameter (id)->getDefaultValue()));
    addAndMakeVisible (slider);
    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setFont (juce::Font (juce::FontOptions (10.f, juce::Font::bold)));
    label.setColour (juce::Label::textColourId, dimText);
    addAndMakeVisible (label);
    att = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (s, id, slider);
}

void Knob::resized()
{
    auto r = getLocalBounds();
    label.setBounds (r.removeFromBottom (14));
    slider.setBounds (r);
}

void Panel::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour (panel);
    g.fillRoundedRectangle (r, 8.f);
    g.setColour (panelEdge);
    g.drawRoundedRectangle (r.reduced (0.5f), 8.f, 1.f);

    auto titleArea = r.removeFromTop (22.f);
    g.setColour (ice.withAlpha (0.08f));
    g.fillRoundedRectangle (titleArea.withTrimmedBottom (-6.f), 8.f);
    g.setColour (ice);
    g.setFont (juce::Font (juce::FontOptions (11.f, juce::Font::bold)));
    g.drawText (title, titleArea.toNearestInt().withTrimmedLeft (10), juce::Justification::centredLeft);
    g.setColour (ice.withAlpha (0.25f));
    g.drawLine (r.getX() + 8.f, titleArea.getBottom(), r.getRight() - 8.f, titleArea.getBottom(), 1.f);
}

void Scope::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour (screen);
    g.fillRoundedRectangle (r, 6.f);
    g.setColour (panelEdge);
    g.drawRoundedRectangle (r.reduced (0.5f), 6.f, 1.f);

    g.setColour (ice.withAlpha (0.07f));
    for (int i = 1; i < 4; ++i) g.drawHorizontalLine ((int) (r.getHeight() * i / 4.f), r.getX(), r.getRight());
    for (int i = 1; i < 8; ++i) g.drawVerticalLine ((int) (r.getWidth() * i / 8.f), r.getY(), r.getBottom());

    juce::Path p;
    const int N = IcewaterProcessor::scopeSize;
    const int start = proc.scopeWrite.load();
    const float midY = r.getCentreY(), amp = r.getHeight() * 0.45f;
    for (int i = 0; i < N; ++i)
    {
        const float v = juce::jlimit (-1.f, 1.f, proc.scope[(size_t) ((start + i) % N)].load());
        const float x = r.getX() + 2.f + (r.getWidth() - 4.f) * i / (float) (N - 1);
        const float y = midY - v * amp;
        if (i == 0) p.startNewSubPath (x, y); else p.lineTo (x, y);
    }
    g.setColour (ice.withAlpha (0.25f));
    g.strokePath (p, juce::PathStrokeType (4.f));
    g.setColour (iceGlow);
    g.strokePath (p, juce::PathStrokeType (1.4f));

    const float lv = juce::jlimit (0.f, 1.f, proc.levelL.load() * 2.5f);
    const float rv = juce::jlimit (0.f, 1.f, proc.levelR.load() * 2.5f);
    auto meter = r.reduced (6.f).removeFromRight (6.f);
    auto ml = meter.removeFromLeft (2.5f); meter.removeFromLeft (1.f); auto mr = meter;
    g.setColour (knobRing); g.fillRect (ml); g.fillRect (mr);
    g.setColour (ice);
    g.fillRect (ml.removeFromBottom (ml.getHeight() * lv));
    g.fillRect (mr.removeFromBottom (mr.getHeight() * rv));
}

void EnvDisplay::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced (4.f, 3.f);
    g.setColour (screen);
    g.fillRoundedRectangle (r, 4.f);

    // time axis: attack + decay + fixed sustain hold + release, log-ish scaled
    auto tscale = [] (float s) { return std::log1p (s * 4.f); };
    const float a = tscale (A->load()), d = tscale (D->load()), rel = tscale (R->load()), hold = 0.9f;
    const float total = a + d + hold + rel;
    const float s = S->load();
    const float w = r.getWidth() - 6.f, h = r.getHeight() - 6.f, x0 = r.getX() + 3.f, y0 = r.getBottom() - 3.f;

    juce::Path p;
    p.startNewSubPath (x0, y0);
    const float xa = x0 + w * a / total, xd = xa + w * d / total, xh = xd + w * hold / total, xr = xh + w * rel / total;
    p.lineTo (xa, y0 - h);
    p.quadraticTo (xa + (xd - xa) * 0.3f, y0 - h * s - (h - h * s) * 0.3f, xd, y0 - h * s);
    p.lineTo (xh, y0 - h * s);
    p.quadraticTo (xh + (xr - xh) * 0.3f, y0 - h * s * 0.3f, xr, y0);

    juce::Path fill (p);
    fill.lineTo (x0, y0); fill.closeSubPath();
    g.setGradientFill (juce::ColourGradient (iceDeep.withAlpha (0.35f), 0.f, r.getY(), iceDeep.withAlpha (0.02f), 0.f, r.getBottom(), false));
    g.fillPath (fill);
    g.setColour (iceGlow);
    g.strokePath (p, juce::PathStrokeType (1.5f));
}

void MidiDragPad::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    auto prog = proc.getProgression();
    const bool has = prog != nullptr && file.existsAsFile();

    g.setColour (screen);
    g.fillRoundedRectangle (r, 6.f);
    g.setColour (has ? ice.withAlpha (0.6f) : panelEdge);
    g.drawRoundedRectangle (r.reduced (0.5f), 6.f, 1.f);

    const std::vector<NoteEvent>* list = prog == nullptr ? nullptr
        : kind == 0 ? &prog->events : kind == 1 ? &prog->melodyEvents : kind == 2 ? &prog->bassEvents : &prog->drumEvents;
    const bool hasNotes = list != nullptr && ! list->empty();
    if (! has || ! hasNotes)
    {
        g.setColour (dimText);
        g.setFont (juce::Font (juce::FontOptions (12.f)));
        juce::String hintText = kind == 1 ? "Melody appears here" : kind == 2 ? "808 / bass line appears here"
                              : kind == 3 ? "LOAD drum MIDI or a loop from WXRST XTRACT (Documents/WXRST/Inbox)"
                              : "Hit GENERATE, then drag these boxes into FL's piano roll or playlist";
        if (kind == 3 && proc.getDrumLoopName().isNotEmpty()) hintText = "loop: " + proc.getDrumLoopName() + "  (plays in sync with PLAY)";
        g.drawText (hintText, r.toNearestInt(), juce::Justification::centred);
        return;
    }

    auto area = r.reduced (8.f, 5.f);
    auto hint = area.removeFromRight (118.f);
    g.setColour (ice);
    g.setFont (juce::Font (juce::FontOptions (11.f, juce::Font::bold)));
    g.drawText (kind == 1 ? "DRAG MELODY  >>" : kind == 2 ? "DRAG 808  >>" : kind == 3 ? "DRAG DRUMS  >>" : "DRAG CHORDS  >>", hint.toNearestInt(), juce::Justification::centredRight);

    const bool playing = proc.isChordPlaying();
    const double pos = proc.playPosition.load();

    if (kind == 0 && ! prog->chords.empty())
    {
        const int n = (int) prog->chords.size();
        const int cur = playing ? proc.currentChord.load() : -1;
        const float gap = 3.f;
        for (int i = 0; i < n; ++i)
        {
            const auto& c = prog->chords[(size_t) i];
            const float x = area.getX() + (float) (c.startBeat / prog->totalBeats) * area.getWidth();
            const float w = (float) (c.lengthBeats / prog->totalBeats) * area.getWidth() - gap;
            auto chip = juce::Rectangle<float> (x, area.getY() + 1.f, juce::jmax (10.f, w), area.getHeight() - 2.f);
            const bool active = i == cur && proc.layerEnabled[0].load();
            g.setColour (active ? iceDeep.withAlpha (0.7f) : knobBody);
            g.fillRoundedRectangle (chip, 4.f);
            g.setColour (active ? ice : panelEdge);
            g.drawRoundedRectangle (chip, 4.f, 1.f);
            g.setColour (active ? juce::Colours::white : (proc.layerEnabled[0].load() ? text : dimText));
            g.setFont (juce::Font (juce::FontOptions (w < 60.f ? 10.f : 12.f, juce::Font::bold)));
            g.drawText (c.name, chip.toNearestInt(), juce::Justification::centred);
        }
    }
    else
    {
        // mini piano roll
        const auto& evs = *list;
        int lo = 127, hi = 0;
        for (auto& e : evs) if (e.on) { lo = juce::jmin (lo, e.note); hi = juce::jmax (hi, e.note); }
        if (hi < lo) { lo = 72; hi = 84; }
        lo -= 1; hi += 1;
        const float rowH = area.getHeight() / (float) (hi - lo + 1);
        const bool melOn = proc.layerEnabled[kind].load();
        for (size_t i = 0; i < evs.size(); ++i)
        {
            const auto& e = evs[i];
            if (! e.on) continue;
            double end = e.beat + 0.5;
            for (size_t j = i + 1; j < evs.size(); ++j)
                if (! evs[j].on && evs[j].note == e.note) { end = evs[j].beat; break; }
            const float x = area.getX() + (float) (e.beat / prog->totalBeats) * area.getWidth();
            const float w = (float) ((end - e.beat) / prog->totalBeats) * area.getWidth() - 1.5f;
            const float y = area.getBottom() - (float) (e.note - lo + 1) * rowH;
            const bool lit = playing && pos >= e.beat && pos < end;
            g.setColour (! melOn ? dimText.withAlpha (0.5f) : (lit ? juce::Colours::white : ice));
            g.fillRoundedRectangle (x, y, juce::jmax (2.f, w), juce::jmax (2.f, rowH - 1.f), 1.5f);
        }
    }

    if (playing)
    {
        const float px = area.getX() + (float) (pos / prog->totalBeats) * area.getWidth();
        g.setColour (iceGlow.withAlpha (0.8f));
        g.drawLine (px, r.getY() + 2.f, px, r.getBottom() - 2.f, 1.5f);
    }
}

void WaveformView::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour (screen);
    g.fillRoundedRectangle (r, 6.f);
    auto s = proc.getSample();
    g.setColour (hover ? ice : (s != nullptr ? ice.withAlpha (0.5f) : panelEdge));
    g.drawRoundedRectangle (r.reduced (0.5f), 6.f, hover ? 2.f : 1.f);

    if (s == nullptr)
    {
        g.setColour (dimText);
        g.setFont (juce::Font (juce::FontOptions (12.f)));
        g.drawText (hover ? "Drop it" : "Drop a sample here (or hit LOAD)", getLocalBounds(), juce::Justification::centred);
        return;
    }

    auto area = r.reduced (4.f, 4.f);
    const float startF = proc.apvts.getRawParameterValue (ID::smpStart)->load();
    const float endF = proc.apvts.getRawParameterValue (ID::smpEnd)->load();
    const float a = juce::jmin (startF, endF), b = juce::jmax (startF, endF);
    // shade outside start/end
    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.fillRect (area.withWidth (area.getWidth() * a));
    g.fillRect (area.withLeft (area.getX() + area.getWidth() * b));

    // slices
    if (! s->slices.empty())
    {
        const bool big = getWidth() > 600;
        int idx = 0;
        for (auto& sl : s->slices)
        {
            const float x = area.getX() + area.getWidth() * (float) sl.first / (float) juce::jmax (1, s->length());
            g.setColour (iceGlow.withAlpha (0.35f));
            g.drawVerticalLine ((int) x, area.getY(), area.getBottom());
            if (big && idx < 64)
            {
                g.setColour (iceGlow.withAlpha (0.8f));
                g.setFont (juce::Font (juce::FontOptions (9.f, juce::Font::bold)));
                g.drawText (juce::String (idx + 1), (int) x + 3, (int) area.getBottom() - 14, 24, 12, juce::Justification::centredLeft);
            }
            ++idx;
        }
    }

    // waveform
    juce::Path pth;
    const float midY = area.getCentreY(), half = area.getHeight() * 0.48f;
    const int nb = (int) s->peaks.size();
    for (int i = 0; i < nb; ++i)
    {
        const float x = area.getX() + area.getWidth() * (float) i / (float) (nb - 1);
        const float v = juce::jlimit (0.f, 1.f, s->peaks[(size_t) i] * 1.2f);
        pth.addRectangle (x, midY - half * v, juce::jmax (1.f, area.getWidth() / (float) nb), juce::jmax (1.f, 2.f * half * v));
    }
    g.setColour (ice.withAlpha (0.85f));
    g.fillPath (pth);

    g.setColour (text);
    g.setFont (juce::Font (juce::FontOptions (11.f, juce::Font::bold)));
    juce::String info = s->name + "   " + juce::String (s->length() / s->sampleRate, 1) + "s";
    if (! s->slices.empty()) info += "   " + juce::String ((int) s->slices.size()) + " chops (C5 up)";
    g.drawText (info, getLocalBounds().reduced (8, 4), juce::Justification::topLeft);
}

void FileDragPad::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    const bool has = file.existsAsFile();
    g.setColour (screen);
    g.fillRoundedRectangle (r, 6.f);
    g.setColour (has ? ice.withAlpha (0.6f) : panelEdge);
    g.drawRoundedRectangle (r.reduced (0.5f), 6.f, 1.f);
    g.setColour (has ? ice : dimText);
    g.setFont (juce::Font (juce::FontOptions (11.f, juce::Font::bold)));
    g.drawText (has ? label : juce::String ("-"), getLocalBounds(), juce::Justification::centred);
}

// ---------------------------------------------------------------------------
juce::String SlicePads::keyName (int note)
{
    static const char* names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    return juce::String (names[(note % 12 + 12) % 12]) + juce::String (note / 12);   // FL: 60 = C5
}

int SlicePads::padAt (juce::Point<int> pt) const
{
    const int cw = getWidth() / cols, ch = getHeight() / rows;
    if (cw <= 0 || ch <= 0) return -1;
    const int c = pt.x / cw, r = pt.y / ch;
    if (c < 0 || c >= cols || r < 0 || r >= rows) return -1;
    return r * cols + c;
}

void SlicePads::mouseDown (const juce::MouseEvent& e)
{
    held = padAt (e.getPosition());
    if (held >= 0) proc.auditionNote (SamplePlayer::chopBaseNote + held, true);
}

void SlicePads::mouseUp (const juce::MouseEvent&)
{
    if (held >= 0) proc.auditionNote (SamplePlayer::chopBaseNote + held, false);
    held = -1;
}

void SlicePads::timerCallback()
{
    auto s = proc.getSample();
    const auto mask = proc.activeSlices();
    if (mask != lastMask || s.get() != lastSample) { lastMask = mask; lastSample = s.get(); repaint(); }
}

void SlicePads::paint (juce::Graphics& g)
{
    auto s = proc.getSample();
    const int nSlices = s ? (int) s->slices.size() : 0;
    const bool chop = proc.apvts.getRawParameterValue (ID::smpChop)->load() > 0.5f && nSlices > 0;
    const auto mask = proc.activeSlices();
    const float cw = (float) getWidth() / cols, ch = (float) getHeight() / rows;
    for (int i = 0; i < count; ++i)
    {
        auto r = juce::Rectangle<float> ((i % cols) * cw, (i / cols) * ch, cw, ch).reduced (3.f);
        const int note = SamplePlayer::chopBaseNote + i;
        const int slice = chop ? SamplePlayer::sliceForNote (note, nSlices) : -1;
        const bool lit = slice >= 0 && (mask & ((juce::uint64) 1 << slice)) != 0;
        const bool real = chop && i < nSlices;        // pads past the last slice wrap around: shown dimmer
        g.setColour (lit ? ice.withAlpha (0.55f) : screen);
        g.fillRoundedRectangle (r, 6.f);
        g.setColour (lit ? iceGlow : (real ? ice.withAlpha (0.6f) : panelEdge));
        g.drawRoundedRectangle (r.reduced (0.5f), 6.f, lit ? 2.f : 1.f);
        g.setColour (real || ! chop ? text : dimText);
        g.setFont (juce::Font (juce::FontOptions (chop ? 15.f : 13.f, juce::Font::bold)));
        if (chop) g.drawText (juce::String (slice + 1), r.reduced (6.f, 4.f), juce::Justification::topLeft);
        g.setFont (juce::Font (juce::FontOptions (10.f, juce::Font::bold)));
        g.setColour (dimText);
        g.drawText (keyName (note), r.reduced (6.f, 4.f), chop ? juce::Justification::bottomRight : juce::Justification::centred);
    }
}

void MidiDragPad::mouseDrag (const juce::MouseEvent& e)
{
    if (dragging || ! file.existsAsFile() || e.getDistanceFromDragStart() < 8) return;
    dragging = true;
    if (auto* c = juce::DragAndDropContainer::findParentDragContainerFor (this))
        c->performExternalDragDropOfFiles ({ file.getFullPathName() }, false, this, [this] { dragging = false; });
}

// ---------------------------------------------------------------------------
IcewaterEditor::IcewaterEditor (IcewaterProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&lnf);
    addAndMakeVisible (content);
    content.setBounds (0, 0, baseWidth, 1030);
    content.addAndMakeVisible (background);

    for (auto* pn : { &oscA, &oscB, &filter, &ampEnv, &filtEnv, &lfo, &pitchP, &global, &lofi, &chorusP, &delayP, &verbP, &chordsP })
    {
        content.addAndMakeVisible (pn);
        pn->setBufferedToImage (true);
    }
    animBtn.setClickingTogglesState (true);
    animBtn.setToggleState ((bool) proc.apvts.state.getProperty ("anim", true) && juce::SystemStats::getEnvironmentVariable ("ICEWATER_ANIM", "1") != "0", juce::dontSendNotification);
    animBtn.setTooltip ("Toggle animated background");
    animBtn.onClick = [this]
    {
        background.setAnimating (animBtn.getToggleState());
        proc.apvts.state.setProperty ("anim", animBtn.getToggleState(), nullptr);
    };
    background.setAnimating (animBtn.getToggleState());
    content.addAndMakeVisible (animBtn);
    content.addAndMakeVisible (scope);
    ampEnv.addAndMakeVisible (ampEnvDisp);
    filtEnv.addAndMakeVisible (filtEnvDisp);

    setupCombo (oscAWave, waveNames, ID::oscAWave, oscAWaveAtt, oscA);
    setupCombo (oscBWave, waveNames, ID::oscBWave, oscBWaveAtt, oscB);
    setupCombo (lfoShape, lfoShapeNames, ID::lfoShape, lfoShapeAtt, lfo);
    setupCombo (filtType, filterTypeNames, ID::filtType, filtTypeAtt, filter);
    setupCombo (voiceMode, voiceModeNames, ID::voiceMode, voiceModeAtt, pitchP);

    oscA.addAndMakeVisible (oscAOn); oscB.addAndMakeVisible (oscBOn);
    oscAOnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, ID::oscAOn, oscAOn);
    oscBOnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, ID::oscBOn, oscBOn);

    for (auto* o : { &oscA, &oscB })
    {
        const bool A = o == &oscA;
        addKnob (*o, A ? ID::oscAOct : ID::oscBOct, "OCT");
        addKnob (*o, A ? ID::oscASemi : ID::oscBSemi, "SEMI");
        addKnob (*o, A ? ID::oscAFine : ID::oscBFine, "FINE");
        addKnob (*o, A ? ID::oscAUnison : ID::oscBUnison, "UNISON");
        addKnob (*o, A ? ID::oscADetune : ID::oscBDetune, "DETUNE");
        addKnob (*o, A ? ID::oscALevel : ID::oscBLevel, "LEVEL");
    }
    addKnob (filter, ID::filtCutoff, "CUTOFF"); addKnob (filter, ID::filtRes, "RES"); addKnob (filter, ID::filtEnv, "ENV");
    addKnob (filter, ID::filtKey, "KEYTRK"); addKnob (filter, ID::filtDrive, "DRIVE");
    addKnob (ampEnv, ID::ampA, "A"); addKnob (ampEnv, ID::ampD, "D"); addKnob (ampEnv, ID::ampS, "S"); addKnob (ampEnv, ID::ampR, "R");
    addKnob (filtEnv, ID::fA, "A"); addKnob (filtEnv, ID::fD, "D"); addKnob (filtEnv, ID::fS, "S"); addKnob (filtEnv, ID::fR, "R");
    addKnob (lfo, ID::lfoRate, "RATE"); addKnob (lfo, ID::lfoPitch, "> PITCH"); addKnob (lfo, ID::lfoFilter, "> FILTER");
    addKnob (pitchP, ID::pitchEnvAmt, "P.ENV"); addKnob (pitchP, ID::pitchEnvDec, "DECAY"); addKnob (pitchP, ID::glide, "GLIDE");
    addKnob (global, ID::subLevel, "SUB"); addKnob (global, ID::noiseLevel, "NOISE"); addKnob (global, ID::drift, "DRIFT");
    addKnob (global, ID::width, "WIDTH"); addKnob (global, ID::master, "MASTER");
    addKnob (lofi, ID::crushBits, "BITS"); addKnob (lofi, ID::crushRate, "RATE"); addKnob (lofi, ID::lofiTone, "TONE");
    addKnob (lofi, ID::wow, "WOW"); addKnob (lofi, ID::wowRate, "WOW RATE"); addKnob (lofi, ID::heat, "HEAT"); addKnob (lofi, ID::dist, "DIST"); addKnob (lofi, ID::crackle, "CRACKLE");
    addKnob (chorusP, ID::chorusMix, "MIX"); addKnob (chorusP, ID::chorusRate, "RATE"); addKnob (chorusP, ID::chorusDepth, "DEPTH");
    addKnob (delayP, ID::delayMix, "MIX"); addKnob (delayP, ID::delayTime, "TIME"); addKnob (delayP, ID::delayFb, "FDBK");
    addKnob (verbP, ID::verbMix, "MIX"); addKnob (verbP, ID::verbSize, "SIZE"); addKnob (verbP, ID::verbDamp, "DAMP");

    // Presets (category -> filtered list)
    categoryBox.addItemList (presetCategories, 1);
    categoryBox.setSelectedId (1, juce::dontSendNotification);
    categoryBox.onChange = [this]
    {
        rebuildPresetList();
        const int curId = proc.currentUserPreset.load() >= 0 ? kUserIdBase + proc.currentUserPreset.load() : proc.getCurrentProgram() + 1;
        if (! visiblePresets.empty() && std::find (visiblePresets.begin(), visiblePresets.end(), curId) == visiblePresets.end())
            loadPresetById (visiblePresets.front());
        lastProgram = -1; lastUserPreset = -2;
    };
    rebuildPresetList();
    presetBox.onChange = [this] { if (presetBox.getSelectedId() > 0) loadPresetById (presetBox.getSelectedId()); };
    auto step = [this] (int dir)
    {
        if (visiblePresets.empty()) return;
        const int cur = proc.currentUserPreset.load() >= 0 ? kUserIdBase + proc.currentUserPreset.load() : proc.getCurrentProgram() + 1;
        int pos = 0;
        for (size_t k = 0; k < visiblePresets.size(); ++k) if (visiblePresets[k] == cur) pos = (int) k;
        pos = (pos + dir + (int) visiblePresets.size()) % (int) visiblePresets.size();
        loadPresetById (visiblePresets[(size_t) pos]);
    };
    saveBtn.setTooltip ("Save the current sound as a user preset");
    saveBtn.onClick = [this] { saveUserPresetDialog(); };
    content.addAndMakeVisible (saveBtn);
    prevBtn.onClick = [step] { step (-1); };
    nextBtn.onClick = [step] { step (1); };
    diceBtn.onClick = [this] { proc.randomiseSound(); };
    diceBtn.setTooltip ("Roll the dice: randomise the sound");
    content.addAndMakeVisible (categoryBox); content.addAndMakeVisible (presetBox); content.addAndMakeVisible (prevBtn); content.addAndMakeVisible (nextBtn); content.addAndMakeVisible (diceBtn);

    // MIDNIGHT MODE: the skin follows the sound. Picking a Synthwave / 16-Bit preset flips to the 80s look,
    // picking anything else flips back to ICE. The tab is a manual override until the next preset change.
    midnightTab.setTooltip ("Midnight Mode: 80s synthwave look (follows the preset; click to override)");
    midnightTab.onClick = [this]
    {
        const bool on = midnightTab.getToggleState();
        applyTheme (on ? 1 : 0);
        if (on)
        {
            const int idx = presetCategories.indexOf ("Synthwave");
            if (idx >= 0 && categoryBox.getSelectedId() != idx + 1) categoryBox.setSelectedId (idx + 1, juce::sendNotificationSync);
        }
    };
    content.addAndMakeVisible (midnightTab);
    const auto envTheme = juce::SystemStats::getEnvironmentVariable ("ICEWATER_THEME", "");
    if (envTheme.isNotEmpty()) { midnightTab.setToggleState (envTheme.getIntValue() == 1, juce::dontSendNotification); applyTheme (envTheme.getIntValue()); }
    else                       { applyTheme (0); syncThemeToPreset (proc.getCurrentProgram()); }

    // Sample section
    content.addAndMakeVisible (sampleP);
    sampleP.setBufferedToImage (false); // waveform changes, don't cache the panel
    sampleP.addAndMakeVisible (waveform);
    for (auto* b : { &loadSampleBtn, &toMidiBtn, &chopBtn }) sampleP.addAndMakeVisible (b);
    for (auto* t : { &smpLoopT, &smpRevT, &smpChopT, &smpHalfT }) sampleP.addAndMakeVisible (t);
    smpHalfT.setTooltip ("Tape-style halftime: plays the sample at half speed, one octave down");
    smpHalfAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, ID::smpHalf, smpHalfT);
    smpLoopAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, ID::smpLoop, smpLoopT);
    smpRevAtt  = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, ID::smpRev, smpRevT);
    smpChopAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, ID::smpChop, smpChopT);
    addKnob (sampleP, ID::smpLevel, "LEVEL"); addKnob (sampleP, ID::smpRoot, "ROOT"); addKnob (sampleP, ID::smpTune, "TUNE");
    addKnob (sampleP, ID::smpStart, "START"); addKnob (sampleP, ID::smpEnd, "END");
    sampleP.addAndMakeVisible (notesPad); sampleP.addAndMakeVisible (chopsPad);
    loadSampleBtn.onClick = [this] { loadSampleDialog(); };
    waveform.onDropped = [this] { setPage (1); };
    toMidiBtn.setTooltip ("Pitch-track the sample into a MIDI clip (melodies, bass, vocals)");
    toMidiBtn.onClick = [this] { setSampleFiles (proc.extractNotesMidi(), {}); };
    chopBtn.setTooltip ("Slice the sample at transients: each slice goes on its own key / pad from C5 up, WAVs land in Documents/WXRST/Chops");
    chopBtn.onClick = [this] { juce::File dir; setSampleFiles ({}, proc.chopSample (dir)); setPage (1); };
    sampleFold.setTooltip ("Collapse / expand");
    sampleP.addAndMakeVisible (sampleFold);
    sampleFold.onClick = [this] { sampleOpen = ! sampleOpen; proc.apvts.state.setProperty ("uiSampleOpen", sampleOpen, nullptr); applyWindowSize(); };

    // ---- SAMPLER page ----
    content.addAndMakeVisible (samplerP);
    samplerP.setBufferedToImage (false);
    samplerP.addAndMakeVisible (bigWave);
    for (auto* b : { &sLoadBtn, &sMidiBtn, &sChopBtn }) samplerP.addAndMakeVisible (b);
    for (auto* t : { &sLoopT, &sRevT, &sChopT, &sHalfT, &sOnlyT }) samplerP.addAndMakeVisible (t);
    samplerP.addAndMakeVisible (sNotesPad); samplerP.addAndMakeVisible (sChopsPad);
    samplerP.addAndMakeVisible (pads);
    samplerP.addAndMakeVisible (padHint);
    padHint.setFont (juce::Font (juce::FontOptions (10.f)));
    padHint.setColour (juce::Label::textColourId, dimText);
    padHint.setText ("PADS / KEYS: pad 1 = C5 (FL's middle C), going up. Before CHOP IT the pads play the sample pitched. Click a pad to hear it; DRAG CHOPS drops the slice order into FL as MIDI.", juce::dontSendNotification);
    sLoopAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, ID::smpLoop, sLoopT);
    sRevAtt  = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, ID::smpRev, sRevT);
    sChopAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, ID::smpChop, sChopT);
    sHalfAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, ID::smpHalf, sHalfT);
    sOnlyAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, ID::smpOnly, sOnlyT);
    sHalfT.setTooltip ("Tape-style halftime: plays the sample at half speed, one octave down");
    sOnlyT.setTooltip ("On: the keys play only the sample while one is loaded. Off: sample layered on top of the synth preset.");
    for (auto [id, text] : std::initializer_list<std::pair<const char*, const char*>> {
             { ID::smpLevel, "LEVEL" }, { ID::smpRoot, "ROOT" }, { ID::smpTune, "TUNE" }, { ID::smpStart, "START" }, { ID::smpEnd, "END" },
             { ID::smpA, "ATTACK" }, { ID::smpD, "DECAY" }, { ID::smpS, "SUSTAIN" }, { ID::smpR, "RELEASE" }, { ID::smpCut, "CUTOFF" }, { ID::smpRes, "RES" } })
    {
        auto* k = knobs.add (new Knob (proc.apvts, id, text));
        samplerP.addAndMakeVisible (k);
        samplerKnobs.push_back (k);
    }
    sLoadBtn.onClick = [this] { loadSampleDialog(); };
    sMidiBtn.setTooltip ("Pitch-track the sample into a MIDI clip (melodies, bass, vocals)");
    sMidiBtn.onClick = [this] { setSampleFiles (proc.extractNotesMidi(), {}); };
    sChopBtn.setTooltip ("Slice the sample at transients: one slice per pad / key from C5 up, WAVs land in Documents/WXRST/Chops");
    sChopBtn.onClick = [this] { juce::File dir; setSampleFiles ({}, proc.chopSample (dir)); };

    // ---- update banner + version ----
    content.addAndMakeVisible (updateBanner);
    content.addAndMakeVisible (versionLbl);
    versionLbl.setText ("v" DENNY_VERSION, juce::dontSendNotification);
    versionLbl.setJustificationType (juce::Justification::centredRight);
    versionLbl.setFont (juce::Font (juce::FontOptions (9.f)));
    versionLbl.setTooltip ("Click to check for updates");
    versionLbl.setInterceptsMouseClicks (true, false);
    versionLbl.addMouseListener (this, false);

    // ---- pages / collapsing ----
    samplerTab.setTooltip ("SAMPLER page: big waveform, pads, the sampler's own envelope + filter");
    samplerTab.onClick = [this] { setPage (samplerTab.getToggleState() ? 1 : 0); };
    content.addAndMakeVisible (samplerTab);
    chordsFold.setTooltip ("Collapse / expand");
    chordsP.addAndMakeVisible (chordsFold);
    chordsFold.onClick = [this] { chordsOpen = ! chordsOpen; proc.apvts.state.setProperty ("uiChordsOpen", chordsOpen, nullptr); applyWindowSize(); };
    chordsOpen = (bool) proc.apvts.state.getProperty ("uiChordsOpen", true);
    sampleOpen = (bool) proc.apvts.state.getProperty ("uiSampleOpen", false);
    uiScale = juce::jlimit (0.5f, 2.f, (float) (double) proc.apvts.state.getProperty ("uiScale", 1.0));
    const auto envPage = juce::SystemStats::getEnvironmentVariable ("WXRST_PAGE", "");
    page = envPage.isNotEmpty() ? envPage.getIntValue() : (int) proc.apvts.state.getProperty ("uiPage", 0);
    samplerTab.setToggleState (page == 1, juce::dontSendNotification);
    for (auto* k : { &oscAWave }) juce::ignoreUnused (k);

    // Chord generator
    auto setupChordCombo = [&] (juce::ComboBox& box, juce::Label& lbl, const juce::StringArray& items, int def)
    {
        box.addItemList (items, 1);
        box.setSelectedId (def + 1, juce::dontSendNotification);
        chordsP.addAndMakeVisible (box);
        lbl.setFont (juce::Font (juce::FontOptions (10.f, juce::Font::bold)));
        lbl.setColour (juce::Label::textColourId, dimText);
        lbl.setJustificationType (juce::Justification::centredLeft);
        chordsP.addAndMakeVisible (lbl);
    };
    setupChordCombo (keyBox, keyLbl, ChordGen::keyNames, 9);
    setupChordCombo (scaleBox, scaleLbl, ChordGen::scaleNames, 0);
    setupChordCombo (styleBox, styleLbl, ChordGen::styleNames, 0);
    setupChordCombo (barsBox, barsLbl, ChordGen::barNames, 0);
    setupChordCombo (rhythmBox, rhythmLbl, ChordGen::rhythmNames, 0);
    for (auto* b : { &generateBtn, &playBtn, &strumBtn }) chordsP.addAndMakeVisible (b);
    chordsP.addAndMakeVisible (chordPad);
    chordsP.addAndMakeVisible (melodyPad);
    chordsP.addAndMakeVisible (bassPad);
    chordsP.addAndMakeVisible (drumPad);
    for (int l = 0; l < 4; ++l)
    {
        auto& b = layerBtn[l];
        chordsP.addAndMakeVisible (b);
        b.setClickingTogglesState (true);
        b.setToggleState (proc.layerEnabled[l].load(), juce::dontSendNotification);
        b.setTooltip ("Include this layer when playing");
        b.onClick = [this, l] { proc.layerEnabled[l].store (layerBtn[l].getToggleState()); };
        chordsP.addAndMakeVisible (layerLoad[l]);
        layerLoad[l].setTooltip ("Load MIDI from WXRST XTRACT's inbox (Documents/WXRST/Inbox) or any .mid file into this row");
        layerLoad[l].onClick = [this, l] { showLoadMenu (l); };
    }
    drumSoundLbl.setFont (juce::Font (juce::FontOptions (10.f)));
    drumSoundLbl.setColour (juce::Label::textColourId, dimText);
    drumSoundLbl.setJustificationType (juce::Justification::centred);
    chordsP.addAndMakeVisible (drumSoundLbl);
    for (int k = 0; k < 2; ++k)
    {
        auto& box = layerSound[k];
        int i = 1;
        for (auto& pr : getPresets()) box.addItem (juce::String (pr.category) + "  -  " + pr.name, i++);
        box.setSelectedId (proc.getLayerPreset (k == 0 ? IcewaterProcessor::melodyLayer : IcewaterProcessor::bassLayer) + 1, juce::dontSendNotification);
        box.onChange = [this, k] { proc.setLayerPreset (k == 0 ? IcewaterProcessor::melodyLayer : IcewaterProcessor::bassLayer, layerSound[k].getSelectedId() - 1); };
        chordsP.addAndMakeVisible (box);
    }
    chordSoundLbl.setFont (juce::Font (juce::FontOptions (10.f)));
    chordSoundLbl.setColour (juce::Label::textColourId, dimText);
    chordSoundLbl.setJustificationType (juce::Justification::centred);
    chordsP.addAndMakeVisible (chordSoundLbl);

    strumBtn.setClickingTogglesState (true);
    generateBtn.onClick = [this] { generateChords(); };
    playBtn.setClickingTogglesState (true);
    playBtn.onClick = [this]
    {
        if (proc.getProgression() == nullptr) generateChords();
        proc.setChordPlaying (playBtn.getToggleState());
        playBtn.setButtonText (playBtn.getToggleState() ? "STOP" : "PLAY");
    };
    if (auto prog = proc.getProgression())
    {
        chordPad.setFile (ChordGen::writeMidiFile (prog->events, prog->title + " chords (" + ChordGen::chordListString (*prog) + ")", proc.getHostBpm()));
        melodyPad.setFile (ChordGen::writeMidiFile (prog->melodyEvents, prog->title + " melody (" + ChordGen::chordListString (*prog) + ")", proc.getHostBpm()));
        bassPad.setFile (ChordGen::writeMidiFile (prog->bassEvents, prog->title + " 808 (" + ChordGen::chordListString (*prog) + ")", proc.getHostBpm()));
    }
    playBtn.setToggleState (proc.isChordPlaying(), juce::dontSendNotification);
    playBtn.setButtonText (proc.isChordPlaying() ? "STOP" : "PLAY");
    refreshLayerPads();

    // WXRST_AUTOLOAD=1 (testing): pull the newest file from each inbox folder into its row
    if (juce::SystemStats::getEnvironmentVariable ("WXRST_AUTOLOAD", {}).isNotEmpty())
    {
        auto newest = [] (int which, const juce::String& pattern) -> juce::File
        {
            juce::Array<juce::File> files;
            IcewaterProcessor::inboxDir (which).findChildFiles (files, juce::File::findFiles, false, pattern);
            juce::File best;
            for (auto& f : files) if (best == juce::File() || f.getLastModificationTime() > best.getLastModificationTime()) best = f;
            return best;
        };
        if (auto f = newest (0, "*.mid"); f.existsAsFile()) { proc.loadLayerMidi (IcewaterProcessor::chordsLayer, f); chordPad.setFile (f); }
        if (auto f = newest (1, "*.mid"); f.existsAsFile()) { proc.loadLayerMidi (IcewaterProcessor::melodyLayer, f); melodyPad.setFile (f); }
        if (auto f = newest (2, "*.mid"); f.existsAsFile()) { proc.loadLayerMidi (IcewaterProcessor::drumsLayer, f); drumPad.setFile (f); }
        if (auto f = newest (2, "*.wav"); f.existsAsFile()) proc.loadDrumLoop (f);
        refreshLayerPads();
    }

    startTimerHz (10);
    timerCallback();
    setResizable (true, true);
    setPage (page);          // sets visibility + window size (setSize must stay at the end: knobs need to exist)
}

void IcewaterEditor::generateChords()
{
    chordSeed = chordSeed * 1664525u + 1013904223u;
    ChordGen::Options o;
    o.key = keyBox.getSelectedId() - 1; o.scale = scaleBox.getSelectedId() - 1; o.style = styleBox.getSelectedId() - 1;
    o.bars = barsBox.getSelectedId() == 2 ? 8 : 4; o.rhythm = rhythmBox.getSelectedId() - 1;
    o.strum = strumBtn.getToggleState(); o.seed = chordSeed;

    auto prog = std::make_shared<Progression> (ChordGen::generate (o));
    if (auto old = proc.getProgression()) prog->drumEvents = old->drumEvents;
    const auto names = ChordGen::chordListString (*prog);
    chordPad.setFile (ChordGen::writeMidiFile (prog->events, prog->title + " chords (" + names + ")", proc.getHostBpm()));
    melodyPad.setFile (ChordGen::writeMidiFile (prog->melodyEvents, prog->title + " melody (" + names + ")", proc.getHostBpm()));
    bassPad.setFile (ChordGen::writeMidiFile (prog->bassEvents, prog->title + " 808 (" + names + ")", proc.getHostBpm()));
    proc.setProgression (prog);
    for (int l = 0; l < 3; ++l) proc.clearLayerFileOnly ((IcewaterProcessor::Layer) l);
}

void IcewaterEditor::refreshLayerPads()
{
    MidiDragPad* pads[4] { &chordPad, &melodyPad, &bassPad, &drumPad };
    for (int l = 0; l < 4; ++l)
    {
        auto f = proc.getLayerFile ((IcewaterProcessor::Layer) l);
        if (f.existsAsFile()) pads[l]->setFile (f);
    }
    auto loop = proc.getDrumLoopName();
    drumSoundLbl.setText (loop.isNotEmpty() ? "kit + loop: " + loop : "built-in kit", juce::dontSendNotification);
}

void IcewaterEditor::showLoadMenu (int layer)
{
    const bool drums = layer == 3;
    juce::PopupMenu menu;
    menuFiles.clear();
    static const char* folderNames[] = { "Chords", "Melody", "Drums" };
    for (int which = 0; which < 3; ++which)
    {
        auto dir = IcewaterProcessor::inboxDir (which);
        juce::Array<juce::File> files;
        dir.findChildFiles (files, juce::File::findFiles, false, drums ? "*.mid;*.wav;*.aif;*.aiff" : "*.mid");
        std::sort (files.begin(), files.end(), [] (const juce::File& a, const juce::File& b) { return a.getLastModificationTime() > b.getLastModificationTime(); });
        juce::PopupMenu sub;
        int shown = 0;
        for (auto& f : files)
        {
            if (++shown > 25) break;
            menuFiles.push_back (f);
            const bool isMidi = f.hasFileExtension ("mid");
            sub.addItem (1000 + (int) menuFiles.size() - 1, (isMidi ? "MIDI   " : "LOOP   ") + f.getFileNameWithoutExtension());
        }
        if (files.isEmpty()) sub.addItem (-1, "(empty - send something here from WXRST XTRACT)", false);
        menu.addSubMenu (juce::String ("XTRACT inbox / ") + folderNames[which], sub);
    }
    menu.addSeparator();
    menu.addItem (1, drums ? "Browse for a MIDI file or loop..." : "Browse for a MIDI file...");
    menu.addItem (2, drums ? "Clear drums (notes + loop)" : "Clear this row");

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (layerLoad[layer]), [this, layer, drums] (int id)
    {
        auto load = [this, layer, drums] (const juce::File& f)
        {
            if (f.hasFileExtension ("mid"))
            {
                if (! proc.loadLayerMidi ((IcewaterProcessor::Layer) layer, f)) return;
                MidiDragPad* pads[4] { &chordPad, &melodyPad, &bassPad, &drumPad };
                pads[layer]->setFile (f);
            }
            else if (drums) proc.loadDrumLoop (f);
            refreshLayerPads();
        };
        if (id >= 1000 && id - 1000 < (int) menuFiles.size()) load (menuFiles[(size_t) id - 1000]);
        else if (id == 1)
        {
            chooser = std::make_unique<juce::FileChooser> ("Load into " + juce::String (layerBtn[layer].getButtonText()),
                                                           IcewaterProcessor::inboxDir (drums ? 2 : layer == 1 ? 1 : 0),
                                                           drums ? "*.mid;*.wav;*.aif;*.aiff" : "*.mid");
            chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                                  [load] (const juce::FileChooser& fc) { if (fc.getResult().existsAsFile()) load (fc.getResult()); });
        }
        else if (id == 2)
        {
            proc.clearLayer ((IcewaterProcessor::Layer) layer);
            if (drums) proc.clearDrumLoop();
            MidiDragPad* pads[4] { &chordPad, &melodyPad, &bassPad, &drumPad };
            pads[layer]->setFile (juce::File());
            refreshLayerPads();
        }
    });
}

IcewaterEditor::~IcewaterEditor()
{
    setLookAndFeel (nullptr);
    IceColours::applyTheme (0); // the LookAndFeel colours are global: leave them in the default state
}

void IcewaterEditor::syncThemeToPreset (int program)
{
    const auto& presets = getPresets();
    if (program < 0 || program >= (int) presets.size()) return;
    const juce::String cat = presets[(size_t) program].category;
    const int want = (cat == "Synthwave" || cat == "16-Bit") ? 1 : 0;
    if (want != IceColours::theme)
    {
        midnightTab.setToggleState (want == 1, juce::dontSendNotification);
        applyTheme (want);
    }
}

void IcewaterEditor::applyTheme (int which)
{
    IceColours::applyTheme (which);
    updateBanner.setColours (IceColours::screen, IceColours::ice, IceColours::text);
    versionLbl.setColour (juce::Label::textColourId, IceColours::dimText);
    lnf.refreshColours();
    for (auto* k : knobs) k->label.setColour (juce::Label::textColourId, IceColours::dimText);
    for (auto* l : { &keyLbl, &scaleLbl, &styleLbl, &barsLbl, &rhythmLbl, &chordSoundLbl })
        l->setColour (juce::Label::textColourId, IceColours::dimText);
    sendLookAndFeelChange();
    background.themeChanged();
    repaint();
}

void IcewaterEditor::setupCombo (juce::ComboBox& box, const juce::StringArray& items, const char* id,
                                 std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>& att, Panel& parent)
{
    box.addItemList (items, 1);
    parent.addAndMakeVisible (box);
    att = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (proc.apvts, id, box);
}

Knob& IcewaterEditor::addKnob (Panel& panel, const char* id, const juce::String& text)
{
    auto* k = knobs.add (new Knob (proc.apvts, id, text));
    panel.addAndMakeVisible (k);
    knobById[id] = k;
    return *k;
}

void IcewaterEditor::loadPresetById (int itemId)
{
    if (itemId >= kUserIdBase) proc.loadUserPreset (itemId - kUserIdBase);
    else if (itemId > 0) proc.loadPreset (itemId - 1);
}

void IcewaterEditor::saveUserPresetDialog()
{
    const int up = proc.currentUserPreset.load();
    const juce::String suggested = up >= 0 ? proc.userPresets[(size_t) up].name : juce::String (getPresets()[(size_t) proc.getCurrentProgram()].name) + " 2";
    auto* w = new juce::AlertWindow ("Save preset", "Name your sound. It goes in Documents/WXRST/Presets and shows up under the User category.",
                                     juce::MessageBoxIconType::NoIcon);
    w->addTextEditor ("name", suggested, "Name");
    w->addButton ("Save", 1, juce::KeyPress (juce::KeyPress::returnKey));
    w->addButton ("Cancel", 0, juce::KeyPress (juce::KeyPress::escapeKey));
    w->setLookAndFeel (&lnf);
    w->enterModalState (true, juce::ModalCallbackFunction::create ([this, w] (int result)
    {
        const juce::String name = w->getTextEditorContents ("name").trim();
        w->setLookAndFeel (nullptr);
        delete w;
        if (result != 1 || name.isEmpty()) return;
        proc.saveUserPreset (name);
        const int userIdx = presetCategories.indexOf ("User");
        if (userIdx >= 0) categoryBox.setSelectedId (userIdx + 1, juce::dontSendNotification);
        rebuildPresetList();
        lastProgram = -1; lastUserPreset = -2;
    }), false);
}

void IcewaterEditor::rebuildPresetList()
{
    const auto cat = presetCategories[categoryBox.getSelectedId() - 1];
    presetBox.clear (juce::dontSendNotification);
    visiblePresets.clear();
    const auto& presets = getPresets();
    for (int i = 0; i < (int) presets.size(); ++i)
        if (cat == "All" || cat == presets[(size_t) i].category)
        {
            presetBox.addItem (presets[(size_t) i].name, i + 1);
            visiblePresets.push_back (i + 1);
        }
    if (cat == "All" || cat == "User")
    {
        if (cat == "All" && ! proc.userPresets.empty()) presetBox.addSeparator();
        for (int i = 0; i < (int) proc.userPresets.size(); ++i)
        {
            presetBox.addItem (proc.userPresets[(size_t) i].name, kUserIdBase + i);
            visiblePresets.push_back (kUserIdBase + i);
        }
        if (cat == "User" && proc.userPresets.empty()) presetBox.setTextWhenNothingSelected ("No user presets yet - hit SAVE");
    }
    if (cat == "Drift") presetBox.setTextWhenNothingSelected ("Drift - coming soon");
    const int up = proc.currentUserPreset.load();
    presetBox.setSelectedId (up >= 0 ? kUserIdBase + up : proc.getCurrentProgram() + 1, juce::dontSendNotification);
}

void IcewaterEditor::timerCallback()
{
    // WXRST_SNAPSHOT=<png>: render the editor to a file (UI check without a screen)
    if (auto snap = juce::SystemStats::getEnvironmentVariable ("WXRST_SNAPSHOT", {}); snap.isNotEmpty() && ++snapshotTicks == 20)
    {
        auto img = createComponentSnapshot (getLocalBounds(), false, 2.f);
        juce::File f (snap); f.deleteFile();
        juce::FileOutputStream os (f);
        juce::PNGImageFormat().writeImageToStream (img, os);
    }

    if (juce::SystemStats::getEnvironmentVariable ("WXRST_AUTOCHOP", "") == "1" && snapshotTicks == 10)
        if (auto smp = proc.getSample(); smp != nullptr && smp->slices.empty()) sChopBtn.onClick();

    const int prog = proc.getCurrentProgram();
    const int up = proc.currentUserPreset.load();
    if (prog != lastProgram || up != lastUserPreset)
    {
        lastProgram = prog; lastUserPreset = up;
        if (up < 0) syncThemeToPreset (prog);   // user presets keep whatever look is on
        const int id = up >= 0 ? kUserIdBase + up : prog + 1;
        if (presetBox.indexOfItemId (id) < 0)
        {
            // current preset isn't in the chosen category: switch category to "All" so it shows
            // (unless the category is deliberately empty, e.g. Drift)
            if (visiblePresets.empty()) { presetBox.setSelectedId (0, juce::dontSendNotification); return; }
            categoryBox.setSelectedId (1, juce::dontSendNotification);
            rebuildPresetList();
            return;
        }
        presetBox.setSelectedId (id, juce::dontSendNotification);
    }
}

void IcewaterEditor::paint (juce::Graphics& g)
{
    if (! background.isAnimating()) g.fillAll (bg);
}

void IcewaterEditor::layoutKnobs (juce::Rectangle<int> area, std::initializer_list<Knob*> ks)
{
    const int n = (int) ks.size();
    if (n == 0) return;
    const int cellW = area.getWidth() / n;
    const int size = juce::jmin (cellW, area.getHeight() - 14);
    int x = area.getX();
    for (auto* k : ks)
    {
        auto cell = juce::Rectangle<int> (x, area.getY(), cellW, area.getHeight());
        k->setBounds (cell.withSizeKeepingCentre (size, size + 14));
        x += cellW;
    }
}

int IcewaterEditor::baseHeight() const
{
    const int rowsBottom = 632;                                   // synth rows 1-3 (page 0) or the sampler panel (page 1) end here + sample row
    int y = rowsBottom + 10;
    if (page == 0) y += (sampleOpen ? 128 : 28) + 8;
    else y += 128 + 8;                                            // sampler panel is taller by exactly the open sample row
    y += (chordsOpen ? 242 : 28);
    return y + 10;
}

void IcewaterEditor::applyWindowSize()
{
    const int h = baseHeight();
    if (auto* c = getConstrainer())
    {
        c->setFixedAspectRatio ((double) baseWidth / (double) h);
        c->setSizeLimits (baseWidth / 2, h / 2, baseWidth * 2, h * 2);
    }
    setSize (juce::roundToInt (baseWidth * uiScale), juce::roundToInt (h * uiScale));
    layoutContent();
}

void IcewaterEditor::setPage (int p)
{
    page = juce::jlimit (0, 1, p);
    proc.apvts.state.setProperty ("uiPage", page, nullptr);
    samplerTab.setToggleState (page == 1, juce::dontSendNotification);
    for (auto* pn : { &oscA, &oscB, &filter, &ampEnv, &filtEnv, &lfo, &pitchP, &global, &lofi, &chorusP, &delayP, &verbP, &sampleP })
        pn->setVisible (page == 0);
    samplerP.setVisible (page == 1);
    applyWindowSize();
}

void IcewaterEditor::setPanelOpen (Panel& panel, juce::TextButton& fold, bool open)
{
    for (auto* c : panel.getChildren()) if (c != &fold) c->setVisible (open);
    fold.setButtonText (open ? "-" : "+");
    fold.setBounds (panel.getWidth() - 30, 4, 22, 20);
}

void IcewaterEditor::mouseDown (const juce::MouseEvent& e)
{
    if (e.eventComponent == &versionLbl) proc.updater.checkNow (true);
}

void IcewaterEditor::loadSampleDialog()
{
    chooser = std::make_unique<juce::FileChooser> ("Load a sample", juce::File::getSpecialLocation (juce::File::userHomeDirectory),
                                                   "*.wav;*.aif;*.aiff;*.mp3;*.flac;*.ogg;*.m4a");
    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                          [this] (const juce::FileChooser& fc) { if (fc.getResult().existsAsFile() && proc.loadSampleFile (fc.getResult())) setPage (1); });
}

void IcewaterEditor::setSampleFiles (const juce::File& notes, const juce::File& chops)
{
    if (notes != juce::File()) { notesPad.setFile (notes); sNotesPad.setFile (notes); }
    if (chops != juce::File()) { chopsPad.setFile (chops); sChopsPad.setFile (chops); }
}

void IcewaterEditor::resized()
{
    uiScale = (float) getWidth() / (float) baseWidth;
    proc.apvts.state.setProperty ("uiScale", (double) uiScale, nullptr);
    content.setTransform (juce::AffineTransform::scale (uiScale));
    layoutContent();
}

void IcewaterEditor::layoutContent()
{
    auto K = [&] (const char* id) { return knobById[id]; };
    content.setBounds (0, 0, baseWidth, baseHeight());

    // Header
    background.setBounds (content.getLocalBounds());
    animBtn.setBounds (216, 24, 24, 24);
    categoryBox.setBounds (250, 22, 86, 28);
    prevBtn.setBounds (342, 22, 26, 28);
    presetBox.setBounds (372, 22, 160, 28);
    nextBtn.setBounds (536, 22, 26, 28);
    diceBtn.setBounds (570, 20, 32, 32);
    midnightTab.setBounds (610, 22, 84, 28);
    samplerTab.setBounds (700, 22, 80, 28);
    saveBtn.setBounds (788, 22, 52, 28);
    scope.setBounds (850, 12, 114, 52);
    versionLbl.setBounds (850, 64, 114, 14);
    updateBanner.setBounds (330, 56, 506, 22);

    // Row 1
    oscA.setBounds (16, 82, 300, 215);
    oscB.setBounds (326, 82, 300, 215);
    filter.setBounds (636, 82, 328, 215);

    for (auto* o : { &oscA, &oscB })
    {
        const bool A = o == &oscA;
        auto r = o->getLocalBounds().reduced (10).withTrimmedTop (24);
        auto top = r.removeFromTop (26);
        (A ? oscAOn : oscBOn).setBounds (top.removeFromLeft (60));
        (A ? oscAWave : oscBWave).setBounds (top.reduced (4, 0));
        auto row1 = r.removeFromTop (r.getHeight() / 2);
        layoutKnobs (row1, { K (A ? ID::oscAOct : ID::oscBOct), K (A ? ID::oscASemi : ID::oscBSemi), K (A ? ID::oscAFine : ID::oscBFine) });
        layoutKnobs (r, { K (A ? ID::oscAUnison : ID::oscBUnison), K (A ? ID::oscADetune : ID::oscBDetune), K (A ? ID::oscALevel : ID::oscBLevel) });
    }
    {
        auto r = filter.getLocalBounds().reduced (10).withTrimmedTop (24);
        auto top = r.removeFromTop (26);
        filtType.setBounds (top.reduced (90, 0));
        auto big = r.removeFromTop ((int) (r.getHeight() * 0.55f));
        layoutKnobs (big, { K (ID::filtCutoff), K (ID::filtRes) });
        layoutKnobs (r, { K (ID::filtEnv), K (ID::filtKey), K (ID::filtDrive) });
    }

    // Row 2
    ampEnv.setBounds (16, 307, 190, 175);
    filtEnv.setBounds (216, 307, 190, 175);
    lfo.setBounds (416, 307, 170, 175);
    pitchP.setBounds (596, 307, 160, 175);
    global.setBounds (766, 307, 198, 175);
    {
        auto r = ampEnv.getLocalBounds().reduced (8).withTrimmedTop (24);
        ampEnvDisp.setBounds (r.removeFromTop (52));
        layoutKnobs (r, { K (ID::ampA), K (ID::ampD), K (ID::ampS), K (ID::ampR) });
        r = filtEnv.getLocalBounds().reduced (8).withTrimmedTop (24);
        filtEnvDisp.setBounds (r.removeFromTop (52));
        layoutKnobs (r, { K (ID::fA), K (ID::fD), K (ID::fS), K (ID::fR) });
    }
    {
        auto r = lfo.getLocalBounds().reduced (10).withTrimmedTop (24);
        lfoShape.setBounds (r.removeFromTop (24).reduced (24, 0));
        layoutKnobs (r, { K (ID::lfoRate), K (ID::lfoPitch), K (ID::lfoFilter) });
    }
    {
        auto r = pitchP.getLocalBounds().reduced (10).withTrimmedTop (24);
        voiceMode.setBounds (r.removeFromTop (24).reduced (20, 0));
        layoutKnobs (r, { K (ID::pitchEnvAmt), K (ID::pitchEnvDec), K (ID::glide) });
    }
    {
        auto r = global.getLocalBounds().reduced (8).withTrimmedTop (24);
        auto row1 = r.removeFromTop (r.getHeight() / 2);
        layoutKnobs (row1, { K (ID::subLevel), K (ID::noiseLevel), K (ID::drift) });
        layoutKnobs (r, { K (ID::width), K (ID::master) });
    }

    // Row 3 (FX)
    lofi.setBounds (16, 492, 420, 140);
    chorusP.setBounds (446, 492, 170, 140);
    delayP.setBounds (626, 492, 170, 140);
    verbP.setBounds (806, 492, 158, 140);
    layoutKnobs (lofi.getLocalBounds().reduced (8).withTrimmedTop (24),
                 { K (ID::crushBits), K (ID::crushRate), K (ID::lofiTone), K (ID::wow), K (ID::wowRate), K (ID::heat), K (ID::dist), K (ID::crackle) });
    layoutKnobs (chorusP.getLocalBounds().reduced (8).withTrimmedTop (24), { K (ID::chorusMix), K (ID::chorusRate), K (ID::chorusDepth) });
    layoutKnobs (delayP.getLocalBounds().reduced (8).withTrimmedTop (24), { K (ID::delayMix), K (ID::delayTime), K (ID::delayFb) });
    layoutKnobs (verbP.getLocalBounds().reduced (8).withTrimmedTop (24), { K (ID::verbMix), K (ID::verbSize), K (ID::verbDamp) });

    // Row 4 (sample) - collapsible
    int y = 642;
    sampleP.setBounds (16, y, 948, sampleOpen ? 128 : 28);
    setPanelOpen (sampleP, sampleFold, sampleOpen);
    y += sampleP.getHeight() + 8;
    if (sampleOpen)
    {
        auto r = sampleP.getLocalBounds().reduced (10).withTrimmedTop (24);
        waveform.setBounds (r.removeFromLeft (330));
        r.removeFromLeft (8);
        auto btns = r.removeFromLeft (80);
        loadSampleBtn.setBounds (btns.removeFromTop (26)); btns.removeFromTop (5);
        toMidiBtn.setBounds (btns.removeFromTop (26)); btns.removeFromTop (5);
        chopBtn.setBounds (btns.removeFromTop (26));
        r.removeFromLeft (8);
        auto pads = r.removeFromRight (116);
        notesPad.setBounds (pads.removeFromTop (42)); pads.removeFromTop (8);
        chopsPad.setBounds (pads.removeFromTop (42));
        r.removeFromRight (8);
        auto tog = r.removeFromRight (62);
        smpLoopT.setBounds (tog.removeFromTop (21)); smpRevT.setBounds (tog.removeFromTop (21)); smpChopT.setBounds (tog.removeFromTop (21)); smpHalfT.setBounds (tog.removeFromTop (21));
        r.removeFromRight (4);
        layoutKnobs (r, { K (ID::smpLevel), K (ID::smpRoot), K (ID::smpTune), K (ID::smpStart), K (ID::smpEnd) });
    }

    // SAMPLER page (replaces rows 1-4)
    if (page == 1)
    {
        y = 82 + 688 + 8;
        samplerP.setBounds (16, 82, 948, 688);
        auto r = samplerP.getLocalBounds().reduced (10).withTrimmedTop (24);
        bigWave.setBounds (r.removeFromTop (230));
        r.removeFromTop (8);
        auto row = r.removeFromTop (26);
        sLoadBtn.setBounds (row.removeFromLeft (80)); row.removeFromLeft (6);
        sMidiBtn.setBounds (row.removeFromLeft (80)); row.removeFromLeft (6);
        sChopBtn.setBounds (row.removeFromLeft (90)); row.removeFromLeft (14);
        for (auto* t : { &sLoopT, &sRevT, &sChopT, &sHalfT }) { t->setBounds (row.removeFromLeft (62)); row.removeFromLeft (2); }
        row.removeFromLeft (10);
        sOnlyT.setBounds (row.removeFromLeft (200));
        sChopsPad.setBounds (row.removeFromRight (116).reduced (0, 0)); row.removeFromRight (6);
        sNotesPad.setBounds (row.removeFromRight (116));
        r.removeFromTop (8);
        auto kr = r.removeFromTop (100);
        {
            const int n = (int) samplerKnobs.size(), cellW = kr.getWidth() / n, size = juce::jmin (cellW, kr.getHeight() - 14);
            int x = kr.getX();
            for (auto* k : samplerKnobs) { k->setBounds (juce::Rectangle<int> (x, kr.getY(), cellW, kr.getHeight()).withSizeKeepingCentre (size, size + 14)); x += cellW; }
        }
        r.removeFromTop (4);
        padHint.setBounds (r.removeFromTop (16));
        pads.setBounds (r);
    }

    // Row 5 (chord engine) - collapsible
    chordsP.setBounds (16, y, 948, chordsOpen ? 242 : 28);
    setPanelOpen (chordsP, chordsFold, chordsOpen);
    if (chordsOpen)
    {
        auto r = chordsP.getLocalBounds().reduced (10).withTrimmedTop (28);
        auto top = r.removeFromTop (26);
        auto place = [&] (juce::ComboBox& box, juce::Label& lbl, int w, int lblW)
        {
            auto cell = top.removeFromLeft (w);
            lbl.setBounds (cell.removeFromLeft (lblW));
            box.setBounds (cell.reduced (2, 0));
            top.removeFromLeft (8);
        };
        place (keyBox, keyLbl, 86, 30);
        place (scaleBox, scaleLbl, 160, 42);
        place (styleBox, styleLbl, 160, 42);
        place (barsBox, barsLbl, 100, 30);
        place (rhythmBox, rhythmLbl, 130, 34);
        strumBtn.setBounds (top.removeFromLeft (62));
        playBtn.setBounds (top.removeFromRight (64)); top.removeFromRight (6);
        generateBtn.setBounds (top.removeFromRight (96));

        r.removeFromTop (10);
        MidiDragPad* pads[4] { &chordPad, &melodyPad, &bassPad, &drumPad };
        for (int l = 0; l < 4; ++l)
        {
            auto row = r.removeFromTop (36);
            layerBtn[l].setBounds (row.removeFromLeft (76).reduced (0, 4));
            row.removeFromLeft (8);
            auto sound = row.removeFromRight (190);
            row.removeFromRight (6);
            layerLoad[l].setBounds (row.removeFromRight (50).reduced (0, 5));
            row.removeFromRight (6);
            if (l == 0) chordSoundLbl.setBounds (sound);
            else if (l == 3) drumSoundLbl.setBounds (sound);
            else layerSound[l - 1].setBounds (sound.reduced (0, 5));
            pads[l]->setBounds (row);
            r.removeFromTop (6);
        }
    }

    std::vector<juce::Rectangle<int>> panelRects;
    for (auto* pn : { &oscA, &oscB, &filter, &ampEnv, &filtEnv, &lfo, &pitchP, &global, &lofi, &chorusP, &delayP, &verbP, &sampleP, &chordsP, &samplerP })
        if (pn->isVisible()) panelRects.push_back (pn->getBounds());
    panelRects.push_back (scope.getBounds());
    panelRects.push_back (presetBox.getBounds());
    background.setExclusions (panelRects);
}
