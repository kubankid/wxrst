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
    g.setFont (juce::Font (juce::FontOptions (30.f, juce::Font::bold)));
    const float sh = 0.5f + 0.5f * std::sin (t * 0.6f);
    juce::ColourGradient logo (midnight ? juce::Colour (0xffffe08a) : IceColours::iceGlow, 20.f + sh * 60.f, 10.f,
                               midnight ? juce::Colour (0xffff2fa0) : IceColours::iceDeep, 260.f + sh * 60.f, 60.f, false);
    if (midnight)
    {
        // dark halo so the chrome lettering stays readable over the sun
        g.setColour (IceColours::bg.withAlpha (0.75f));
        for (auto off : { juce::Point<int> (2, 2), juce::Point<int> (-1, 1), juce::Point<int> (1, -1), juce::Point<int> (0, 3) })
            g.drawText ("DENNYDIDIT", 22 + off.x, 10 + off.y, 300, 40, juce::Justification::centredLeft);
    }
    g.setGradientFill (logo);
    g.drawText ("DENNYDIDIT", 22, 10, 300, 40, juce::Justification::centredLeft);
    g.setColour (IceColours::dimText);
    g.setFont (juce::Font (juce::FontOptions (10.f)));
    g.drawText (midnight ? "MIDNIGHT MODE  /  SYNTHWAVE  /  CS-80  /  16-BIT" : "LO-FI PADS  /  KEYS  /  808s  /  CHORD ENGINE",
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

    if (! has)
    {
        g.setColour (dimText);
        g.setFont (juce::Font (juce::FontOptions (12.f)));
        g.drawText (kind == 1 ? "Melody appears here" : kind == 2 ? "808 / bass line appears here"
                              : "Hit GENERATE, then drag these boxes into FL's piano roll or playlist",
                    r.toNearestInt(), juce::Justification::centred);
        return;
    }

    auto area = r.reduced (8.f, 5.f);
    auto hint = area.removeFromRight (118.f);
    g.setColour (ice);
    g.setFont (juce::Font (juce::FontOptions (11.f, juce::Font::bold)));
    g.drawText (kind == 1 ? "DRAG MELODY  >>" : kind == 2 ? "DRAG 808  >>" : "DRAG CHORDS  >>", hint.toNearestInt(), juce::Justification::centredRight);

    const bool playing = proc.isChordPlaying();
    const double pos = proc.playPosition.load();

    if (kind == 0)
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
        const auto& evs = kind == 1 ? prog->melodyEvents : prog->bassEvents;
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
    addAndMakeVisible (background);

    for (auto* pn : { &oscA, &oscB, &filter, &ampEnv, &filtEnv, &lfo, &pitchP, &global, &lofi, &chorusP, &delayP, &verbP, &chordsP })
    {
        addAndMakeVisible (pn);
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
    addAndMakeVisible (animBtn);
    addAndMakeVisible (scope);
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
        if (! visiblePresets.empty() && std::find (visiblePresets.begin(), visiblePresets.end(), proc.getCurrentProgram()) == visiblePresets.end())
            proc.loadPreset (visiblePresets.front());
        lastProgram = -1;
    };
    rebuildPresetList();
    presetBox.onChange = [this] { if (presetBox.getSelectedId() > 0) proc.loadPreset (presetBox.getSelectedId() - 1); };
    auto step = [this] (int dir)
    {
        if (visiblePresets.empty()) return;
        const int cur = proc.getCurrentProgram();
        int pos = 0;
        for (size_t k = 0; k < visiblePresets.size(); ++k) if (visiblePresets[k] == cur) pos = (int) k;
        pos = (pos + dir + (int) visiblePresets.size()) % (int) visiblePresets.size();
        proc.loadPreset (visiblePresets[(size_t) pos]);
    };
    prevBtn.onClick = [step] { step (-1); };
    nextBtn.onClick = [step] { step (1); };
    diceBtn.onClick = [this] { proc.randomiseSound(); };
    diceBtn.setTooltip ("Roll the dice: randomise the sound");
    addAndMakeVisible (categoryBox); addAndMakeVisible (presetBox); addAndMakeVisible (prevBtn); addAndMakeVisible (nextBtn); addAndMakeVisible (diceBtn);

    // MIDNIGHT MODE tab: 80s synthwave skin, saved with the plugin state
    const int savedTheme = juce::SystemStats::getEnvironmentVariable ("ICEWATER_THEME", "").isNotEmpty()
                         ? juce::SystemStats::getEnvironmentVariable ("ICEWATER_THEME", "0").getIntValue()
                         : (int) proc.apvts.state.getProperty ("theme", 1); // Midnight by default
    midnightTab.setTooltip ("Midnight Mode: 80s synthwave look");
    midnightTab.onClick = [this]
    {
        const bool on = midnightTab.getToggleState();
        applyTheme (on ? 1 : 0);
        proc.apvts.state.setProperty ("theme", on ? 1 : 0, nullptr);
        // flipping Midnight on jumps the browser to the Synthwave sounds
        if (on)
        {
            const int idx = presetCategories.indexOf ("Synthwave");
            if (idx >= 0 && categoryBox.getSelectedId() != idx + 1) categoryBox.setSelectedId (idx + 1, juce::sendNotificationSync);
        }
    };
    addAndMakeVisible (midnightTab);
    midnightTab.setToggleState (savedTheme == 1, juce::dontSendNotification);
    applyTheme (savedTheme);

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
    for (int l = 0; l < 3; ++l)
    {
        auto& b = layerBtn[l];
        chordsP.addAndMakeVisible (b);
        b.setClickingTogglesState (true);
        b.setToggleState (proc.layerEnabled[l].load(), juce::dontSendNotification);
        b.setTooltip ("Include this layer when playing");
        b.onClick = [this, l] { proc.layerEnabled[l].store (layerBtn[l].getToggleState()); };
    }
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

    startTimerHz (10);
    timerCallback();
    setSize (980, 852);
}

