#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace ID
{
    // Oscillators
    inline constexpr auto oscAOn = "oscAOn", oscAWave = "oscAWave", oscAOct = "oscAOct", oscASemi = "oscASemi",
                          oscAFine = "oscAFine", oscAUnison = "oscAUnison", oscADetune = "oscADetune", oscALevel = "oscALevel";
    inline constexpr auto oscBOn = "oscBOn", oscBWave = "oscBWave", oscBOct = "oscBOct", oscBSemi = "oscBSemi",
                          oscBFine = "oscBFine", oscBUnison = "oscBUnison", oscBDetune = "oscBDetune", oscBLevel = "oscBLevel";
    inline constexpr auto noiseLevel = "noiseLevel", subLevel = "subLevel", drift = "drift";
    inline constexpr auto glide = "glide";
    inline constexpr auto voiceMode = "voiceMode", pitchEnvAmt = "pitchEnvAmt", pitchEnvDec = "pitchEnvDec";

    // Filter
    inline constexpr auto filtType = "filtType";
    inline constexpr auto filtCutoff = "filtCutoff", filtRes = "filtRes", filtEnv = "filtEnv", filtKey = "filtKey", filtDrive = "filtDrive";

    // Envelopes
    inline constexpr auto ampA = "ampA", ampD = "ampD", ampS = "ampS", ampR = "ampR";
    inline constexpr auto fA = "fA", fD = "fD", fS = "fS", fR = "fR";

    // LFO
    inline constexpr auto lfoRate = "lfoRate", lfoPitch = "lfoPitch", lfoFilter = "lfoFilter", lfoShape = "lfoShape";

    // FX
    inline constexpr auto crushBits = "crushBits", crushRate = "crushRate", lofiTone = "lofiTone", wow = "wow", wowRate = "wowRate";
    inline constexpr auto heat = "heat", crackle = "crackle", dist = "dist";
    inline constexpr auto chorusMix = "chorusMix", chorusRate = "chorusRate", chorusDepth = "chorusDepth";
    inline constexpr auto delayMix = "delayMix", delayTime = "delayTime", delayFb = "delayFb";
    inline constexpr auto verbMix = "verbMix", verbSize = "verbSize", verbDamp = "verbDamp";
    inline constexpr auto width = "width", master = "master";
}

inline const juce::StringArray waveNames { "Saw", "Square", "Triangle", "Sine", "Pulse", "Glass" };
inline const juce::StringArray lfoShapeNames { "Sine", "Triangle", "Saw", "Square", "Random" };
inline const juce::StringArray filterTypeNames { "LP 12", "LP 24", "BP", "HP" };
inline const juce::StringArray voiceModeNames { "Poly", "Mono" };

inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;

    auto f = [&] (const char* id, const char* name, float lo, float hi, float def, float skew = 1.0f, const String& unit = {})
    {
        NormalisableRange<float> r (lo, hi, 0.0f, skew);
        p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { id, 1 }, name, r, def,
                     AudioParameterFloatAttributes().withLabel (unit)));
    };
    auto i = [&] (const char* id, const char* name, int lo, int hi, int def)
    {
        p.push_back (std::make_unique<AudioParameterInt> (ParameterID { id, 1 }, name, lo, hi, def));
    };
    auto c = [&] (const char* id, const char* name, const StringArray& choices, int def)
    {
        p.push_back (std::make_unique<AudioParameterChoice> (ParameterID { id, 1 }, name, choices, def));
    };
    auto b = [&] (const char* id, const char* name, bool def)
    {
        p.push_back (std::make_unique<AudioParameterBool> (ParameterID { id, 1 }, name, def));
    };

    // Osc A
    b (ID::oscAOn, "Osc A On", true);
    c (ID::oscAWave, "Osc A Wave", waveNames, 0);
    i (ID::oscAOct, "Osc A Octave", -2, 2, 0);
    i (ID::oscASemi, "Osc A Semi", -12, 12, 0);
    f (ID::oscAFine, "Osc A Fine", -100.f, 100.f, 0.f, 1.f, "ct");
    i (ID::oscAUnison, "Osc A Unison", 1, 7, 3);
    f (ID::oscADetune, "Osc A Detune", 0.f, 100.f, 20.f);
    f (ID::oscALevel, "Osc A Level", 0.f, 1.f, 0.8f);

    // Osc B
    b (ID::oscBOn, "Osc B On", true);
    c (ID::oscBWave, "Osc B Wave", waveNames, 2);
    i (ID::oscBOct, "Osc B Octave", -2, 2, -1);
    i (ID::oscBSemi, "Osc B Semi", -12, 12, 0);
    f (ID::oscBFine, "Osc B Fine", -100.f, 100.f, 0.f, 1.f, "ct");
    i (ID::oscBUnison, "Osc B Unison", 1, 7, 1);
    f (ID::oscBDetune, "Osc B Detune", 0.f, 100.f, 10.f);
    f (ID::oscBLevel, "Osc B Level", 0.f, 1.f, 0.5f);

    f (ID::noiseLevel, "Noise", 0.f, 1.f, 0.05f);
    f (ID::subLevel, "Sub", 0.f, 1.f, 0.3f);
    f (ID::drift, "Drift", 0.f, 1.f, 0.3f);
    f (ID::glide, "Glide", 0.f, 2.f, 0.f, 0.4f, "s");
    c (ID::voiceMode, "Voice Mode", voiceModeNames, 0);
    f (ID::pitchEnvAmt, "Pitch Env", -24.f, 24.f, 0.f, 1.f, "st");
    f (ID::pitchEnvDec, "Pitch Env Decay", 0.005f, 2.f, 0.08f, 0.4f, "s");

    // Filter
    c (ID::filtType, "Filter Type", filterTypeNames, 0);
    f (ID::filtCutoff, "Cutoff", 20.f, 20000.f, 2500.f, 0.3f, "Hz");
    f (ID::filtRes, "Resonance", 0.f, 1.f, 0.2f);
    f (ID::filtEnv, "Filter Env", -1.f, 1.f, 0.3f);
    f (ID::filtKey, "Key Track", 0.f, 1.f, 0.3f);
    f (ID::filtDrive, "Drive", 0.f, 1.f, 0.1f);

    // Envs
    f (ID::ampA, "Amp Attack", 0.001f, 8.f, 0.6f, 0.35f, "s");
    f (ID::ampD, "Amp Decay", 0.001f, 8.f, 1.0f, 0.35f, "s");
    f (ID::ampS, "Amp Sustain", 0.f, 1.f, 0.8f);
    f (ID::ampR, "Amp Release", 0.001f, 12.f, 1.5f, 0.35f, "s");
    f (ID::fA, "Filt Attack", 0.001f, 8.f, 1.0f, 0.35f, "s");
    f (ID::fD, "Filt Decay", 0.001f, 8.f, 2.0f, 0.35f, "s");
    f (ID::fS, "Filt Sustain", 0.f, 1.f, 0.4f);
    f (ID::fR, "Filt Release", 0.001f, 12.f, 2.0f, 0.35f, "s");

    // LFO
    c (ID::lfoShape, "LFO Shape", lfoShapeNames, 0);
    f (ID::lfoRate, "LFO Rate", 0.02f, 20.f, 0.4f, 0.35f, "Hz");
    f (ID::lfoPitch, "LFO > Pitch", 0.f, 1.f, 0.08f);
    f (ID::lfoFilter, "LFO > Filter", -1.f, 1.f, 0.15f);

    // FX
    f (ID::crushBits, "Bit Depth", 3.f, 16.f, 12.f);
    f (ID::crushRate, "Sample Rate", 2000.f, 48000.f, 22050.f, 0.4f, "Hz");
    f (ID::lofiTone, "Tone", 500.f, 20000.f, 9000.f, 0.3f, "Hz");
    f (ID::wow, "Wow", 0.f, 1.f, 0.25f);
    f (ID::wowRate, "Wow Rate", 0.05f, 6.f, 0.6f, 0.5f, "Hz");
    f (ID::heat, "Heat", 0.f, 1.f, 0.2f);
    f (ID::dist, "Dist", 0.f, 1.f, 0.f);
    f (ID::crackle, "Crackle", 0.f, 1.f, 0.1f);
    f (ID::chorusMix, "Chorus Mix", 0.f, 1.f, 0.45f);
    f (ID::chorusRate, "Chorus Rate", 0.05f, 5.f, 0.3f, 0.5f, "Hz");
    f (ID::chorusDepth, "Chorus Depth", 0.f, 1.f, 0.4f);
    f (ID::delayMix, "Delay Mix", 0.f, 1.f, 0.15f);
    f (ID::delayTime, "Delay Time", 0.02f, 1.5f, 0.375f, 0.5f, "s");
    f (ID::delayFb, "Delay Feedback", 0.f, 0.95f, 0.4f);
    f (ID::verbMix, "Reverb Mix", 0.f, 1.f, 0.35f);
    f (ID::verbSize, "Reverb Size", 0.f, 1.f, 0.8f);
    f (ID::verbDamp, "Reverb Damp", 0.f, 1.f, 0.5f);
    f (ID::width, "Width", 0.f, 1.f, 0.8f);
    f (ID::master, "Master", -36.f, 6.f, -6.f, 1.f, "dB");

    return { p.begin(), p.end() };
}