void IcewaterEditor::generateChords()
{
    chordSeed = chordSeed * 1664525u + 1013904223u;
    ChordGen::Options o;
    o.key = keyBox.getSelectedId() - 1; o.scale = scaleBox.getSelectedId() - 1; o.style = styleBox.getSelectedId() - 1;
    o.bars = barsBox.getSelectedId() == 2 ? 8 : 4; o.rhythm = rhythmBox.getSelectedId() - 1;
    o.strum = strumBtn.getToggleState(); o.seed = chordSeed;

    auto prog = std::make_shared<Progression> (ChordGen::generate (o));
    const auto names = ChordGen::chordListString (*prog);
    chordPad.setFile (ChordGen::writeMidiFile (prog->events, prog->title + " chords (" + names + ")", proc.getHostBpm()));
    melodyPad.setFile (ChordGen::writeMidiFile (prog->melodyEvents, prog->title + " melody (" + names + ")", proc.getHostBpm()));
    bassPad.setFile (ChordGen::writeMidiFile (prog->bassEvents, prog->title + " 808 (" + names + ")", proc.getHostBpm()));
    proc.setProgression (prog);
}

IcewaterEditor::~IcewaterEditor()
{
    setLookAndFeel (nullptr);
    IceColours::applyTheme (0); // the LookAndFeel colours are global: leave them in the default state
}

void IcewaterEditor::applyTheme (int which)
{
    IceColours::applyTheme (which);
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
            visiblePresets.push_back (i);
        }
    presetBox.setSelectedId (proc.getCurrentProgram() + 1, juce::dontSendNotification);
}

void IcewaterEditor::timerCallback()
{
    const int prog = proc.getCurrentProgram();
    if (prog != lastProgram)
    {
        lastProgram = prog;
        if (presetBox.indexOfItemId (prog + 1) < 0)
        {
            // current preset isn't in the chosen category: switch category to "All" so it shows
            categoryBox.setSelectedId (1, juce::dontSendNotification);
            rebuildPresetList();
            return;
        }
        presetBox.setSelectedId (prog + 1, juce::dontSendNotification);
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

void IcewaterEditor::resized()
{
    auto K = [&] (const char* id) { return knobById[id]; };

    // Header
    background.setBounds (getLocalBounds());
    animBtn.setBounds (216, 24, 24, 24);
    categoryBox.setBounds (250, 22, 86, 28);
    prevBtn.setBounds (342, 22, 26, 28);
    presetBox.setBounds (372, 22, 186, 28);
    nextBtn.setBounds (562, 22, 26, 28);
    diceBtn.setBounds (596, 20, 32, 32);
    midnightTab.setBounds (638, 22, 84, 28);
    scope.setBounds (734, 12, 230, 52);

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

    // Row 4 (chord engine)
    chordsP.setBounds (16, 642, 948, 200);
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
        MidiDragPad* pads[3] { &chordPad, &melodyPad, &bassPad };
        for (int l = 0; l < 3; ++l)
        {
            auto row = r.removeFromTop (36);
            layerBtn[l].setBounds (row.removeFromLeft (76).reduced (0, 4));
            row.removeFromLeft (8);
            auto sound = row.removeFromRight (190);
            row.removeFromRight (8);
            if (l == 0) chordSoundLbl.setBounds (sound);
            else layerSound[l - 1].setBounds (sound.reduced (0, 5));
            pads[l]->setBounds (row);
            r.removeFromTop (6);
        }
    }

    std::vector<juce::Rectangle<int>> panelRects;
    for (auto* pn : { &oscA, &oscB, &filter, &ampEnv, &filtEnv, &lfo, &pitchP, &global, &lofi, &chorusP, &delayP, &verbP, &chordsP })
        panelRects.push_back (pn->getBounds());
    panelRects.push_back (scope.getBounds());
    panelRects.push_back (presetBox.getBounds());
    background.setExclusions (panelRects);
}
