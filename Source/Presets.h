#pragma once
#include "Params.h"
#include <vector>
#include <utility>

struct Preset
{
    const char* name;
    const char* category; // "Pads", "Keys", "808", "Bass", "Synth"
    std::vector<std::pair<const char*, float>> values; // param id -> value in real units
};

inline const juce::StringArray presetCategories { "All", "Pads", "Keys", "808", "Bass", "Synth", "Synthwave", "16-Bit", "Drift", "User" };

// Any param not listed falls back to its default (see Params.h). Values are real-world units.
inline const std::vector<Preset>& getPresets()
{
    using namespace ID;
    static const std::vector<Preset> presets {
        { "Init Pad", "Pads", {} },

        { "Memory Card", "Pads", {
            { oscAWave, 2 }, { oscAUnison, 2 }, { oscADetune, 12 }, { oscALevel, 0.8f },
            { oscBWave, 3 }, { oscBOct, -1 }, { oscBLevel, 0.6f },
            { filtCutoff, 3000 }, { filtRes, 0.1f }, { filtEnv, 0.2f },
            { ampA, 0.4f }, { ampR, 1.8f },
            { crushBits, 10 }, { crushRate, 16000 }, { lofiTone, 7000 }, { wow, 0.3f },
            { chorusMix, 0.55f }, { chorusDepth, 0.5f }, { verbMix, 0.4f }, { verbSize, 0.75f }, { delayMix, 0.1f } } },

        { "Ice Water", "Pads", {
            { oscAWave, 5 }, { oscAUnison, 4 }, { oscADetune, 25 }, { oscALevel, 0.7f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBUnison, 5 }, { oscBDetune, 30 }, { oscBLevel, 0.35f },
            { noiseLevel, 0.03f },
            { filtCutoff, 6000 }, { filtRes, 0.15f }, { filtEnv, 0.25f }, { filtKey, 0.5f },
            { ampA, 0.9f }, { ampD, 2 }, { ampS, 0.85f }, { ampR, 2.5f },
            { lfoRate, 0.25f }, { lfoPitch, 0.06f }, { lfoFilter, 0.2f },
            { crushBits, 12 }, { crushRate, 24000 }, { lofiTone, 11000 }, { wow, 0.4f }, { wowRate, 0.4f },
            { chorusMix, 0.5f }, { chorusRate, 0.25f }, { chorusDepth, 0.6f },
            { delayMix, 0.22f }, { delayTime, 0.5f }, { delayFb, 0.45f },
            { verbMix, 0.5f }, { verbSize, 0.92f }, { verbDamp, 0.3f }, { width, 1.f } } },

        { "Chlorine Pool", "Pads", {
            { oscAWave, 1 }, { oscAUnison, 3 }, { oscADetune, 18 }, { oscALevel, 0.6f },
            { oscBWave, 2 }, { oscBOct, -1 }, { oscBUnison, 2 }, { oscBDetune, 10 }, { oscBLevel, 0.6f },
            { filtCutoff, 1400 }, { filtRes, 0.3f }, { filtEnv, 0.35f },
            { ampA, 0.7f }, { ampR, 2.f }, { fA, 0.5f }, { fD, 3.f }, { fS, 0.5f },
            { lfoRate, 0.6f }, { lfoPitch, 0.18f }, { lfoFilter, 0.3f },
            { crushBits, 11 }, { crushRate, 18000 }, { lofiTone, 6000 }, { wow, 0.55f }, { wowRate, 0.9f },
            { chorusMix, 0.8f }, { chorusRate, 0.5f }, { chorusDepth, 0.8f },
            { delayMix, 0.1f }, { verbMix, 0.45f }, { verbSize, 0.85f } } },

        { "Save Point", "Pads", {
            { oscAWave, 3 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 4 }, { oscBOct, 0 }, { oscBSemi, 12 }, { oscBLevel, 0.25f },
            { filtCutoff, 2200 }, { filtRes, 0.05f }, { filtEnv, 0.4f },
            { ampA, 0.05f }, { ampD, 1.5f }, { ampS, 0.6f }, { ampR, 1.2f }, { fA, 0.01f }, { fD, 1.0f }, { fS, 0.2f },
            { crushBits, 8 }, { crushRate, 12000 }, { lofiTone, 5500 }, { wow, 0.15f },
            { chorusMix, 0.3f }, { delayMix, 0.35f }, { delayTime, 0.375f }, { delayFb, 0.5f },
            { verbMix, 0.3f }, { verbSize, 0.7f } } },

        { "VHS Static", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 5 }, { oscADetune, 35 }, { oscALevel, 0.6f },
            { oscBWave, 2 }, { oscBOct, -1 }, { oscBLevel, 0.5f },
            { noiseLevel, 0.25f },
            { filtCutoff, 1800 }, { filtRes, 0.2f }, { filtEnv, 0.15f }, { filtDrive, 0.3f },
            { ampA, 1.2f }, { ampR, 3.f },
            { lfoRate, 0.15f }, { lfoPitch, 0.1f }, { lfoFilter, 0.1f },
            { crushBits, 9 }, { crushRate, 14000 }, { lofiTone, 4000 }, { wow, 0.8f }, { wowRate, 1.3f },
            { chorusMix, 0.4f }, { chorusDepth, 0.5f }, { verbMix, 0.4f }, { verbSize, 0.8f }, { verbDamp, 0.7f } } },

        { "Creamsicle", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 7 }, { oscADetune, 45 }, { oscALevel, 0.7f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBUnison, 3 }, { oscBDetune, 15 }, { oscBLevel, 0.45f },
            { filtCutoff, 900 }, { filtRes, 0.1f }, { filtEnv, 0.4f }, { filtKey, 0.4f }, { filtDrive, 0.25f },
            { ampA, 1.5f }, { ampD, 2 }, { ampS, 0.9f }, { ampR, 3.f }, { fA, 1.8f }, { fD, 3.f }, { fS, 0.6f }, { fR, 3.f },
            { lfoRate, 0.2f }, { lfoPitch, 0.04f }, { lfoFilter, 0.15f },
            { crushBits, 13 }, { crushRate, 30000 }, { lofiTone, 6500 }, { wow, 0.3f },
            { chorusMix, 0.5f }, { chorusDepth, 0.45f }, { delayMix, 0.08f }, { verbMix, 0.35f }, { verbSize, 0.8f }, { width, 1.f } } },

        { "Frostbite", "Pads", {
            { oscAWave, 5 }, { oscAUnison, 3 }, { oscADetune, 15 }, { oscALevel, 0.75f },
            { oscBWave, 0 }, { oscBOct, 1 }, { oscBUnison, 4 }, { oscBDetune, 20 }, { oscBLevel, 0.25f },
            { noiseLevel, 0.04f },
            { filtCutoff, 9000 }, { filtRes, 0.2f }, { filtEnv, 0.1f }, { filtKey, 0.6f },
            { ampA, 0.3f }, { ampD, 2.5f }, { ampS, 0.7f }, { ampR, 3.f },
            { lfoRate, 4.f }, { lfoPitch, 0.02f }, { lfoFilter, 0.05f },
            { crushBits, 12 }, { crushRate, 22050 }, { lofiTone, 14000 }, { wow, 0.2f },
            { chorusMix, 0.4f }, { chorusRate, 0.8f }, { chorusDepth, 0.3f },
            { delayMix, 0.35f }, { delayTime, 0.25f }, { delayFb, 0.6f },
            { verbMix, 0.5f }, { verbSize, 0.95f }, { verbDamp, 0.2f }, { width, 1.f } } },

        { "Dreamcast Lobby", "Pads", {
            { oscAWave, 1 }, { oscAUnison, 2 }, { oscADetune, 14 }, { oscALevel, 0.55f },
            { oscBWave, 2 }, { oscBOct, 0 }, { oscBSemi, 7 }, { oscBUnison, 2 }, { oscBDetune, 10 }, { oscBLevel, 0.4f },
            { filtCutoff, 2600 }, { filtRes, 0.25f }, { filtEnv, 0.3f },
            { ampA, 0.5f }, { ampR, 1.5f },
            { lfoRate, 0.35f }, { lfoPitch, 0.1f },
            { crushBits, 12 }, { crushRate, 22050 }, { lofiTone, 8000 }, { wow, 0.25f },
            { chorusMix, 0.65f }, { chorusRate, 0.4f }, { chorusDepth, 0.6f },
            { delayMix, 0.2f }, { delayTime, 0.5f }, { verbMix, 0.35f }, { verbSize, 0.7f } } },

        { "Aquarium", "Pads", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 2 }, { oscADetune, 8 }, { oscALevel, 0.8f },
            { oscBWave, 5 }, { oscBOct, 0 }, { oscBUnison, 3 }, { oscBDetune, 20 }, { oscBLevel, 0.45f },
            { filtCutoff, 1200 }, { filtRes, 0.45f }, { filtEnv, 0.2f },
            { ampA, 1.0f }, { ampS, 0.9f }, { ampR, 3.5f },
            { lfoShape, 1 }, { lfoRate, 0.12f }, { lfoPitch, 0.05f }, { lfoFilter, 0.6f },
            { crushBits, 11 }, { crushRate, 20000 }, { lofiTone, 7000 }, { wow, 0.5f }, { wowRate, 0.3f },
            { chorusMix, 0.7f }, { chorusRate, 0.2f }, { chorusDepth, 0.7f },
            { delayMix, 0.15f }, { delayTime, 0.75f }, { delayFb, 0.5f },
            { verbMix, 0.6f }, { verbSize, 1.f }, { verbDamp, 0.4f }, { width, 1.f } } },

        { "Loading Screen", "Pads", {
            { oscAWave, 4 }, { oscAUnison, 3 }, { oscADetune, 22 }, { oscALevel, 0.6f },
            { oscBWave, 4 }, { oscBOct, -1 }, { oscBUnison, 3 }, { oscBDetune, 22 }, { oscBLevel, 0.5f },
            { filtCutoff, 1600 }, { filtRes, 0.3f }, { filtEnv, 0.5f }, { filtDrive, 0.2f },
            { ampA, 0.6f }, { ampR, 1.5f }, { fA, 2.f }, { fD, 2.f }, { fS, 0.3f },
            { lfoRate, 0.3f }, { lfoPitch, 0.07f },
            { crushBits, 9 }, { crushRate, 11025 }, { lofiTone, 5000 }, { wow, 0.35f },
            { chorusMix, 0.5f }, { chorusDepth, 0.5f }, { delayMix, 0.2f }, { verbMix, 0.35f }, { verbSize, 0.8f } } },

        { "Glacier", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 7 }, { oscADetune, 30 }, { oscALevel, 0.65f },
            { oscBWave, 2 }, { oscBOct, -2 }, { oscBLevel, 0.5f },
            { noiseLevel, 0.06f },
            { filtCutoff, 400 }, { filtRes, 0.15f }, { filtEnv, 0.75f }, { filtKey, 0.3f },
            { ampA, 3.f }, { ampD, 3.f }, { ampS, 0.9f }, { ampR, 5.f }, { fA, 4.f }, { fD, 4.f }, { fS, 0.5f }, { fR, 5.f },
            { lfoRate, 0.1f }, { lfoPitch, 0.05f }, { lfoFilter, 0.1f },
            { crushBits, 12 }, { crushRate, 24000 }, { lofiTone, 9000 }, { wow, 0.35f }, { wowRate, 0.25f },
            { chorusMix, 0.45f }, { chorusDepth, 0.5f }, { delayMix, 0.15f }, { delayTime, 1.f }, { delayFb, 0.5f },
            { verbMix, 0.55f }, { verbSize, 1.f }, { verbDamp, 0.3f }, { width, 1.f } } },

        { "Night Drive", "Pads", {
            { oscAWave, 2 }, { oscAUnison, 3 }, { oscADetune, 16 }, { oscALevel, 0.7f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBUnison, 3 }, { oscBDetune, 18 }, { oscBLevel, 0.45f },
            { filtCutoff, 1800 }, { filtRes, 0.2f }, { filtEnv, 0.3f }, { filtDrive, 0.2f },
            { ampA, 0.4f }, { ampD, 1.5f }, { ampS, 0.8f }, { ampR, 2.f },
            { lfoRate, 0.5f }, { lfoPitch, 0.05f },
            { crushBits, 13 }, { crushRate, 32000 }, { lofiTone, 7500 }, { wow, 0.3f },
            { chorusMix, 0.4f }, { delayMix, 0.4f }, { delayTime, 0.375f }, { delayFb, 0.55f },
            { verbMix, 0.3f }, { verbSize, 0.75f } } },

        { "Fog Machine", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 5 }, { oscADetune, 50 }, { oscALevel, 0.5f },
            { oscBWave, 3 }, { oscBOct, -1 }, { oscBLevel, 0.5f },
            { noiseLevel, 0.4f },
            { filtCutoff, 600 }, { filtRes, 0.5f }, { filtEnv, 0.3f },
            { ampA, 2.f }, { ampS, 0.9f }, { ampR, 4.f }, { fA, 3.f }, { fS, 0.6f },
            { lfoShape, 4 }, { lfoRate, 2.f }, { lfoPitch, 0.03f }, { lfoFilter, 0.2f },
            { crushBits, 10 }, { crushRate, 16000 }, { lofiTone, 4500 }, { wow, 0.6f },
            { chorusMix, 0.6f }, { chorusDepth, 0.7f }, { verbMix, 0.6f }, { verbSize, 0.95f }, { verbDamp, 0.6f } } },

        { "Crystal Keys", "Pads", {
            { oscAWave, 5 }, { oscAUnison, 2 }, { oscADetune, 10 }, { oscALevel, 0.8f },
            { oscBWave, 3 }, { oscBOct, 1 }, { oscBLevel, 0.3f },
            { filtCutoff, 5000 }, { filtRes, 0.1f }, { filtEnv, 0.5f }, { filtKey, 0.7f },
            { ampA, 0.003f }, { ampD, 1.2f }, { ampS, 0.3f }, { ampR, 1.5f }, { fA, 0.003f }, { fD, 0.6f }, { fS, 0.1f }, { fR, 1.f },
            { lfoRate, 5.f }, { lfoPitch, 0.01f },
            { crushBits, 11 }, { crushRate, 22050 }, { lofiTone, 12000 }, { wow, 0.2f },
            { chorusMix, 0.45f }, { chorusDepth, 0.4f }, { delayMix, 0.35f }, { delayTime, 0.375f }, { delayFb, 0.5f },
            { verbMix, 0.45f }, { verbSize, 0.9f }, { verbDamp, 0.3f } } },

        // ---- Soft / lush / Drake-OVO style -----------------------------------
        { "OVO Nights", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 7 }, { oscADetune, 28 }, { oscALevel, 0.6f },
            { oscBWave, 3 }, { oscBOct, -1 }, { oscBUnison, 2 }, { oscBDetune, 6 }, { oscBLevel, 0.7f },
            { noiseLevel, 0.02f },
            { filtCutoff, 650 }, { filtRes, 0.12f }, { filtEnv, 0.25f }, { filtKey, 0.35f }, { filtDrive, 0.15f },
            { ampA, 1.4f }, { ampD, 2.f }, { ampS, 0.9f }, { ampR, 3.5f }, { fA, 2.5f }, { fD, 3.f }, { fS, 0.5f }, { fR, 3.f },
            { lfoRate, 0.15f }, { lfoPitch, 0.03f }, { lfoFilter, 0.08f },
            { crushBits, 14 }, { crushRate, 32000 }, { lofiTone, 5500 }, { wow, 0.2f }, { wowRate, 0.3f },
            { chorusMix, 0.5f }, { chorusRate, 0.2f }, { chorusDepth, 0.5f },
            { delayMix, 0.1f }, { delayTime, 0.5f }, { delayFb, 0.35f },
            { verbMix, 0.5f }, { verbSize, 0.9f }, { verbDamp, 0.6f }, { width, 0.9f }, { master, -8.f } } },

        { "Marvin's Haze", "Pads", {
            { oscAWave, 2 }, { oscAUnison, 5 }, { oscADetune, 22 }, { oscALevel, 0.7f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBUnison, 4 }, { oscBDetune, 16 }, { oscBLevel, 0.4f },
            { filtCutoff, 900 }, { filtRes, 0.18f }, { filtEnv, 0.2f }, { filtKey, 0.4f },
            { ampA, 1.8f }, { ampD, 2.5f }, { ampS, 0.9f }, { ampR, 4.f }, { fA, 3.f }, { fS, 0.6f }, { fR, 4.f },
            { lfoShape, 1 }, { lfoRate, 0.1f }, { lfoPitch, 0.04f }, { lfoFilter, 0.12f },
            { crushBits, 13 }, { crushRate, 28000 }, { lofiTone, 4800 }, { wow, 0.35f }, { wowRate, 0.25f },
            { chorusMix, 0.6f }, { chorusRate, 0.15f }, { chorusDepth, 0.65f },
            { delayMix, 0.12f }, { delayTime, 0.75f }, { delayFb, 0.4f },
            { verbMix, 0.6f }, { verbSize, 0.95f }, { verbDamp, 0.7f }, { width, 1.f }, { master, -8.f } } },

        { "6ix Fog", "Pads", {
            { oscAWave, 3 }, { oscAUnison, 3 }, { oscADetune, 12 }, { oscALevel, 0.8f },
            { oscBWave, 5 }, { oscBOct, 0 }, { oscBUnison, 3 }, { oscBDetune, 18 }, { oscBLevel, 0.3f },
            { noiseLevel, 0.08f },
            { filtCutoff, 1100 }, { filtRes, 0.1f }, { filtEnv, 0.15f }, { filtKey, 0.5f },
            { ampA, 2.2f }, { ampS, 0.95f }, { ampR, 5.f }, { fA, 3.f }, { fS, 0.5f },
            { lfoRate, 0.08f }, { lfoPitch, 0.03f }, { lfoFilter, 0.15f },
            { crushBits, 12 }, { crushRate, 24000 }, { lofiTone, 6000 }, { wow, 0.45f }, { wowRate, 0.2f },
            { chorusMix, 0.55f }, { chorusRate, 0.2f }, { chorusDepth, 0.6f },
            { delayMix, 0.18f }, { delayTime, 1.f }, { delayFb, 0.5f },
            { verbMix, 0.65f }, { verbSize, 1.f }, { verbDamp, 0.5f }, { width, 1.f }, { master, -8.f } } },

        { "Take Care", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 7 }, { oscADetune, 18 }, { oscALevel, 0.55f },
            { oscBWave, 2 }, { oscBOct, 0 }, { oscBSemi, 0 }, { oscBUnison, 3 }, { oscBDetune, 10 }, { oscBLevel, 0.6f },
            { filtCutoff, 1300 }, { filtRes, 0.08f }, { filtEnv, 0.2f }, { filtKey, 0.3f }, { filtDrive, 0.2f },
            { ampA, 0.9f }, { ampD, 2.f }, { ampS, 0.85f }, { ampR, 3.f }, { fA, 1.5f }, { fS, 0.5f },
            { lfoRate, 0.2f }, { lfoPitch, 0.04f },
            { crushBits, 14 }, { crushRate, 36000 }, { lofiTone, 7000 }, { wow, 0.25f },
            { chorusMix, 0.45f }, { chorusRate, 0.3f }, { chorusDepth, 0.5f },
            { delayMix, 0.08f }, { verbMix, 0.45f }, { verbSize, 0.85f }, { verbDamp, 0.6f }, { width, 0.85f }, { master, -8.f } } },

        { "Velvet Sub", "Pads", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.9f },
            { oscBWave, 2 }, { oscBOct, 0 }, { oscBUnison, 4 }, { oscBDetune, 14 }, { oscBLevel, 0.45f },
            { filtCutoff, 500 }, { filtRes, 0.15f }, { filtEnv, 0.3f }, { filtKey, 0.5f }, { filtDrive, 0.3f },
            { ampA, 0.6f }, { ampD, 2.f }, { ampS, 0.9f }, { ampR, 2.5f }, { fA, 1.2f }, { fD, 2.5f }, { fS, 0.4f },
            { lfoRate, 0.3f }, { lfoPitch, 0.02f },
            { crushBits, 14 }, { crushRate, 40000 }, { lofiTone, 4000 }, { wow, 0.15f },
            { chorusMix, 0.3f }, { chorusDepth, 0.4f }, { delayMix, 0.05f },
            { verbMix, 0.3f }, { verbSize, 0.8f }, { verbDamp, 0.8f }, { width, 0.6f }, { master, -6.f } } },

        { "Late Night Text", "Pads", {
            { oscAWave, 5 }, { oscAUnison, 2 }, { oscADetune, 8 }, { oscALevel, 0.6f },
            { oscBWave, 3 }, { oscBOct, -1 }, { oscBUnison, 2 }, { oscBDetune, 5 }, { oscBLevel, 0.7f },
            { filtCutoff, 1500 }, { filtRes, 0.1f }, { filtEnv, 0.45f }, { filtKey, 0.6f },
            { ampA, 0.02f }, { ampD, 2.5f }, { ampS, 0.5f }, { ampR, 2.5f }, { fA, 0.01f }, { fD, 1.5f }, { fS, 0.2f }, { fR, 2.f },
            { lfoRate, 0.25f }, { lfoPitch, 0.03f },
            { crushBits, 12 }, { crushRate, 22050 }, { lofiTone, 6500 }, { wow, 0.3f }, { wowRate, 0.35f },
            { chorusMix, 0.5f }, { chorusRate, 0.25f }, { chorusDepth, 0.55f },
            { delayMix, 0.3f }, { delayTime, 0.5f }, { delayFb, 0.5f },
            { verbMix, 0.5f }, { verbSize, 0.9f }, { verbDamp, 0.6f }, { width, 0.9f }, { master, -8.f } } },

        { "Pillowtalk", "Pads", {
            { oscAWave, 2 }, { oscAUnison, 6 }, { oscADetune, 24 }, { oscALevel, 0.7f },
            { oscBWave, 3 }, { oscBOct, 1 }, { oscBUnison, 2 }, { oscBDetune, 8 }, { oscBLevel, 0.25f },
            { noiseLevel, 0.03f },
            { filtCutoff, 1800 }, { filtRes, 0.05f }, { filtEnv, 0.1f }, { filtKey, 0.4f },
            { ampA, 2.5f }, { ampD, 2.f }, { ampS, 0.95f }, { ampR, 5.f }, { fA, 2.f }, { fS, 0.7f },
            { lfoRate, 0.12f }, { lfoPitch, 0.05f }, { lfoFilter, 0.1f },
            { crushBits, 13 }, { crushRate, 30000 }, { lofiTone, 7500 }, { wow, 0.3f }, { wowRate, 0.2f },
            { chorusMix, 0.65f }, { chorusRate, 0.18f }, { chorusDepth, 0.7f },
            { delayMix, 0.15f }, { delayTime, 0.75f }, { delayFb, 0.45f },
            { verbMix, 0.6f }, { verbSize, 0.95f }, { verbDamp, 0.5f }, { width, 1.f }, { master, -8.f } } },

        { "Houston Rain", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 7 }, { oscADetune, 40 }, { oscALevel, 0.5f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBUnison, 5 }, { oscBDetune, 20 }, { oscBLevel, 0.4f },
            { noiseLevel, 0.1f },
            { filtCutoff, 750 }, { filtRes, 0.25f }, { filtEnv, 0.35f }, { filtKey, 0.3f }, { filtDrive, 0.3f },
            { ampA, 1.6f }, { ampS, 0.9f }, { ampR, 4.f }, { fA, 2.5f }, { fD, 4.f }, { fS, 0.5f }, { fR, 4.f },
            { lfoShape, 1 }, { lfoRate, 0.1f }, { lfoPitch, 0.04f }, { lfoFilter, 0.2f },
            { crushBits, 12 }, { crushRate, 20000 }, { lofiTone, 4500 }, { wow, 0.5f }, { wowRate, 0.3f },
            { chorusMix, 0.5f }, { chorusDepth, 0.6f }, { delayMix, 0.1f }, { delayTime, 1.f },
            { verbMix, 0.6f }, { verbSize, 0.95f }, { verbDamp, 0.7f }, { width, 1.f }, { master, -8.f } } },

        { "Candlelight", "Pads", {
            { oscAWave, 3 }, { oscAUnison, 3 }, { oscADetune, 10 }, { oscALevel, 0.75f },
            { oscBWave, 2 }, { oscBOct, 0 }, { oscBSemi, 7 }, { oscBUnison, 2 }, { oscBDetune, 8 }, { oscBLevel, 0.3f },
            { filtCutoff, 2000 }, { filtRes, 0.05f }, { filtEnv, 0.15f }, { filtKey, 0.5f },
            { ampA, 1.2f }, { ampS, 0.9f }, { ampR, 3.5f }, { fA, 1.5f }, { fS, 0.6f },
            { lfoRate, 0.18f }, { lfoPitch, 0.03f },
            { crushBits, 13 }, { crushRate, 32000 }, { lofiTone, 6000 }, { wow, 0.25f }, { wowRate, 0.25f },
            { chorusMix, 0.55f }, { chorusRate, 0.22f }, { chorusDepth, 0.5f },
            { delayMix, 0.12f }, { delayTime, 0.375f }, { delayFb, 0.4f },
            { verbMix, 0.55f }, { verbSize, 0.9f }, { verbDamp, 0.6f }, { width, 0.9f }, { master, -8.f } } },

        { "Honest Hour", "Pads", {
            { oscAWave, 2 }, { oscAUnison, 7 }, { oscADetune, 30 }, { oscALevel, 0.6f },
            { oscBWave, 3 }, { oscBOct, -1 }, { oscBUnison, 3 }, { oscBDetune, 8 }, { oscBLevel, 0.65f },
            { noiseLevel, 0.04f },
            { filtCutoff, 800 }, { filtRes, 0.12f }, { filtEnv, 0.3f }, { filtKey, 0.4f }, { filtDrive, 0.2f },
            { ampA, 3.f }, { ampD, 3.f }, { ampS, 0.95f }, { ampR, 6.f }, { fA, 4.f }, { fD, 4.f }, { fS, 0.5f }, { fR, 5.f },
            { lfoRate, 0.07f }, { lfoPitch, 0.04f }, { lfoFilter, 0.15f },
            { crushBits, 13 }, { crushRate, 26000 }, { lofiTone, 5000 }, { wow, 0.4f }, { wowRate, 0.2f },
            { chorusMix, 0.6f }, { chorusRate, 0.15f }, { chorusDepth, 0.7f },
            { delayMix, 0.15f }, { delayTime, 1.f }, { delayFb, 0.5f },
            { verbMix, 0.7f }, { verbSize, 1.f }, { verbDamp, 0.6f }, { width, 1.f }, { master, -9.f } } },

        // ====================== KEYS ==========================================
        { "Glass Rhodes", "Keys", {
            { oscAWave, 5 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 3 }, { oscBOct, 1 }, { oscBLevel, 0.25f }, { subLevel, 0.15f }, { drift, 0.15f },
            { filtCutoff, 4500 }, { filtRes, 0.05f }, { filtEnv, 0.4f }, { filtKey, 0.6f },
            { ampA, 0.003f }, { ampD, 2.5f }, { ampS, 0.25f }, { ampR, 0.8f }, { fA, 0.003f }, { fD, 0.9f }, { fS, 0.15f }, { fR, 0.6f },
            { lfoRate, 5.5f }, { lfoPitch, 0.015f },
            { crushBits, 12 }, { crushRate, 26000 }, { lofiTone, 9000 }, { wow, 0.15f }, { heat, 0.25f }, { crackle, 0.1f },
            { chorusMix, 0.4f }, { chorusRate, 0.6f }, { chorusDepth, 0.35f }, { delayMix, 0.2f }, { delayTime, 0.375f }, { delayFb, 0.35f },
            { verbMix, 0.35f }, { verbSize, 0.8f }, { width, 0.8f }, { master, -6.f } } },

        { "PS2 Piano", "Keys", {
            { oscAWave, 1 }, { oscAUnison, 2 }, { oscADetune, 6 }, { oscALevel, 0.6f },
            { oscBWave, 2 }, { oscBOct, 1 }, { oscBLevel, 0.4f }, { subLevel, 0.2f }, { drift, 0.1f },
            { filtCutoff, 2200 }, { filtRes, 0.1f }, { filtEnv, 0.6f }, { filtKey, 0.7f },
            { ampA, 0.003f }, { ampD, 1.8f }, { ampS, 0.15f }, { ampR, 0.5f }, { fA, 0.003f }, { fD, 0.5f }, { fS, 0.1f }, { fR, 0.4f },
            { crushBits, 10 }, { crushRate, 16000 }, { lofiTone, 6500 }, { wow, 0.2f }, { heat, 0.2f }, { crackle, 0.15f },
            { chorusMix, 0.3f }, { delayMix, 0.15f }, { verbMix, 0.4f }, { verbSize, 0.75f }, { master, -6.f } } },

        { "Music Box", "Keys", {
            { oscAWave, 5 }, { oscAOct, 1 }, { oscAUnison, 1 }, { oscALevel, 0.7f },
            { oscBWave, 3 }, { oscBOct, 2 }, { oscBLevel, 0.3f }, { subLevel, 0.f }, { drift, 0.1f },
            { filtCutoff, 8000 }, { filtRes, 0.f }, { filtEnv, 0.3f }, { filtKey, 0.8f },
            { ampA, 0.002f }, { ampD, 1.2f }, { ampS, 0.f }, { ampR, 0.6f }, { fA, 0.002f }, { fD, 0.3f }, { fS, 0.f },
            { crushBits, 11 }, { crushRate, 22050 }, { lofiTone, 12000 }, { wow, 0.3f }, { wowRate, 0.4f }, { heat, 0.1f }, { crackle, 0.2f },
            { chorusMix, 0.3f }, { delayMix, 0.35f }, { delayTime, 0.375f }, { delayFb, 0.45f },
            { verbMix, 0.55f }, { verbSize, 0.9f }, { verbDamp, 0.3f }, { master, -7.f } } },

        { "Dusty E-Piano", "Keys", {
            { oscAWave, 3 }, { oscAUnison, 2 }, { oscADetune, 5 }, { oscALevel, 0.8f },
            { oscBWave, 5 }, { oscBOct, 0 }, { oscBLevel, 0.35f }, { subLevel, 0.25f }, { drift, 0.25f },
            { filtCutoff, 2600 }, { filtRes, 0.1f }, { filtEnv, 0.35f }, { filtKey, 0.5f }, { filtDrive, 0.3f },
            { ampA, 0.003f }, { ampD, 3.f }, { ampS, 0.3f }, { ampR, 1.f }, { fA, 0.003f }, { fD, 1.2f }, { fS, 0.2f },
            { lfoShape, 0 }, { lfoRate, 4.f }, { lfoFilter, 0.06f },
            { crushBits, 11 }, { crushRate, 20000 }, { lofiTone, 5500 }, { wow, 0.35f }, { wowRate, 0.5f }, { heat, 0.4f }, { crackle, 0.3f },
            { chorusMix, 0.45f }, { chorusRate, 0.8f }, { chorusDepth, 0.3f }, { delayMix, 0.1f },
            { verbMix, 0.35f }, { verbSize, 0.7f }, { verbDamp, 0.7f }, { master, -6.f } } },

        { "Bell Keys", "Keys", {
            { oscAWave, 5 }, { oscAUnison, 2 }, { oscADetune, 8 }, { oscALevel, 0.7f },
            { oscBWave, 3 }, { oscBOct, 1 }, { oscBSemi, 7 }, { oscBLevel, 0.2f }, { subLevel, 0.1f },
            { filtCutoff, 7000 }, { filtRes, 0.05f }, { filtEnv, 0.3f }, { filtKey, 0.6f },
            { ampA, 0.003f }, { ampD, 3.5f }, { ampS, 0.1f }, { ampR, 1.5f }, { fA, 0.003f }, { fD, 1.5f }, { fS, 0.1f },
            { crushBits, 12 }, { crushRate, 24000 }, { lofiTone, 11000 }, { wow, 0.2f }, { heat, 0.15f },
            { chorusMix, 0.5f }, { chorusDepth, 0.5f }, { delayMix, 0.3f }, { delayTime, 0.5f }, { delayFb, 0.5f },
            { verbMix, 0.5f }, { verbSize, 0.95f }, { verbDamp, 0.3f }, { width, 1.f }, { master, -7.f } } },

        { "Pluck Keys", "Keys", {
            { oscAWave, 0 }, { oscAUnison, 3 }, { oscADetune, 12 }, { oscALevel, 0.7f },
            { oscBWave, 1 }, { oscBOct, 0 }, { oscBLevel, 0.3f }, { subLevel, 0.2f },
            { filtCutoff, 900 }, { filtRes, 0.3f }, { filtEnv, 0.8f }, { filtKey, 0.5f },
            { ampA, 0.002f }, { ampD, 1.f }, { ampS, 0.1f }, { ampR, 0.5f }, { fA, 0.002f }, { fD, 0.25f }, { fS, 0.05f }, { fR, 0.3f },
            { crushBits, 11 }, { crushRate, 22050 }, { lofiTone, 8000 }, { wow, 0.15f }, { heat, 0.3f },
            { chorusMix, 0.3f }, { delayMix, 0.3f }, { delayTime, 0.375f }, { delayFb, 0.45f }, { verbMix, 0.35f }, { verbSize, 0.8f }, { master, -6.f } } },

        { "Toy Organ", "Keys", {
            { oscAWave, 1 }, { oscAUnison, 1 }, { oscALevel, 0.5f },
            { oscBWave, 1 }, { oscBOct, 1 }, { oscBLevel, 0.3f }, { subLevel, 0.3f }, { drift, 0.2f },
            { filtCutoff, 3000 }, { filtRes, 0.05f }, { filtEnv, 0.f }, { filtKey, 0.5f },
            { ampA, 0.01f }, { ampD, 0.5f }, { ampS, 1.f }, { ampR, 0.15f },
            { lfoRate, 5.f }, { lfoPitch, 0.03f },
            { crushBits, 9 }, { crushRate, 14000 }, { lofiTone, 5000 }, { wow, 0.4f }, { wowRate, 0.8f }, { heat, 0.2f }, { crackle, 0.2f },
            { chorusMix, 0.5f }, { chorusRate, 1.2f }, { chorusDepth, 0.4f }, { delayMix, 0.1f }, { verbMix, 0.3f }, { verbSize, 0.6f }, { master, -7.f } } },

        { "Vapor Keys", "Keys", {
            { oscAWave, 2 }, { oscAUnison, 3 }, { oscADetune, 14 }, { oscALevel, 0.7f },
            { oscBWave, 5 }, { oscBOct, 0 }, { oscBLevel, 0.4f }, { subLevel, 0.2f }, { drift, 0.4f },
            { filtCutoff, 3000 }, { filtRes, 0.15f }, { filtEnv, 0.4f }, { filtKey, 0.5f },
            { ampA, 0.005f }, { ampD, 2.5f }, { ampS, 0.4f }, { ampR, 1.5f }, { fA, 0.005f }, { fD, 1.f }, { fS, 0.3f },
            { lfoRate, 0.3f }, { lfoPitch, 0.08f },
            { crushBits, 10 }, { crushRate, 18000 }, { lofiTone, 7000 }, { wow, 0.5f }, { wowRate, 0.35f }, { heat, 0.2f }, { crackle, 0.15f },
            { chorusMix, 0.7f }, { chorusRate, 0.3f }, { chorusDepth, 0.7f }, { delayMix, 0.25f }, { delayTime, 0.5f },
            { verbMix, 0.5f }, { verbSize, 0.9f }, { width, 1.f }, { master, -7.f } } },

        // ====================== 808 ===========================================
        // Common 808 base: mono, sine body, pitch-env knock, no spatial FX, mono width.
        { "Ramzoid 808", "808", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.9f },
            { oscBWave, 2 }, { oscBOct, -1 }, { oscBLevel, 0.25f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.f },
            { voiceMode, 1 }, { glide, 0.08f }, { pitchEnvAmt, 14.f }, { pitchEnvDec, 0.045f },
            { filtType, 0 }, { filtCutoff, 1200 }, { filtRes, 0.1f }, { filtEnv, 0.4f }, { filtKey, 0.f }, { filtDrive, 0.6f },
            { ampA, 0.002f }, { ampD, 1.6f }, { ampS, 0.f }, { ampR, 0.25f }, { fA, 0.002f }, { fD, 0.3f }, { fS, 0.f }, { fR, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 3500 }, { wow, 0.f }, { heat, 0.7f }, { dist, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -3.f } } },

        { "Clean Sub 808", "808", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 1.f },
            { oscBOn, 0 }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.f },
            { voiceMode, 1 }, { glide, 0.06f }, { pitchEnvAmt, 10.f }, { pitchEnvDec, 0.035f },
            { filtCutoff, 600 }, { filtRes, 0.f }, { filtEnv, 0.2f }, { filtKey, 0.f }, { filtDrive, 0.2f },
            { ampA, 0.002f }, { ampD, 2.f }, { ampS, 0.f }, { ampR, 0.2f }, { fA, 0.002f }, { fD, 0.2f }, { fS, 0.f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 2500 }, { wow, 0.f }, { heat, 0.3f }, { dist, 0.f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -3.f } } },

        { "Distorted 808", "808", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.9f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBLevel, 0.2f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.f },
            { voiceMode, 1 }, { glide, 0.07f }, { pitchEnvAmt, 16.f }, { pitchEnvDec, 0.05f },
            { filtType, 0 }, { filtCutoff, 2500 }, { filtRes, 0.15f }, { filtEnv, 0.5f }, { filtKey, 0.f }, { filtDrive, 1.f },
            { ampA, 0.002f }, { ampD, 1.4f }, { ampS, 0.f }, { ampR, 0.2f }, { fA, 0.002f }, { fD, 0.4f }, { fS, 0.f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 12 }, { crushRate, 48000 }, { lofiTone, 5000 }, { wow, 0.f }, { heat, 1.f }, { dist, 0.8f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -4.f } } },

        { "Slide 808", "808", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.9f },
            { oscBWave, 2 }, { oscBOct, 0 }, { oscBLevel, 0.15f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.f },
            { voiceMode, 1 }, { glide, 0.25f }, { pitchEnvAmt, 8.f }, { pitchEnvDec, 0.04f },
            { filtCutoff, 1000 }, { filtRes, 0.1f }, { filtEnv, 0.3f }, { filtKey, 0.f }, { filtDrive, 0.5f },
            { ampA, 0.002f }, { ampD, 2.5f }, { ampS, 0.4f }, { ampR, 0.3f }, { fA, 0.002f }, { fD, 0.3f }, { fS, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 3000 }, { wow, 0.f }, { heat, 0.6f }, { dist, 0.25f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -3.f } } },

        { "Knock 808", "808", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.9f },
            { oscBWave, 4 }, { oscBOct, 0 }, { oscBLevel, 0.3f }, { subLevel, 0.f }, { noiseLevel, 0.03f }, { drift, 0.f },
            { voiceMode, 1 }, { glide, 0.05f }, { pitchEnvAmt, 24.f }, { pitchEnvDec, 0.03f },
            { filtCutoff, 1800 }, { filtRes, 0.2f }, { filtEnv, 0.7f }, { filtKey, 0.f }, { filtDrive, 0.7f },
            { ampA, 0.001f }, { ampD, 1.f }, { ampS, 0.f }, { ampR, 0.15f }, { fA, 0.001f }, { fD, 0.12f }, { fS, 0.f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 14 }, { crushRate, 48000 }, { lofiTone, 4000 }, { wow, 0.f }, { heat, 0.8f }, { dist, 0.5f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -3.f } } },

        { "Long Boomy 808", "808", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 1.f },
            { oscBWave, 3 }, { oscBOct, -2 }, { oscBLevel, 0.3f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.f },
            { voiceMode, 1 }, { glide, 0.1f }, { pitchEnvAmt, 12.f }, { pitchEnvDec, 0.06f },
            { filtCutoff, 800 }, { filtRes, 0.1f }, { filtEnv, 0.3f }, { filtKey, 0.f }, { filtDrive, 0.5f },
            { ampA, 0.002f }, { ampD, 4.f }, { ampS, 0.1f }, { ampR, 0.4f }, { fA, 0.002f }, { fD, 0.4f }, { fS, 0.f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 3000 }, { wow, 0.f }, { heat, 0.6f }, { dist, 0.2f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -3.f } } },

        { "Lo-Fi 808", "808", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.9f },
            { oscBWave, 2 }, { oscBOct, -1 }, { oscBLevel, 0.3f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.1f },
            { voiceMode, 1 }, { glide, 0.08f }, { pitchEnvAmt, 12.f }, { pitchEnvDec, 0.05f },
            { filtCutoff, 900 }, { filtRes, 0.1f }, { filtEnv, 0.3f }, { filtKey, 0.f }, { filtDrive, 0.5f },
            { ampA, 0.002f }, { ampD, 1.5f }, { ampS, 0.f }, { ampR, 0.25f }, { fA, 0.002f }, { fD, 0.3f }, { fS, 0.f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 8 }, { crushRate, 11025 }, { lofiTone, 2500 }, { wow, 0.1f }, { heat, 0.7f }, { dist, 0.4f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -3.f } } },

        { "Trap Growl 808", "808", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.9f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBUnison, 2 }, { oscBDetune, 6 }, { oscBLevel, 0.3f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.f },
            { voiceMode, 1 }, { glide, 0.08f }, { pitchEnvAmt, 14.f }, { pitchEnvDec, 0.045f },
            { filtType, 1 }, { filtCutoff, 700 }, { filtRes, 0.35f }, { filtEnv, 0.6f }, { filtKey, 0.f }, { filtDrive, 0.9f },
            { ampA, 0.002f }, { ampD, 1.8f }, { ampS, 0.f }, { ampR, 0.25f }, { fA, 0.002f }, { fD, 0.6f }, { fS, 0.1f },
            { lfoShape, 0 }, { lfoRate, 6.f }, { lfoPitch, 0.f }, { lfoFilter, 0.1f },
            { crushBits, 14 }, { crushRate, 48000 }, { lofiTone, 4500 }, { wow, 0.f }, { heat, 0.9f }, { dist, 0.7f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -4.f } } },

        // ====================== BASS ==========================================
        { "Dark Reese", "Bass", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 4 }, { oscADetune, 18 }, { oscALevel, 0.8f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBUnison, 3 }, { oscBDetune, 10 }, { oscBLevel, 0.5f }, { subLevel, 0.6f }, { drift, 0.2f },
            { voiceMode, 1 }, { glide, 0.03f },
            { filtType, 1 }, { filtCutoff, 500 }, { filtRes, 0.2f }, { filtEnv, 0.3f }, { filtKey, 0.3f }, { filtDrive, 0.5f },
            { ampA, 0.01f }, { ampD, 1.f }, { ampS, 0.9f }, { ampR, 0.3f }, { fA, 0.01f }, { fD, 1.f }, { fS, 0.3f },
            { lfoRate, 0.3f }, { lfoPitch, 0.f }, { lfoFilter, 0.15f },
            { crushBits, 14 }, { crushRate, 40000 }, { lofiTone, 4000 }, { wow, 0.1f }, { heat, 0.5f }, { dist, 0.15f }, { crackle, 0.f },
            { chorusMix, 0.25f }, { chorusRate, 0.2f }, { chorusDepth, 0.4f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.5f }, { master, -5.f } } },

        { "Sub Bass", "Bass", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 1.f },
            { oscBOn, 0 }, { subLevel, 0.3f }, { noiseLevel, 0.f }, { drift, 0.f },
            { voiceMode, 1 }, { glide, 0.04f },
            { filtCutoff, 400 }, { filtRes, 0.f }, { filtEnv, 0.f }, { filtKey, 0.f }, { filtDrive, 0.2f },
            { ampA, 0.005f }, { ampD, 0.5f }, { ampS, 1.f }, { ampR, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 2000 }, { wow, 0.f }, { heat, 0.3f }, { dist, 0.f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -3.f } } },

        { "Square Bass", "Bass", {
            { oscAWave, 1 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.7f },
            { oscBWave, 3 }, { oscBOct, -2 }, { oscBLevel, 0.5f }, { subLevel, 0.2f }, { drift, 0.1f },
            { voiceMode, 1 }, { glide, 0.05f },
            { filtCutoff, 700 }, { filtRes, 0.3f }, { filtEnv, 0.6f }, { filtKey, 0.3f }, { filtDrive, 0.4f },
            { ampA, 0.003f }, { ampD, 0.6f }, { ampS, 0.6f }, { ampR, 0.15f }, { fA, 0.003f }, { fD, 0.25f }, { fS, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 11 }, { crushRate, 22050 }, { lofiTone, 5000 }, { wow, 0.1f }, { heat, 0.5f }, { dist, 0.2f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -5.f } } },

        { "Pluck Bass", "Bass", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 2 }, { oscADetune, 8 }, { oscALevel, 0.8f },
            { oscBWave, 1 }, { oscBOct, -1 }, { oscBLevel, 0.3f }, { subLevel, 0.5f }, { drift, 0.1f },
            { voiceMode, 1 }, { glide, 0.02f },
            { filtCutoff, 300 }, { filtRes, 0.35f }, { filtEnv, 1.f }, { filtKey, 0.4f }, { filtDrive, 0.5f },
            { ampA, 0.002f }, { ampD, 0.8f }, { ampS, 0.2f }, { ampR, 0.2f }, { fA, 0.002f }, { fD, 0.18f }, { fS, 0.f }, { fR, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 12 }, { crushRate, 32000 }, { lofiTone, 6000 }, { wow, 0.f }, { heat, 0.5f }, { dist, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.1f }, { delayTime, 0.375f }, { verbMix, 0.05f }, { width, 0.2f }, { master, -5.f } } },

        { "Moog-ish Bass", "Bass", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBFine, 8 }, { oscBLevel, 0.6f }, { subLevel, 0.4f }, { drift, 0.3f },
            { voiceMode, 1 }, { glide, 0.06f },
            { filtType, 1 }, { filtCutoff, 450 }, { filtRes, 0.45f }, { filtEnv, 0.7f }, { filtKey, 0.3f }, { filtDrive, 0.6f },
            { ampA, 0.003f }, { ampD, 0.5f }, { ampS, 0.7f }, { ampR, 0.15f }, { fA, 0.003f }, { fD, 0.35f }, { fS, 0.2f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 5000 }, { wow, 0.05f }, { heat, 0.6f }, { dist, 0.2f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -5.f } } },

        { "Wobble Bass", "Bass", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 3 }, { oscADetune, 12 }, { oscALevel, 0.8f },
            { oscBWave, 1 }, { oscBOct, -2 }, { oscBLevel, 0.4f }, { subLevel, 0.5f }, { drift, 0.1f },
            { voiceMode, 1 }, { glide, 0.03f },
            { filtType, 1 }, { filtCutoff, 350 }, { filtRes, 0.4f }, { filtEnv, 0.2f }, { filtKey, 0.2f }, { filtDrive, 0.7f },
            { ampA, 0.005f }, { ampD, 0.5f }, { ampS, 1.f }, { ampR, 0.15f },
            { lfoShape, 0 }, { lfoRate, 2.f }, { lfoPitch, 0.f }, { lfoFilter, 0.7f },
            { crushBits, 13 }, { crushRate, 32000 }, { lofiTone, 6000 }, { wow, 0.f }, { heat, 0.6f }, { dist, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.3f }, { master, -5.f } } },

        // ------------------------------------------------------------------
        // VINTAGE COLLECTION - modelled on the Arturia V Collection 5 lineup
        // (Mini, ARP 2600, CS-80, Jup-8, Prophet 5/VS, Matrix-12, SEM, Modular,
        //  Solina, Synclavier, Stage-73, Wurli, Piano V, B-3, VOX, Farfisa).
        // Mostly clean (16-bit / 48k, no crackle) with analog drift + heat.
        // ------------------------------------------------------------------

        // ---- Pads ----
        { "Solina Strings", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 5 }, { oscADetune, 22 }, { oscALevel, 0.7f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBUnison, 3 }, { oscBDetune, 14 }, { oscBFine, 5 }, { oscBLevel, 0.45f },
            { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.35f },
            { filtType, 0 }, { filtCutoff, 5200 }, { filtRes, 0.05f }, { filtEnv, 0.15f }, { filtKey, 0.4f }, { filtDrive, 0.1f },
            { ampA, 0.45f }, { ampD, 1.5f }, { ampS, 0.9f }, { ampR, 1.4f }, { fA, 0.3f }, { fD, 2.f }, { fS, 0.6f },
            { lfoRate, 0.5f }, { lfoPitch, 0.03f }, { lfoFilter, 0.05f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 11000 }, { wow, 0.08f }, { heat, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.9f }, { chorusRate, 0.9f }, { chorusDepth, 0.7f }, { delayMix, 0.08f }, { verbMix, 0.35f }, { verbSize, 0.8f }, { width, 0.9f }, { master, -7.f } } },

        { "Solina Dusty Tape", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 4 }, { oscADetune, 20 }, { oscALevel, 0.7f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBUnison, 2 }, { oscBDetune, 10 }, { oscBLevel, 0.4f },
            { subLevel, 0.f }, { noiseLevel, 0.02f }, { drift, 0.4f },
            { filtCutoff, 3200 }, { filtRes, 0.1f }, { filtEnv, 0.1f }, { filtKey, 0.3f },
            { ampA, 0.6f }, { ampD, 1.5f }, { ampS, 0.9f }, { ampR, 1.8f },
            { lfoRate, 0.4f }, { lfoPitch, 0.04f }, { lfoFilter, 0.08f },
            { crushBits, 11 }, { crushRate, 20000 }, { lofiTone, 6500 }, { wow, 0.45f }, { wowRate, 0.5f }, { heat, 0.35f }, { crackle, 0.2f },
            { chorusMix, 0.85f }, { chorusRate, 0.8f }, { chorusDepth, 0.65f }, { delayMix, 0.12f }, { verbMix, 0.4f }, { verbSize, 0.85f }, { width, 0.85f }, { master, -7.f } } },

        { "CS-80 Blade Runner", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 2 }, { oscADetune, 9 }, { oscALevel, 0.75f },
            { oscBWave, 4 }, { oscBOct, 0 }, { oscBFine, 7 }, { oscBLevel, 0.5f },
            { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { filtType, 0 }, { filtCutoff, 1500 }, { filtRes, 0.25f }, { filtEnv, 0.45f }, { filtKey, 0.3f }, { filtDrive, 0.2f },
            { ampA, 0.55f }, { ampD, 2.f }, { ampS, 0.9f }, { ampR, 2.2f }, { fA, 1.5f }, { fD, 3.f }, { fS, 0.5f }, { fR, 2.f },
            { lfoShape, 0 }, { lfoRate, 5.2f }, { lfoPitch, 0.035f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 12000 }, { wow, 0.05f }, { heat, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { chorusRate, 0.3f }, { chorusDepth, 0.4f }, { delayMix, 0.3f }, { delayTime, 0.5f }, { delayFb, 0.45f },
            { verbMix, 0.55f }, { verbSize, 0.92f }, { verbDamp, 0.35f }, { width, 0.9f }, { master, -7.f } } },

        { "CS-80 Ribbon Pad", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 3 }, { oscADetune, 14 }, { oscALevel, 0.7f },
            { oscBWave, 1 }, { oscBOct, -1 }, { oscBLevel, 0.4f },
            { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.35f },
            { filtType, 0 }, { filtCutoff, 1100 }, { filtRes, 0.3f }, { filtEnv, 0.5f }, { filtKey, 0.3f },
            { ampA, 0.35f }, { ampD, 2.f }, { ampS, 0.85f }, { ampR, 1.8f }, { fA, 0.9f }, { fD, 2.5f }, { fS, 0.4f },
            { lfoShape, 1 }, { lfoRate, 0.18f }, { lfoPitch, 0.02f }, { lfoFilter, 0.3f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 10000 }, { wow, 0.08f }, { heat, 0.4f }, { crackle, 0.f },
            { chorusMix, 0.35f }, { delayMix, 0.2f }, { delayTime, 0.75f }, { delayFb, 0.4f }, { verbMix, 0.5f }, { verbSize, 0.9f }, { width, 0.85f }, { master, -7.f } } },

        { "Jup-8 Sweep", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 4 }, { oscADetune, 16 }, { oscALevel, 0.7f },
            { oscBWave, 1 }, { oscBOct, -1 }, { oscBUnison, 2 }, { oscBDetune, 8 }, { oscBLevel, 0.4f },
            { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { filtType, 1 }, { filtCutoff, 700 }, { filtRes, 0.35f }, { filtEnv, 0.75f }, { filtKey, 0.2f }, { filtDrive, 0.15f },
            { ampA, 0.3f }, { ampD, 2.f }, { ampS, 0.9f }, { ampR, 2.f }, { fA, 2.5f }, { fD, 3.f }, { fS, 0.5f }, { fR, 2.5f },
            { lfoRate, 0.3f }, { lfoPitch, 0.02f }, { lfoFilter, 0.1f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 12000 }, { wow, 0.05f }, { heat, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.6f }, { chorusRate, 0.4f }, { chorusDepth, 0.5f }, { delayMix, 0.15f }, { verbMix, 0.45f }, { verbSize, 0.85f }, { width, 0.9f }, { master, -7.f } } },

        { "Jup-8 Hollow Pad", "Pads", {
            { oscAWave, 4 }, { oscAUnison, 3 }, { oscADetune, 12 }, { oscALevel, 0.65f },
            { oscBWave, 2 }, { oscBOct, 0 }, { oscBFine, -6 }, { oscBLevel, 0.5f },
            { subLevel, 0.2f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { filtType, 0 }, { filtCutoff, 2400 }, { filtRes, 0.15f }, { filtEnv, 0.25f }, { filtKey, 0.4f },
            { ampA, 0.5f }, { ampD, 2.f }, { ampS, 0.85f }, { ampR, 2.f }, { fA, 0.8f }, { fD, 2.f }, { fS, 0.5f },
            { lfoShape, 1 }, { lfoRate, 0.35f }, { lfoPitch, 0.03f }, { lfoFilter, 0.15f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 10000 }, { wow, 0.06f }, { heat, 0.25f }, { crackle, 0.f },
            { chorusMix, 0.7f }, { chorusRate, 0.5f }, { chorusDepth, 0.55f }, { delayMix, 0.15f }, { verbMix, 0.45f }, { verbSize, 0.85f }, { width, 0.9f }, { master, -7.f } } },

        { "Prophet 5 Poly Pad", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 2 }, { oscADetune, 10 }, { oscALevel, 0.7f },
            { oscBWave, 1 }, { oscBOct, 0 }, { oscBFine, 9 }, { oscBLevel, 0.5f },
            { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.4f },
            { filtType, 1 }, { filtCutoff, 1900 }, { filtRes, 0.2f }, { filtEnv, 0.35f }, { filtKey, 0.3f }, { filtDrive, 0.2f },
            { ampA, 0.2f }, { ampD, 1.5f }, { ampS, 0.85f }, { ampR, 1.2f }, { fA, 0.4f }, { fD, 1.5f }, { fS, 0.4f },
            { lfoRate, 0.4f }, { lfoPitch, 0.02f }, { lfoFilter, 0.08f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 11000 }, { wow, 0.05f }, { heat, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.35f }, { chorusRate, 0.35f }, { chorusDepth, 0.4f }, { delayMix, 0.1f }, { verbMix, 0.35f }, { verbSize, 0.8f }, { width, 0.8f }, { master, -7.f } } },

        { "Prophet VS Vector", "Pads", {
            { oscAWave, 5 }, { oscAUnison, 3 }, { oscADetune, 14 }, { oscALevel, 0.6f },
            { oscBWave, 4 }, { oscBOct, 0 }, { oscBUnison, 2 }, { oscBDetune, 10 }, { oscBLevel, 0.4f },
            { subLevel, 0.1f }, { noiseLevel, 0.01f }, { drift, 0.15f },
            { filtType, 0 }, { filtCutoff, 4000 }, { filtRes, 0.1f }, { filtEnv, 0.3f }, { filtKey, 0.4f },
            { ampA, 0.4f }, { ampD, 2.f }, { ampS, 0.85f }, { ampR, 2.f }, { fA, 1.2f }, { fD, 2.5f }, { fS, 0.5f },
            { lfoShape, 1 }, { lfoRate, 0.25f }, { lfoPitch, 0.03f }, { lfoFilter, 0.25f },
            { crushBits, 12 }, { crushRate, 32000 }, { lofiTone, 12000 }, { wow, 0.08f }, { heat, 0.2f }, { crackle, 0.f },
            { chorusMix, 0.5f }, { chorusRate, 0.3f }, { chorusDepth, 0.5f }, { delayMix, 0.2f }, { delayTime, 0.5f }, { delayFb, 0.4f },
            { verbMix, 0.5f }, { verbSize, 0.9f }, { width, 1.f }, { master, -7.f } } },

        { "Matrix-12 Drift", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 3 }, { oscADetune, 12 }, { oscALevel, 0.65f },
            { oscBWave, 4 }, { oscBOct, -1 }, { oscBFine, 5 }, { oscBLevel, 0.5f },
            { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.5f },
            { filtType, 1 }, { filtCutoff, 1300 }, { filtRes, 0.3f }, { filtEnv, 0.4f }, { filtKey, 0.3f }, { filtDrive, 0.25f },
            { ampA, 0.4f }, { ampD, 2.f }, { ampS, 0.85f }, { ampR, 2.f }, { fA, 0.6f }, { fD, 2.f }, { fS, 0.5f },
            { lfoShape, 4 }, { lfoRate, 0.6f }, { lfoPitch, 0.f }, { lfoFilter, 0.3f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 10000 }, { wow, 0.05f }, { heat, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.45f }, { chorusRate, 0.4f }, { chorusDepth, 0.5f }, { delayMix, 0.15f }, { verbMix, 0.45f }, { verbSize, 0.85f }, { width, 0.9f }, { master, -7.f } } },

        { "Oberheim Brass Pad", "Pads", {
            { oscAWave, 0 }, { oscAUnison, 3 }, { oscADetune, 15 }, { oscALevel, 0.75f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBUnison, 2 }, { oscBDetune, 10 }, { oscBFine, -8 }, { oscBLevel, 0.5f },
            { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.35f },
            { filtType, 1 }, { filtCutoff, 1000 }, { filtRes, 0.2f }, { filtEnv, 0.65f }, { filtKey, 0.3f }, { filtDrive, 0.3f },
            { ampA, 0.06f }, { ampD, 1.2f }, { ampS, 0.85f }, { ampR, 0.8f }, { fA, 0.06f }, { fD, 0.9f }, { fS, 0.35f }, { fR, 0.6f },
            { lfoRate, 0.3f }, { lfoPitch, 0.01f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 11000 }, { wow, 0.04f }, { heat, 0.45f }, { crackle, 0.f },
            { chorusMix, 0.4f }, { chorusRate, 0.35f }, { chorusDepth, 0.45f }, { delayMix, 0.1f }, { verbMix, 0.3f }, { verbSize, 0.75f }, { width, 0.85f }, { master, -7.f } } },

        { "SEM Warm Pad", "Pads", {
            { oscAWave, 2 }, { oscAUnison, 2 }, { oscADetune, 8 }, { oscALevel, 0.75f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBFine, 6 }, { oscBLevel, 0.4f },
            { subLevel, 0.2f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { filtType, 0 }, { filtCutoff, 1700 }, { filtRes, 0.1f }, { filtEnv, 0.3f }, { filtKey, 0.4f }, { filtDrive, 0.15f },
            { ampA, 0.35f }, { ampD, 1.5f }, { ampS, 0.9f }, { ampR, 1.5f }, { fA, 0.5f }, { fD, 2.f }, { fS, 0.5f },
            { lfoRate, 0.3f }, { lfoPitch, 0.02f }, { lfoFilter, 0.1f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 9000 }, { wow, 0.08f }, { heat, 0.4f }, { crackle, 0.f },
            { chorusMix, 0.4f }, { chorusRate, 0.3f }, { chorusDepth, 0.4f }, { delayMix, 0.1f }, { verbMix, 0.35f }, { verbSize, 0.8f }, { width, 0.8f }, { master, -7.f } } },

        { "Modular Drone", "Pads", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 2 }, { oscADetune, 6 }, { oscALevel, 0.7f },
            { oscBWave, 1 }, { oscBOct, -1 }, { oscBFine, 4 }, { oscBLevel, 0.5f },
            { subLevel, 0.3f }, { noiseLevel, 0.02f }, { drift, 0.4f },
            { filtType, 1 }, { filtCutoff, 600 }, { filtRes, 0.5f }, { filtEnv, 0.3f }, { filtKey, 0.1f }, { filtDrive, 0.4f },
            { ampA, 1.2f }, { ampD, 3.f }, { ampS, 0.9f }, { ampR, 3.f }, { fA, 2.f }, { fD, 4.f }, { fS, 0.5f },
            { lfoShape, 1 }, { lfoRate, 0.12f }, { lfoPitch, 0.01f }, { lfoFilter, 0.45f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 8000 }, { wow, 0.08f }, { heat, 0.5f }, { dist, 0.05f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { chorusRate, 0.2f }, { chorusDepth, 0.5f }, { delayMix, 0.2f }, { delayTime, 0.75f }, { delayFb, 0.5f },
            { verbMix, 0.5f }, { verbSize, 0.95f }, { width, 0.9f }, { master, -8.f } } },

        { "Synclavier Choir", "Pads", {
            { oscAWave, 5 }, { oscAUnison, 5 }, { oscADetune, 18 }, { oscALevel, 0.6f },
            { oscBWave, 3 }, { oscBOct, 0 }, { oscBUnison, 3 }, { oscBDetune, 12 }, { oscBLevel, 0.5f },
            { subLevel, 0.1f }, { noiseLevel, 0.02f }, { drift, 0.2f },
            { filtType, 0 }, { filtCutoff, 2600 }, { filtRes, 0.15f }, { filtEnv, 0.3f }, { filtKey, 0.5f },
            { ampA, 0.8f }, { ampD, 2.f }, { ampS, 0.9f }, { ampR, 2.5f }, { fA, 1.f }, { fD, 2.f }, { fS, 0.5f },
            { lfoRate, 0.3f }, { lfoPitch, 0.03f }, { lfoFilter, 0.1f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 10000 }, { wow, 0.1f }, { heat, 0.2f }, { crackle, 0.f },
            { chorusMix, 0.6f }, { chorusRate, 0.25f }, { chorusDepth, 0.5f }, { delayMix, 0.15f }, { verbMix, 0.6f }, { verbSize, 0.95f }, { verbDamp, 0.3f }, { width, 1.f }, { master, -7.f } } },

        { "Synclavier Glass Pad", "Pads", {
            { oscAWave, 5 }, { oscAUnison, 2 }, { oscADetune, 8 }, { oscALevel, 0.6f },
            { oscBWave, 5 }, { oscBOct, 1 }, { oscBLevel, 0.3f },
            { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.1f },
            { filtType, 0 }, { filtCutoff, 6000 }, { filtRes, 0.05f }, { filtEnv, 0.2f }, { filtKey, 0.6f },
            { ampA, 0.3f }, { ampD, 3.f }, { ampS, 0.7f }, { ampR, 2.5f }, { fA, 0.2f }, { fD, 3.f }, { fS, 0.4f },
            { lfoRate, 4.5f }, { lfoPitch, 0.012f }, { lfoFilter, 0.f },
            { crushBits, 12 }, { crushRate, 32000 }, { lofiTone, 13000 }, { wow, 0.1f }, { heat, 0.15f }, { crackle, 0.f },
            { chorusMix, 0.45f }, { chorusRate, 0.4f }, { chorusDepth, 0.45f }, { delayMix, 0.3f }, { delayTime, 0.375f }, { delayFb, 0.5f },
            { verbMix, 0.5f }, { verbSize, 0.9f }, { width, 1.f }, { master, -7.f } } },

        { "ARP Sci-Fi Pad", "Pads", {
            { oscAWave, 1 }, { oscAUnison, 2 }, { oscADetune, 10 }, { oscALevel, 0.65f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBFine, 7 }, { oscBLevel, 0.45f },
            { subLevel, 0.1f }, { noiseLevel, 0.03f }, { drift, 0.35f },
            { filtType, 2 }, { filtCutoff, 1200 }, { filtRes, 0.45f }, { filtEnv, 0.4f }, { filtKey, 0.3f }, { filtDrive, 0.2f },
            { ampA, 0.5f }, { ampD, 2.f }, { ampS, 0.85f }, { ampR, 2.f }, { fA, 1.f }, { fD, 3.f }, { fS, 0.5f },
            { lfoShape, 0 }, { lfoRate, 0.22f }, { lfoPitch, 0.02f }, { lfoFilter, 0.5f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 10000 }, { wow, 0.08f }, { heat, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.4f }, { chorusRate, 0.3f }, { chorusDepth, 0.5f }, { delayMix, 0.25f }, { delayTime, 0.5f }, { delayFb, 0.5f },
            { verbMix, 0.5f }, { verbSize, 0.9f }, { width, 0.95f }, { master, -7.f } } },

        // ---- Keys ----
        { "Stage-73 Rhodes", "Keys", {
            { oscAWave, 3 }, { oscAUnison, 1 }, { oscALevel, 0.85f },
            { oscBWave, 5 }, { oscBOct, 1 }, { oscBLevel, 0.2f },
            { subLevel, 0.12f }, { noiseLevel, 0.f }, { drift, 0.12f },
            { filtType, 0 }, { filtCutoff, 3800 }, { filtRes, 0.05f }, { filtEnv, 0.5f }, { filtKey, 0.7f }, { filtDrive, 0.15f },
            { ampA, 0.002f }, { ampD, 3.f }, { ampS, 0.3f }, { ampR, 0.7f }, { fA, 0.002f }, { fD, 0.6f }, { fS, 0.1f }, { fR, 0.5f },
            { lfoRate, 0.4f }, { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 10000 }, { wow, 0.05f }, { heat, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.2f }, { chorusRate, 0.5f }, { chorusDepth, 0.3f }, { delayMix, 0.1f }, { verbMix, 0.3f }, { verbSize, 0.75f }, { width, 0.7f }, { master, -6.f } } },

        { "Suitcase Tremolo", "Keys", {
            { oscAWave, 3 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 5 }, { oscBOct, 1 }, { oscBLevel, 0.25f },
            { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.15f },
            { filtType, 0 }, { filtCutoff, 3200 }, { filtRes, 0.05f }, { filtEnv, 0.5f }, { filtKey, 0.7f },
            { ampA, 0.002f }, { ampD, 3.f }, { ampS, 0.3f }, { ampR, 0.8f }, { fA, 0.002f }, { fD, 0.6f }, { fS, 0.1f }, { fR, 0.5f },
            { lfoShape, 0 }, { lfoRate, 4.2f }, { lfoPitch, 0.f }, { lfoFilter, 0.25f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 9000 }, { wow, 0.05f }, { heat, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.55f }, { chorusRate, 4.2f }, { chorusDepth, 0.5f }, { delayMix, 0.08f }, { verbMix, 0.3f }, { verbSize, 0.75f }, { width, 0.9f }, { master, -6.f } } },

        { "Wurli Bark", "Keys", {
            { oscAWave, 2 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 1 }, { oscBOct, 0 }, { oscBLevel, 0.3f },
            { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.1f },
            { filtType, 0 }, { filtCutoff, 1600 }, { filtRes, 0.15f }, { filtEnv, 0.7f }, { filtKey, 0.6f }, { filtDrive, 0.4f },
            { ampA, 0.002f }, { ampD, 2.f }, { ampS, 0.3f }, { ampR, 0.5f }, { fA, 0.002f }, { fD, 0.35f }, { fS, 0.1f }, { fR, 0.4f },
            { lfoRate, 5.f }, { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 8000 }, { wow, 0.05f }, { heat, 0.45f }, { dist, 0.05f }, { crackle, 0.f },
            { chorusMix, 0.15f }, { delayMix, 0.08f }, { verbMix, 0.25f }, { verbSize, 0.7f }, { width, 0.6f }, { master, -6.f } } },

        { "Wurli Soft Trem", "Keys", {
            { oscAWave, 2 }, { oscAUnison, 2 }, { oscADetune, 4 }, { oscALevel, 0.8f },
            { oscBWave, 3 }, { oscBOct, 1 }, { oscBLevel, 0.2f },
            { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.15f },
            { filtType, 0 }, { filtCutoff, 1900 }, { filtRes, 0.1f }, { filtEnv, 0.5f }, { filtKey, 0.6f }, { filtDrive, 0.3f },
            { ampA, 0.003f }, { ampD, 2.2f }, { ampS, 0.35f }, { ampR, 0.6f }, { fA, 0.003f }, { fD, 0.5f }, { fS, 0.15f }, { fR, 0.4f },
            { lfoShape, 0 }, { lfoRate, 5.5f }, { lfoPitch, 0.f }, { lfoFilter, 0.2f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 8000 }, { wow, 0.08f }, { heat, 0.4f }, { crackle, 0.05f },
            { chorusMix, 0.25f }, { chorusRate, 0.4f }, { chorusDepth, 0.3f }, { delayMix, 0.1f }, { verbMix, 0.3f }, { verbSize, 0.75f }, { width, 0.7f }, { master, -6.f } } },

        { "Piano V Felt", "Keys", {
            { oscAWave, 2 }, { oscAUnison, 2 }, { oscADetune, 5 }, { oscALevel, 0.75f },
            { oscBWave, 3 }, { oscBOct, 1 }, { oscBLevel, 0.3f },
            { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.05f },
            { filtType, 0 }, { filtCutoff, 2800 }, { filtRes, 0.05f }, { filtEnv, 0.5f }, { filtKey, 0.8f }, { filtDrive, 0.1f },
            { ampA, 0.002f }, { ampD, 2.5f }, { ampS, 0.2f }, { ampR, 0.6f }, { fA, 0.002f }, { fD, 0.7f }, { fS, 0.1f }, { fR, 0.5f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 6500 }, { wow, 0.06f }, { heat, 0.2f }, { crackle, 0.05f },
            { chorusMix, 0.1f }, { delayMix, 0.08f }, { verbMix, 0.35f }, { verbSize, 0.8f }, { verbDamp, 0.6f }, { width, 0.7f }, { master, -6.f } } },

        { "Piano V Bright", "Keys", {
            { oscAWave, 0 }, { oscAUnison, 2 }, { oscADetune, 4 }, { oscALevel, 0.6f },
            { oscBWave, 2 }, { oscBOct, 1 }, { oscBLevel, 0.4f },
            { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.05f },
            { filtType, 0 }, { filtCutoff, 3200 }, { filtRes, 0.05f }, { filtEnv, 0.7f }, { filtKey, 0.8f }, { filtDrive, 0.1f },
            { ampA, 0.002f }, { ampD, 2.2f }, { ampS, 0.15f }, { ampR, 0.5f }, { fA, 0.002f }, { fD, 0.4f }, { fS, 0.05f }, { fR, 0.4f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 11000 }, { wow, 0.03f }, { heat, 0.15f }, { crackle, 0.f },
            { chorusMix, 0.05f }, { delayMix, 0.05f }, { verbMix, 0.3f }, { verbSize, 0.8f }, { width, 0.7f }, { master, -6.f } } },

        { "B-3 Drawbars", "Keys", {
            { oscAWave, 3 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 3 }, { oscBOct, 1 }, { oscBLevel, 0.45f },
            { subLevel, 0.5f }, { noiseLevel, 0.f }, { drift, 0.02f },
            { filtType, 0 }, { filtCutoff, 6000 }, { filtRes, 0.f }, { filtEnv, 0.f }, { filtKey, 0.5f }, { filtDrive, 0.2f },
            { ampA, 0.004f }, { ampD, 1.f }, { ampS, 1.f }, { ampR, 0.05f }, { fA, 0.004f }, { fS, 1.f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 8000 }, { wow, 0.02f }, { heat, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.45f }, { chorusRate, 1.1f }, { chorusDepth, 0.4f }, { delayMix, 0.f }, { verbMix, 0.25f }, { verbSize, 0.7f }, { width, 0.8f }, { master, -7.f } } },

        { "B-3 Full Leslie", "Keys", {
            { oscAWave, 1 }, { oscAUnison, 1 }, { oscALevel, 0.6f },
            { oscBWave, 4 }, { oscBOct, 1 }, { oscBLevel, 0.4f },
            { subLevel, 0.5f }, { noiseLevel, 0.f }, { drift, 0.02f },
            { filtType, 0 }, { filtCutoff, 4500 }, { filtRes, 0.05f }, { filtEnv, 0.f }, { filtKey, 0.4f }, { filtDrive, 0.3f },
            { ampA, 0.004f }, { ampD, 1.f }, { ampS, 1.f }, { ampR, 0.06f }, { fA, 0.004f }, { fS, 1.f },
            { lfoShape, 0 }, { lfoRate, 6.5f }, { lfoPitch, 0.01f }, { lfoFilter, 0.15f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 7000 }, { wow, 0.02f }, { heat, 0.55f }, { dist, 0.12f }, { crackle, 0.f },
            { chorusMix, 0.7f }, { chorusRate, 5.f }, { chorusDepth, 0.6f }, { delayMix, 0.f }, { verbMix, 0.25f }, { verbSize, 0.7f }, { width, 1.f }, { master, -7.f } } },

        { "Farfisa Combo", "Keys", {
            { oscAWave, 4 }, { oscAUnison, 1 }, { oscALevel, 0.6f },
            { oscBWave, 1 }, { oscBOct, 1 }, { oscBLevel, 0.3f },
            { subLevel, 0.2f }, { noiseLevel, 0.f }, { drift, 0.03f },
            { filtType, 0 }, { filtCutoff, 5000 }, { filtRes, 0.1f }, { filtEnv, 0.f }, { filtKey, 0.5f }, { filtDrive, 0.2f },
            { ampA, 0.003f }, { ampD, 1.f }, { ampS, 1.f }, { ampR, 0.05f }, { fA, 0.003f }, { fS, 1.f },
            { lfoShape, 0 }, { lfoRate, 5.8f }, { lfoPitch, 0.01f }, { lfoFilter, 0.f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 7500 }, { wow, 0.04f }, { heat, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.2f }, { chorusRate, 0.6f }, { chorusDepth, 0.3f }, { delayMix, 0.f }, { verbMix, 0.3f }, { verbSize, 0.7f }, { width, 0.7f }, { master, -8.f } } },

        { "VOX Continental", "Keys", {
            { oscAWave, 4 }, { oscAUnison, 1 }, { oscALevel, 0.55f },
            { oscBWave, 3 }, { oscBOct, 1 }, { oscBLevel, 0.45f },
            { subLevel, 0.3f }, { noiseLevel, 0.f }, { drift, 0.03f },
            { filtType, 0 }, { filtCutoff, 6500 }, { filtRes, 0.f }, { filtEnv, 0.f }, { filtKey, 0.5f }, { filtDrive, 0.15f },
            { ampA, 0.003f }, { ampD, 1.f }, { ampS, 1.f }, { ampR, 0.05f }, { fA, 0.003f }, { fS, 1.f },
            { lfoRate, 6.f }, { lfoPitch, 0.012f }, { lfoFilter, 0.f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 8000 }, { wow, 0.04f }, { heat, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.15f }, { delayMix, 0.f }, { verbMix, 0.3f }, { verbSize, 0.75f }, { width, 0.6f }, { master, -8.f } } },

        { "Synclavier Bells", "Keys", {
            { oscAWave, 5 }, { oscAOct, 1 }, { oscAUnison, 1 }, { oscALevel, 0.7f },
            { oscBWave, 3 }, { oscBOct, 2 }, { oscBFine, 4 }, { oscBLevel, 0.35f },
            { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.05f },
            { filtType, 0 }, { filtCutoff, 9000 }, { filtRes, 0.f }, { filtEnv, 0.3f }, { filtKey, 0.8f },
            { ampA, 0.002f }, { ampD, 2.5f }, { ampS, 0.f }, { ampR, 1.5f }, { fA, 0.002f }, { fD, 0.4f }, { fS, 0.f }, { fR, 0.5f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 12 }, { crushRate, 32000 }, { lofiTone, 13000 }, { wow, 0.05f }, { heat, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { chorusRate, 0.5f }, { chorusDepth, 0.35f }, { delayMix, 0.3f }, { delayTime, 0.375f }, { delayFb, 0.5f },
            { verbMix, 0.45f }, { verbSize, 0.9f }, { width, 1.f }, { master, -7.f } } },

        { "VS Digital Keys", "Keys", {
            { oscAWave, 5 }, { oscAUnison, 2 }, { oscADetune, 6 }, { oscALevel, 0.7f },
            { oscBWave, 4 }, { oscBOct, 0 }, { oscBLevel, 0.3f },
            { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.08f },
            { filtType, 0 }, { filtCutoff, 3000 }, { filtRes, 0.1f }, { filtEnv, 0.6f }, { filtKey, 0.6f },
            { ampA, 0.002f }, { ampD, 1.8f }, { ampS, 0.25f }, { ampR, 0.6f }, { fA, 0.002f }, { fD, 0.5f }, { fS, 0.1f }, { fR, 0.4f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 10 }, { crushRate, 28000 }, { lofiTone, 11000 }, { wow, 0.06f }, { heat, 0.15f }, { crackle, 0.f },
            { chorusMix, 0.4f }, { chorusRate, 0.4f }, { chorusDepth, 0.45f }, { delayMix, 0.2f }, { delayTime, 0.375f }, { delayFb, 0.4f },
            { verbMix, 0.4f }, { verbSize, 0.85f }, { width, 0.9f }, { master, -6.f } } },

        { "Clav Funk", "Keys", {
            { oscAWave, 4 }, { oscAUnison, 1 }, { oscALevel, 0.75f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBLevel, 0.35f },
            { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.05f },
            { filtType, 0 }, { filtCutoff, 1800 }, { filtRes, 0.3f }, { filtEnv, 0.8f }, { filtKey, 0.6f }, { filtDrive, 0.4f },
            { ampA, 0.002f }, { ampD, 1.f }, { ampS, 0.4f }, { ampR, 0.15f }, { fA, 0.002f }, { fD, 0.25f }, { fS, 0.05f }, { fR, 0.15f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 9000 }, { wow, 0.03f }, { heat, 0.45f }, { dist, 0.08f }, { crackle, 0.f },
            { chorusMix, 0.1f }, { delayMix, 0.1f }, { verbMix, 0.15f }, { verbSize, 0.6f }, { width, 0.5f }, { master, -6.f } } },

        // ---- 808 ----
        { "Mini V 808", "808", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.9f },
            { oscBWave, 2 }, { oscBOct, -1 }, { oscBLevel, 0.3f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.05f },
            { voiceMode, 1 }, { glide, 0.07f }, { pitchEnvAmt, 12.f }, { pitchEnvDec, 0.04f },
            { filtType, 1 }, { filtCutoff, 900 }, { filtRes, 0.15f }, { filtEnv, 0.4f }, { filtKey, 0.f }, { filtDrive, 0.7f },
            { ampA, 0.002f }, { ampD, 1.4f }, { ampS, 0.f }, { ampR, 0.25f }, { fA, 0.002f }, { fD, 0.25f }, { fS, 0.f }, { fR, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 3500 }, { wow, 0.f }, { heat, 0.65f }, { dist, 0.2f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -3.f } } },

        { "Modular Punch 808", "808", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 1.f },
            { oscBOn, 0 }, { subLevel, 0.f }, { noiseLevel, 0.05f }, { drift, 0.f },
            { voiceMode, 1 }, { glide, 0.05f }, { pitchEnvAmt, 20.f }, { pitchEnvDec, 0.03f },
            { filtType, 1 }, { filtCutoff, 1500 }, { filtRes, 0.1f }, { filtEnv, 0.6f }, { filtKey, 0.f }, { filtDrive, 0.5f },
            { ampA, 0.001f }, { ampD, 0.9f }, { ampS, 0.f }, { ampR, 0.15f }, { fA, 0.001f }, { fD, 0.08f }, { fS, 0.f }, { fR, 0.05f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 4000 }, { wow, 0.f }, { heat, 0.5f }, { dist, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -3.f } } },

        { "ARP 2600 Sub 808", "808", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.9f },
            { oscBWave, 1 }, { oscBOct, -2 }, { oscBLevel, 0.2f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.f },
            { voiceMode, 1 }, { glide, 0.08f }, { pitchEnvAmt, 9.f }, { pitchEnvDec, 0.05f },
            { filtType, 1 }, { filtCutoff, 700 }, { filtRes, 0.2f }, { filtEnv, 0.3f }, { filtKey, 0.f }, { filtDrive, 0.6f },
            { ampA, 0.002f }, { ampD, 1.8f }, { ampS, 0.f }, { ampR, 0.3f }, { fA, 0.002f }, { fD, 0.3f }, { fS, 0.f }, { fR, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 3000 }, { wow, 0.f }, { heat, 0.6f }, { dist, 0.25f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -3.f } } },

        { "CS-80 Boom 808", "808", {
            { oscAWave, 3 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.9f },
            { oscBWave, 3 }, { oscBOct, 0 }, { oscBLevel, 0.15f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.05f },
            { voiceMode, 1 }, { glide, 0.1f }, { pitchEnvAmt, 7.f }, { pitchEnvDec, 0.06f },
            { filtType, 0 }, { filtCutoff, 800 }, { filtRes, 0.1f }, { filtEnv, 0.3f }, { filtKey, 0.f }, { filtDrive, 0.4f },
            { ampA, 0.003f }, { ampD, 2.6f }, { ampS, 0.f }, { ampR, 0.4f }, { fA, 0.003f }, { fD, 0.5f }, { fS, 0.f }, { fR, 0.2f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 3000 }, { wow, 0.f }, { heat, 0.5f }, { dist, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.08f }, { verbSize, 0.6f }, { verbDamp, 0.8f }, { width, 0.f }, { master, -3.f } } },

        // ---- Bass ----
        { "Mini V Fat Bass", "Bass", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 0 }, { oscBOct, -2 }, { oscBFine, 6 }, { oscBLevel, 0.6f }, { subLevel, 0.3f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { voiceMode, 1 }, { glide, 0.05f },
            { filtType, 1 }, { filtCutoff, 520 }, { filtRes, 0.3f }, { filtEnv, 0.65f }, { filtKey, 0.3f }, { filtDrive, 0.55f },
            { ampA, 0.003f }, { ampD, 0.6f }, { ampS, 0.75f }, { ampR, 0.15f }, { fA, 0.003f }, { fD, 0.4f }, { fS, 0.2f }, { fR, 0.15f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 6000 }, { wow, 0.03f }, { heat, 0.55f }, { dist, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -5.f } } },

        { "ARP 2600 Bass", "Bass", {
            { oscAWave, 1 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.75f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBFine, 5 }, { oscBLevel, 0.5f }, { subLevel, 0.3f }, { noiseLevel, 0.f }, { drift, 0.25f },
            { voiceMode, 1 }, { glide, 0.03f },
            { filtType, 1 }, { filtCutoff, 650 }, { filtRes, 0.5f }, { filtEnv, 0.8f }, { filtKey, 0.3f }, { filtDrive, 0.45f },
            { ampA, 0.002f }, { ampD, 0.5f }, { ampS, 0.6f }, { ampR, 0.12f }, { fA, 0.002f }, { fD, 0.3f }, { fS, 0.1f }, { fR, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 7000 }, { wow, 0.03f }, { heat, 0.5f }, { dist, 0.12f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -5.f } } },

        { "SEM Round Bass", "Bass", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.7f },
            { oscBWave, 2 }, { oscBOct, -1 }, { oscBLevel, 0.6f }, { subLevel, 0.4f }, { noiseLevel, 0.f }, { drift, 0.25f },
            { voiceMode, 1 }, { glide, 0.04f },
            { filtType, 0 }, { filtCutoff, 600 }, { filtRes, 0.2f }, { filtEnv, 0.5f }, { filtKey, 0.3f }, { filtDrive, 0.3f },
            { ampA, 0.004f }, { ampD, 0.8f }, { ampS, 0.8f }, { ampR, 0.2f }, { fA, 0.004f }, { fD, 0.5f }, { fS, 0.25f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 5000 }, { wow, 0.04f }, { heat, 0.45f }, { dist, 0.f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -5.f } } },

        { "Prophet Punch Bass", "Bass", {
            { oscAWave, 4 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.7f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBFine, 12 }, { oscBLevel, 0.6f }, { subLevel, 0.3f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { voiceMode, 1 }, { glide, 0.02f },
            { filtType, 1 }, { filtCutoff, 850 }, { filtRes, 0.4f }, { filtEnv, 0.9f }, { filtKey, 0.3f }, { filtDrive, 0.4f },
            { ampA, 0.002f }, { ampD, 0.7f }, { ampS, 0.5f }, { ampR, 0.15f }, { fA, 0.002f }, { fD, 0.2f }, { fS, 0.05f }, { fR, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 7000 }, { wow, 0.03f }, { heat, 0.5f }, { dist, 0.15f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -5.f } } },

        { "Jup-8 Rubber Bass", "Bass", {
            { oscAWave, 1 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.75f },
            { oscBWave, 3 }, { oscBOct, -1 }, { oscBLevel, 0.6f }, { subLevel, 0.3f }, { noiseLevel, 0.f }, { drift, 0.2f },
            { voiceMode, 1 }, { glide, 0.03f },
            { filtType, 0 }, { filtCutoff, 420 }, { filtRes, 0.25f }, { filtEnv, 0.55f }, { filtKey, 0.3f }, { filtDrive, 0.3f },
            { ampA, 0.003f }, { ampD, 0.45f }, { ampS, 0.5f }, { ampR, 0.15f }, { fA, 0.003f }, { fD, 0.3f }, { fS, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 5000 }, { wow, 0.03f }, { heat, 0.4f }, { dist, 0.05f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -5.f } } },

        { "Modular Acid", "Bass", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.9f },
            { oscBOn, 0 }, { subLevel, 0.2f }, { noiseLevel, 0.f }, { drift, 0.2f },
            { voiceMode, 1 }, { glide, 0.08f },
            { filtType, 1 }, { filtCutoff, 340 }, { filtRes, 0.7f }, { filtEnv, 1.f }, { filtKey, 0.2f }, { filtDrive, 0.5f },
            { ampA, 0.002f }, { ampD, 0.5f }, { ampS, 0.7f }, { ampR, 0.1f }, { fA, 0.002f }, { fD, 0.3f }, { fS, 0.05f }, { fR, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 8000 }, { wow, 0.f }, { heat, 0.5f }, { dist, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.08f }, { delayTime, 0.375f }, { verbMix, 0.f }, { width, 0.2f }, { master, -5.f } } },

        { "Matrix-12 Growl", "Bass", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 2 }, { oscADetune, 7 }, { oscALevel, 0.8f },
            { oscBWave, 1 }, { oscBOct, -2 }, { oscBLevel, 0.45f }, { subLevel, 0.3f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { voiceMode, 1 }, { glide, 0.04f },
            { filtType, 1 }, { filtCutoff, 480 }, { filtRes, 0.45f }, { filtEnv, 0.4f }, { filtKey, 0.2f }, { filtDrive, 0.6f },
            { ampA, 0.003f }, { ampD, 0.6f }, { ampS, 0.9f }, { ampR, 0.15f }, { fA, 0.003f }, { fD, 0.4f }, { fS, 0.3f },
            { lfoShape, 0 }, { lfoRate, 4.f }, { lfoPitch, 0.f }, { lfoFilter, 0.3f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 6000 }, { wow, 0.f }, { heat, 0.6f }, { dist, 0.25f }, { crackle, 0.f },
            { chorusMix, 0.1f }, { chorusRate, 0.3f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.3f }, { master, -5.f } } },

        { "Taurus Pedal", "Bass", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 1 }, { oscBOct, -2 }, { oscBLevel, 0.6f }, { subLevel, 0.5f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { voiceMode, 1 }, { glide, 0.1f },
            { filtType, 1 }, { filtCutoff, 300 }, { filtRes, 0.15f }, { filtEnv, 0.3f }, { filtKey, 0.1f }, { filtDrive, 0.6f },
            { ampA, 0.01f }, { ampD, 1.f }, { ampS, 1.f }, { ampR, 0.4f }, { fA, 0.01f }, { fD, 0.8f }, { fS, 0.4f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 4000 }, { wow, 0.03f }, { heat, 0.65f }, { dist, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.f }, { width, 0.f }, { master, -5.f } } },

        // ---- Synth (leads / brass / strings / plucks) ----
        { "Mini V Lead", "Synth", {
            { oscAWave, 0 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBFine, 8 }, { oscBLevel, 0.6f }, { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { voiceMode, 1 }, { glide, 0.1f },
            { filtType, 1 }, { filtCutoff, 1800 }, { filtRes, 0.2f }, { filtEnv, 0.3f }, { filtKey, 0.4f }, { filtDrive, 0.4f },
            { ampA, 0.01f }, { ampD, 1.f }, { ampS, 0.9f }, { ampR, 0.3f }, { fA, 0.01f }, { fD, 0.6f }, { fS, 0.4f },
            { lfoShape, 0 }, { lfoRate, 5.5f }, { lfoPitch, 0.03f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 10000 }, { wow, 0.03f }, { heat, 0.45f }, { dist, 0.05f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.25f }, { delayTime, 0.375f }, { delayFb, 0.4f }, { verbMix, 0.25f }, { verbSize, 0.75f }, { width, 0.4f }, { master, -6.f } } },

        { "ARP 2600 Lead", "Synth", {
            { oscAWave, 1 }, { oscAUnison, 1 }, { oscALevel, 0.7f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBFine, 6 }, { oscBLevel, 0.5f }, { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { voiceMode, 1 }, { glide, 0.06f },
            { filtType, 0 }, { filtCutoff, 2400 }, { filtRes, 0.4f }, { filtEnv, 0.4f }, { filtKey, 0.4f }, { filtDrive, 0.35f },
            { ampA, 0.005f }, { ampD, 0.8f }, { ampS, 0.85f }, { ampR, 0.3f }, { fA, 0.005f }, { fD, 0.4f }, { fS, 0.3f },
            { lfoShape, 0 }, { lfoRate, 5.f }, { lfoPitch, 0.025f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 10000 }, { wow, 0.03f }, { heat, 0.45f }, { dist, 0.12f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.3f }, { delayTime, 0.5f }, { delayFb, 0.45f }, { verbMix, 0.3f }, { verbSize, 0.8f }, { width, 0.4f }, { master, -6.f } } },

        { "Vangelis Theme", "Synth", {
            { oscAWave, 0 }, { oscAUnison, 2 }, { oscADetune, 8 }, { oscALevel, 0.75f },
            { oscBWave, 4 }, { oscBOct, 0 }, { oscBFine, 5 }, { oscBLevel, 0.5f }, { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { voiceMode, 1 }, { glide, 0.12f },
            { filtType, 0 }, { filtCutoff, 2200 }, { filtRes, 0.25f }, { filtEnv, 0.35f }, { filtKey, 0.3f }, { filtDrive, 0.25f },
            { ampA, 0.08f }, { ampD, 1.5f }, { ampS, 0.9f }, { ampR, 1.f }, { fA, 0.3f }, { fD, 1.5f }, { fS, 0.4f },
            { lfoShape, 0 }, { lfoRate, 5.5f }, { lfoPitch, 0.045f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 11000 }, { wow, 0.05f }, { heat, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { chorusRate, 0.3f }, { chorusDepth, 0.4f }, { delayMix, 0.35f }, { delayTime, 0.5f }, { delayFb, 0.5f },
            { verbMix, 0.5f }, { verbSize, 0.9f }, { width, 0.8f }, { master, -6.f } } },

        { "CS-80 Brass", "Synth", {
            { oscAWave, 0 }, { oscAUnison, 2 }, { oscADetune, 10 }, { oscALevel, 0.75f },
            { oscBWave, 4 }, { oscBOct, 0 }, { oscBFine, -7 }, { oscBLevel, 0.5f }, { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.35f },
            { filtType, 1 }, { filtCutoff, 900 }, { filtRes, 0.3f }, { filtEnv, 0.8f }, { filtKey, 0.3f }, { filtDrive, 0.3f },
            { ampA, 0.05f }, { ampD, 1.f }, { ampS, 0.85f }, { ampR, 0.5f }, { fA, 0.1f }, { fD, 0.7f }, { fS, 0.3f }, { fR, 0.4f },
            { lfoShape, 0 }, { lfoRate, 5.f }, { lfoPitch, 0.02f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 11000 }, { wow, 0.04f }, { heat, 0.45f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { chorusRate, 0.3f }, { chorusDepth, 0.4f }, { delayMix, 0.12f }, { verbMix, 0.35f }, { verbSize, 0.8f }, { width, 0.8f }, { master, -7.f } } },

        { "Jup-8 Brass", "Synth", {
            { oscAWave, 0 }, { oscAUnison, 3 }, { oscADetune, 15 }, { oscALevel, 0.75f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBUnison, 2 }, { oscBDetune, 10 }, { oscBFine, 6 }, { oscBLevel, 0.5f }, { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { filtType, 1 }, { filtCutoff, 1200 }, { filtRes, 0.2f }, { filtEnv, 0.7f }, { filtKey, 0.3f }, { filtDrive, 0.3f },
            { ampA, 0.02f }, { ampD, 1.f }, { ampS, 0.85f }, { ampR, 0.5f }, { fA, 0.04f }, { fD, 0.5f }, { fS, 0.3f }, { fR, 0.4f },
            { lfoRate, 0.3f }, { lfoPitch, 0.01f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 11000 }, { wow, 0.04f }, { heat, 0.4f }, { crackle, 0.f },
            { chorusMix, 0.45f }, { chorusRate, 0.4f }, { chorusDepth, 0.45f }, { delayMix, 0.1f }, { verbMix, 0.3f }, { verbSize, 0.8f }, { width, 0.9f }, { master, -7.f } } },

        { "Prophet 5 Lead", "Synth", {
            { oscAWave, 4 }, { oscAUnison, 2 }, { oscADetune, 8 }, { oscALevel, 0.7f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBFine, 14 }, { oscBLevel, 0.5f }, { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { voiceMode, 1 }, { glide, 0.05f },
            { filtType, 1 }, { filtCutoff, 1600 }, { filtRes, 0.45f }, { filtEnv, 0.5f }, { filtKey, 0.4f }, { filtDrive, 0.35f },
            { ampA, 0.005f }, { ampD, 0.8f }, { ampS, 0.85f }, { ampR, 0.3f }, { fA, 0.005f }, { fD, 0.5f }, { fS, 0.3f },
            { lfoShape, 0 }, { lfoRate, 5.2f }, { lfoPitch, 0.025f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 10000 }, { wow, 0.03f }, { heat, 0.45f }, { dist, 0.08f }, { crackle, 0.f },
            { chorusMix, 0.15f }, { delayMix, 0.25f }, { delayTime, 0.375f }, { delayFb, 0.4f }, { verbMix, 0.3f }, { verbSize, 0.8f }, { width, 0.5f }, { master, -6.f } } },

        { "SEM Square Lead", "Synth", {
            { oscAWave, 1 }, { oscAUnison, 2 }, { oscADetune, 8 }, { oscALevel, 0.75f },
            { oscBWave, 2 }, { oscBOct, -1 }, { oscBLevel, 0.4f }, { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.25f },
            { voiceMode, 1 }, { glide, 0.04f },
            { filtType, 0 }, { filtCutoff, 2000 }, { filtRes, 0.3f }, { filtEnv, 0.35f }, { filtKey, 0.4f }, { filtDrive, 0.3f },
            { ampA, 0.005f }, { ampD, 0.8f }, { ampS, 0.85f }, { ampR, 0.3f }, { fA, 0.005f }, { fD, 0.5f }, { fS, 0.3f },
            { lfoShape, 0 }, { lfoRate, 5.f }, { lfoPitch, 0.02f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 9000 }, { wow, 0.04f }, { heat, 0.4f }, { crackle, 0.f },
            { chorusMix, 0.1f }, { delayMix, 0.2f }, { delayTime, 0.375f }, { delayFb, 0.35f }, { verbMix, 0.25f }, { verbSize, 0.75f }, { width, 0.5f }, { master, -6.f } } },

        { "Solina Violin", "Synth", {
            { oscAWave, 0 }, { oscAUnison, 2 }, { oscADetune, 10 }, { oscALevel, 0.75f },
            { oscBWave, 0 }, { oscBOct, 1 }, { oscBFine, 4 }, { oscBLevel, 0.3f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { voiceMode, 1 }, { glide, 0.05f },
            { filtType, 0 }, { filtCutoff, 3500 }, { filtRes, 0.1f }, { filtEnv, 0.2f }, { filtKey, 0.5f },
            { ampA, 0.25f }, { ampD, 1.5f }, { ampS, 0.9f }, { ampR, 0.8f }, { fA, 0.3f }, { fD, 1.5f }, { fS, 0.5f },
            { lfoShape, 0 }, { lfoRate, 5.5f }, { lfoPitch, 0.03f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 10000 }, { wow, 0.06f }, { heat, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.8f }, { chorusRate, 0.8f }, { chorusDepth, 0.6f }, { delayMix, 0.2f }, { delayTime, 0.5f }, { verbMix, 0.4f }, { verbSize, 0.85f }, { width, 0.8f }, { master, -7.f } } },

        { "Modular Blip", "Synth", {
            { oscAWave, 1 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 3 }, { oscBOct, 1 }, { oscBLevel, 0.3f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.2f },
            { voiceMode, 1 }, { glide, 0.f },
            { filtType, 1 }, { filtCutoff, 500 }, { filtRes, 0.5f }, { filtEnv, 1.f }, { filtKey, 0.5f }, { filtDrive, 0.3f },
            { ampA, 0.001f }, { ampD, 0.3f }, { ampS, 0.f }, { ampR, 0.15f }, { fA, 0.001f }, { fD, 0.08f }, { fS, 0.f }, { fR, 0.05f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 10000 }, { wow, 0.f }, { heat, 0.4f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.35f }, { delayTime, 0.375f }, { delayFb, 0.55f }, { verbMix, 0.25f }, { verbSize, 0.8f }, { width, 0.7f }, { master, -6.f } } },

        { "Matrix-12 Pluck", "Synth", {
            { oscAWave, 0 }, { oscAUnison, 3 }, { oscADetune, 12 }, { oscALevel, 0.75f },
            { oscBWave, 4 }, { oscBOct, 0 }, { oscBLevel, 0.4f }, { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { filtType, 0 }, { filtCutoff, 700 }, { filtRes, 0.3f }, { filtEnv, 0.9f }, { filtKey, 0.5f }, { filtDrive, 0.25f },
            { ampA, 0.002f }, { ampD, 0.8f }, { ampS, 0.1f }, { ampR, 0.4f }, { fA, 0.002f }, { fD, 0.3f }, { fS, 0.05f }, { fR, 0.3f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 10000 }, { wow, 0.04f }, { heat, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.4f }, { chorusRate, 0.4f }, { chorusDepth, 0.45f }, { delayMix, 0.3f }, { delayTime, 0.375f }, { delayFb, 0.45f }, { verbMix, 0.35f }, { verbSize, 0.8f }, { width, 0.9f }, { master, -6.f } } },

        { "Synclavier FM Pluck", "Synth", {
            { oscAWave, 5 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 5 }, { oscBOct, 1 }, { oscBLevel, 0.4f }, { subLevel, 0.05f }, { noiseLevel, 0.f }, { drift, 0.05f },
            { filtType, 0 }, { filtCutoff, 1500 }, { filtRes, 0.15f }, { filtEnv, 0.85f }, { filtKey, 0.6f },
            { ampA, 0.001f }, { ampD, 0.6f }, { ampS, 0.f }, { ampR, 0.4f }, { fA, 0.001f }, { fD, 0.15f }, { fS, 0.f }, { fR, 0.2f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 12 }, { crushRate, 32000 }, { lofiTone, 12000 }, { wow, 0.03f }, { heat, 0.15f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { delayMix, 0.35f }, { delayTime, 0.375f }, { delayFb, 0.5f }, { verbMix, 0.35f }, { verbSize, 0.85f }, { width, 0.9f }, { master, -6.f } } },

        { "VS Shimmer Lead", "Synth", {
            { oscAWave, 5 }, { oscAUnison, 2 }, { oscADetune, 10 }, { oscALevel, 0.7f },
            { oscBWave, 3 }, { oscBOct, 1 }, { oscBLevel, 0.4f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.1f },
            { filtType, 0 }, { filtCutoff, 7000 }, { filtRes, 0.05f }, { filtEnv, 0.2f }, { filtKey, 0.5f },
            { ampA, 0.02f }, { ampD, 1.5f }, { ampS, 0.8f }, { ampR, 1.f }, { fA, 0.02f }, { fD, 1.f }, { fS, 0.5f },
            { lfoShape, 0 }, { lfoRate, 5.f }, { lfoPitch, 0.02f }, { lfoFilter, 0.f },
            { crushBits, 12 }, { crushRate, 32000 }, { lofiTone, 13000 }, { wow, 0.06f }, { heat, 0.15f }, { crackle, 0.f },
            { chorusMix, 0.5f }, { chorusRate, 0.4f }, { chorusDepth, 0.5f }, { delayMix, 0.3f }, { delayTime, 0.5f }, { delayFb, 0.5f }, { verbMix, 0.45f }, { verbSize, 0.9f }, { width, 1.f }, { master, -6.f } } },

        { "Farfisa Sweep", "Synth", {
            { oscAWave, 4 }, { oscAUnison, 2 }, { oscADetune, 6 }, { oscALevel, 0.65f },
            { oscBWave, 1 }, { oscBOct, 0 }, { oscBFine, -5 }, { oscBLevel, 0.45f }, { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.1f },
            { filtType, 0 }, { filtCutoff, 1200 }, { filtRes, 0.4f }, { filtEnv, 0.2f }, { filtKey, 0.3f }, { filtDrive, 0.3f },
            { ampA, 0.01f }, { ampD, 1.f }, { ampS, 1.f }, { ampR, 0.3f }, { fA, 0.01f }, { fS, 1.f },
            { lfoShape, 1 }, { lfoRate, 0.3f }, { lfoPitch, 0.f }, { lfoFilter, 0.5f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 8000 }, { wow, 0.05f }, { heat, 0.4f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { chorusRate, 0.5f }, { chorusDepth, 0.4f }, { delayMix, 0.15f }, { verbMix, 0.3f }, { verbSize, 0.75f }, { width, 0.8f }, { master, -7.f } } },

        { "Dreamcast Jupiter", "Synth", {
            { oscAWave, 0 }, { oscAUnison, 3 }, { oscADetune, 14 }, { oscALevel, 0.7f },
            { oscBWave, 1 }, { oscBOct, -1 }, { oscBLevel, 0.4f }, { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.25f },
            { filtType, 0 }, { filtCutoff, 900 }, { filtRes, 0.3f }, { filtEnv, 0.8f }, { filtKey, 0.5f }, { filtDrive, 0.2f },
            { ampA, 0.002f }, { ampD, 0.7f }, { ampS, 0.15f }, { ampR, 0.5f }, { fA, 0.002f }, { fD, 0.3f }, { fS, 0.05f }, { fR, 0.3f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 11 }, { crushRate, 22050 }, { lofiTone, 8000 }, { wow, 0.2f }, { heat, 0.3f }, { crackle, 0.1f },
            { chorusMix, 0.5f }, { chorusRate, 0.4f }, { chorusDepth, 0.5f }, { delayMix, 0.3f }, { delayTime, 0.375f }, { delayFb, 0.45f }, { verbMix, 0.4f }, { verbSize, 0.85f }, { width, 0.9f }, { master, -6.f } } },

        // ------------------------------------------------------------------
        // MIDNIGHT COLLECTION - Blade Runner (Vangelis / CS-80), The Terminator (Brad Fiedel),
        // Drive OST (Kavinsky, College, Desire, Chromatics, Cliff Martinez), general 80s synthwave,
        // and Mega Man X (SNES SPC700: 32 kHz samples, soft-filtered highs).
        // ------------------------------------------------------------------

        // ---- Synthwave: Blade Runner ----
        { "BR Main Titles", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 3 }, { oscADetune, 12 }, { oscALevel, 0.75f },
            { oscBWave, 4 }, { oscBOct, 0 }, { oscBFine, 6 }, { oscBLevel, 0.5f },
            { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { voiceMode, 1 }, { glide, 0.15f },
            { filtType, 0 }, { filtCutoff, 1700 }, { filtRes, 0.3f }, { filtEnv, 0.45f }, { filtKey, 0.3f }, { filtDrive, 0.25f },
            { ampA, 0.12f }, { ampD, 2.f }, { ampS, 0.9f }, { ampR, 1.5f }, { fA, 0.6f }, { fD, 2.5f }, { fS, 0.5f }, { fR, 1.5f },
            { lfoShape, 0 }, { lfoRate, 5.2f }, { lfoPitch, 0.05f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 11000 }, { wow, 0.06f }, { heat, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { chorusRate, 0.3f }, { chorusDepth, 0.4f }, { delayMix, 0.4f }, { delayTime, 0.5f }, { delayFb, 0.55f },
            { verbMix, 0.6f }, { verbSize, 0.95f }, { verbDamp, 0.3f }, { width, 0.9f }, { master, -6.f } } },

        { "BR Tears In Rain", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 4 }, { oscADetune, 14 }, { oscALevel, 0.6f },
            { oscBWave, 3 }, { oscBOct, 0 }, { oscBUnison, 2 }, { oscBDetune, 8 }, { oscBLevel, 0.55f },
            { subLevel, 0.2f }, { noiseLevel, 0.01f }, { drift, 0.3f },
            { filtType, 0 }, { filtCutoff, 1100 }, { filtRes, 0.15f }, { filtEnv, 0.4f }, { filtKey, 0.3f },
            { ampA, 1.2f }, { ampD, 3.f }, { ampS, 0.9f }, { ampR, 3.5f }, { fA, 2.5f }, { fD, 4.f }, { fS, 0.5f }, { fR, 3.f },
            { lfoShape, 0 }, { lfoRate, 0.2f }, { lfoPitch, 0.02f }, { lfoFilter, 0.15f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 9000 }, { wow, 0.08f }, { heat, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.45f }, { chorusRate, 0.25f }, { chorusDepth, 0.5f }, { delayMix, 0.3f }, { delayTime, 0.75f }, { delayFb, 0.5f },
            { verbMix, 0.7f }, { verbSize, 0.98f }, { verbDamp, 0.25f }, { width, 1.f }, { master, -7.f } } },

        { "BR Love Theme Lead", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 2 }, { oscADetune, 8 }, { oscALevel, 0.7f },
            { oscBWave, 4 }, { oscBOct, 0 }, { oscBFine, -9 }, { oscBLevel, 0.55f },
            { subLevel, 0.1f }, { noiseLevel, 0.02f }, { drift, 0.35f },
            { voiceMode, 1 }, { glide, 0.1f },
            { filtType, 2 }, { filtCutoff, 1300 }, { filtRes, 0.4f }, { filtEnv, 0.5f }, { filtKey, 0.5f }, { filtDrive, 0.4f },
            { ampA, 0.06f }, { ampD, 1.f }, { ampS, 0.9f }, { ampR, 0.8f }, { fA, 0.15f }, { fD, 1.f }, { fS, 0.5f },
            { lfoShape, 0 }, { lfoRate, 5.8f }, { lfoPitch, 0.06f }, { lfoFilter, 0.08f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 9000 }, { wow, 0.05f }, { heat, 0.5f }, { dist, 0.08f }, { crackle, 0.f },
            { chorusMix, 0.2f }, { delayMix, 0.35f }, { delayTime, 0.5f }, { delayFb, 0.5f }, { verbMix, 0.5f }, { verbSize, 0.9f }, { width, 0.7f }, { master, -6.f } } },

        { "BR Rachel's Bells", "Synthwave", {
            { oscAWave, 5 }, { oscAOct, 1 }, { oscAUnison, 2 }, { oscADetune, 6 }, { oscALevel, 0.65f },
            { oscBWave, 3 }, { oscBOct, 1 }, { oscBFine, 5 }, { oscBLevel, 0.4f },
            { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.1f },
            { filtType, 0 }, { filtCutoff, 7000 }, { filtRes, 0.05f }, { filtEnv, 0.3f }, { filtKey, 0.7f },
            { ampA, 0.002f }, { ampD, 3.f }, { ampS, 0.1f }, { ampR, 2.5f }, { fA, 0.002f }, { fD, 0.8f }, { fS, 0.1f }, { fR, 1.f },
            { lfoRate, 4.5f }, { lfoPitch, 0.01f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 12000 }, { wow, 0.08f }, { heat, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.4f }, { chorusRate, 0.4f }, { chorusDepth, 0.45f }, { delayMix, 0.4f }, { delayTime, 0.5f }, { delayFb, 0.55f },
            { verbMix, 0.65f }, { verbSize, 0.95f }, { verbDamp, 0.3f }, { width, 1.f }, { master, -7.f } } },

        { "BR Deckard's Drift", "Synthwave", {
            { oscAWave, 2 }, { oscAOct, -1 }, { oscAUnison, 2 }, { oscADetune, 10 }, { oscALevel, 0.7f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBUnison, 3 }, { oscBDetune, 18 }, { oscBLevel, 0.35f },
            { subLevel, 0.3f }, { noiseLevel, 0.04f }, { drift, 0.4f },
            { filtType, 1 }, { filtCutoff, 700 }, { filtRes, 0.35f }, { filtEnv, 0.4f }, { filtKey, 0.2f }, { filtDrive, 0.3f },
            { ampA, 2.f }, { ampD, 3.f }, { ampS, 0.9f }, { ampR, 4.f }, { fA, 3.f }, { fD, 4.f }, { fS, 0.5f }, { fR, 3.f },
            { lfoShape, 1 }, { lfoRate, 0.1f }, { lfoPitch, 0.02f }, { lfoFilter, 0.4f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 8000 }, { wow, 0.1f }, { heat, 0.4f }, { crackle, 0.f },
            { chorusMix, 0.4f }, { chorusRate, 0.2f }, { chorusDepth, 0.5f }, { delayMix, 0.3f }, { delayTime, 1.f }, { delayFb, 0.55f },
            { verbMix, 0.7f }, { verbSize, 0.98f }, { verbDamp, 0.2f }, { width, 1.f }, { master, -8.f } } },

        // ---- Synthwave: The Terminator ----
        { "T-800 Pulse Bass", "Synthwave", {
            { oscAWave, 1 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBFine, 7 }, { oscBLevel, 0.5f }, { subLevel, 0.4f }, { noiseLevel, 0.f }, { drift, 0.2f },
            { voiceMode, 1 }, { glide, 0.f },
            { filtType, 1 }, { filtCutoff, 500 }, { filtRes, 0.4f }, { filtEnv, 0.9f }, { filtKey, 0.3f }, { filtDrive, 0.5f },
            { ampA, 0.002f }, { ampD, 0.35f }, { ampS, 0.3f }, { ampR, 0.1f }, { fA, 0.002f }, { fD, 0.18f }, { fS, 0.05f }, { fR, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 7000 }, { wow, 0.f }, { heat, 0.55f }, { dist, 0.2f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.15f }, { delayTime, 0.1875f }, { delayFb, 0.3f }, { verbMix, 0.1f }, { verbSize, 0.6f }, { width, 0.3f }, { master, -5.f } } },

        { "Future War Pad", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 5 }, { oscADetune, 24 }, { oscALevel, 0.65f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBUnison, 3 }, { oscBDetune, 12 }, { oscBLevel, 0.45f },
            { subLevel, 0.25f }, { noiseLevel, 0.03f }, { drift, 0.4f },
            { filtType, 1 }, { filtCutoff, 650 }, { filtRes, 0.3f }, { filtEnv, 0.5f }, { filtKey, 0.2f }, { filtDrive, 0.4f },
            { ampA, 0.8f }, { ampD, 3.f }, { ampS, 0.9f }, { ampR, 2.5f }, { fA, 2.f }, { fD, 4.f }, { fS, 0.4f }, { fR, 2.f },
            { lfoShape, 0 }, { lfoRate, 0.15f }, { lfoPitch, 0.02f }, { lfoFilter, 0.35f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 8000 }, { wow, 0.08f }, { heat, 0.5f }, { dist, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.35f }, { chorusRate, 0.2f }, { chorusDepth, 0.5f }, { delayMix, 0.2f }, { delayTime, 0.5f }, { delayFb, 0.45f },
            { verbMix, 0.5f }, { verbSize, 0.92f }, { width, 1.f }, { master, -8.f } } },

        { "Cyberdyne Lead", "Synthwave", {
            { oscAWave, 1 }, { oscAUnison, 2 }, { oscADetune, 9 }, { oscALevel, 0.75f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBLevel, 0.4f }, { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { voiceMode, 1 }, { glide, 0.12f },
            { filtType, 1 }, { filtCutoff, 1500 }, { filtRes, 0.35f }, { filtEnv, 0.5f }, { filtKey, 0.4f }, { filtDrive, 0.45f },
            { ampA, 0.01f }, { ampD, 0.8f }, { ampS, 0.85f }, { ampR, 0.4f }, { fA, 0.01f }, { fD, 0.5f }, { fS, 0.3f },
            { lfoShape, 0 }, { lfoRate, 5.f }, { lfoPitch, 0.03f }, { lfoFilter, 0.f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 9000 }, { wow, 0.04f }, { heat, 0.55f }, { dist, 0.15f }, { crackle, 0.f },
            { chorusMix, 0.15f }, { delayMix, 0.3f }, { delayTime, 0.375f }, { delayFb, 0.45f }, { verbMix, 0.3f }, { verbSize, 0.8f }, { width, 0.5f }, { master, -6.f } } },

        { "Judgment Day Drone", "Synthwave", {
            { oscAWave, 0 }, { oscAOct, -2 }, { oscAUnison, 2 }, { oscADetune, 8 }, { oscALevel, 0.8f },
            { oscBWave, 1 }, { oscBOct, -1 }, { oscBSemi, 7 }, { oscBLevel, 0.35f },
            { subLevel, 0.4f }, { noiseLevel, 0.06f }, { drift, 0.4f },
            { filtType, 1 }, { filtCutoff, 380 }, { filtRes, 0.5f }, { filtEnv, 0.3f }, { filtKey, 0.1f }, { filtDrive, 0.6f },
            { ampA, 1.5f }, { ampD, 3.f }, { ampS, 1.f }, { ampR, 4.f }, { fA, 4.f }, { fD, 4.f }, { fS, 0.5f }, { fR, 4.f },
            { lfoShape, 1 }, { lfoRate, 0.08f }, { lfoPitch, 0.01f }, { lfoFilter, 0.5f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 6000 }, { wow, 0.1f }, { heat, 0.6f }, { dist, 0.2f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { chorusRate, 0.15f }, { chorusDepth, 0.5f }, { delayMix, 0.2f }, { delayTime, 1.f }, { delayFb, 0.5f },
            { verbMix, 0.55f }, { verbSize, 0.97f }, { verbDamp, 0.4f }, { width, 1.f }, { master, -8.f } } },

        // ---- Synthwave: Drive OST ----
        { "Nightcall Talkbox", "Synthwave", {
            { oscAWave, 4 }, { oscAUnison, 2 }, { oscADetune, 7 }, { oscALevel, 0.75f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBFine, 5 }, { oscBLevel, 0.45f },
            { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.15f },
            { filtType, 2 }, { filtCutoff, 900 }, { filtRes, 0.6f }, { filtEnv, 0.6f }, { filtKey, 0.5f }, { filtDrive, 0.5f },
            { ampA, 0.01f }, { ampD, 0.8f }, { ampS, 0.8f }, { ampR, 0.3f }, { fA, 0.04f }, { fD, 0.5f }, { fS, 0.4f }, { fR, 0.3f },
            { lfoShape, 0 }, { lfoRate, 4.f }, { lfoPitch, 0.f }, { lfoFilter, 0.25f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 9000 }, { wow, 0.04f }, { heat, 0.55f }, { dist, 0.15f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { chorusRate, 0.4f }, { chorusDepth, 0.4f }, { delayMix, 0.25f }, { delayTime, 0.375f }, { delayFb, 0.4f },
            { verbMix, 0.3f }, { verbSize, 0.8f }, { width, 0.8f }, { master, -6.f } } },

        { "Real Hero Keys", "Synthwave", {
            { oscAWave, 4 }, { oscAUnison, 2 }, { oscADetune, 6 }, { oscALevel, 0.65f },
            { oscBWave, 3 }, { oscBOct, 1 }, { oscBLevel, 0.3f },
            { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.15f },
            { filtType, 0 }, { filtCutoff, 2400 }, { filtRes, 0.15f }, { filtEnv, 0.5f }, { filtKey, 0.5f }, { filtDrive, 0.15f },
            { ampA, 0.004f }, { ampD, 1.4f }, { ampS, 0.5f }, { ampR, 0.8f }, { fA, 0.004f }, { fD, 0.5f }, { fS, 0.2f }, { fR, 0.5f },
            { lfoRate, 0.3f }, { lfoPitch, 0.01f }, { lfoFilter, 0.f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 10000 }, { wow, 0.06f }, { heat, 0.25f }, { crackle, 0.f },
            { chorusMix, 0.7f }, { chorusRate, 0.6f }, { chorusDepth, 0.5f }, { delayMix, 0.3f }, { delayTime, 0.375f }, { delayFb, 0.45f },
            { verbMix, 0.45f }, { verbSize, 0.85f }, { width, 1.f }, { master, -6.f } } },

        { "Under Your Spell Arp", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 2 }, { oscADetune, 8 }, { oscALevel, 0.7f },
            { oscBWave, 4 }, { oscBOct, 1 }, { oscBLevel, 0.3f },
            { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.15f },
            { filtType, 0 }, { filtCutoff, 900 }, { filtRes, 0.35f }, { filtEnv, 0.85f }, { filtKey, 0.5f }, { filtDrive, 0.25f },
            { ampA, 0.002f }, { ampD, 0.5f }, { ampS, 0.1f }, { ampR, 0.3f }, { fA, 0.002f }, { fD, 0.22f }, { fS, 0.05f }, { fR, 0.2f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 10000 }, { wow, 0.03f }, { heat, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.35f }, { chorusRate, 0.4f }, { chorusDepth, 0.4f }, { delayMix, 0.4f }, { delayTime, 0.375f }, { delayFb, 0.55f },
            { verbMix, 0.35f }, { verbSize, 0.85f }, { width, 1.f }, { master, -6.f } } },

        { "Tick Of The Clock Bass", "Synthwave", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 1 }, { oscBOct, -2 }, { oscBLevel, 0.5f }, { subLevel, 0.4f }, { noiseLevel, 0.f }, { drift, 0.2f },
            { voiceMode, 1 }, { glide, 0.f },
            { filtType, 1 }, { filtCutoff, 420 }, { filtRes, 0.3f }, { filtEnv, 0.7f }, { filtKey, 0.3f }, { filtDrive, 0.4f },
            { ampA, 0.002f }, { ampD, 0.3f }, { ampS, 0.4f }, { ampR, 0.08f }, { fA, 0.002f }, { fD, 0.15f }, { fS, 0.1f }, { fR, 0.08f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 6000 }, { wow, 0.f }, { heat, 0.5f }, { dist, 0.12f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.05f }, { width, 0.2f }, { master, -5.f } } },

        { "Martinez Glass Pad", "Synthwave", {
            { oscAWave, 5 }, { oscAUnison, 3 }, { oscADetune, 10 }, { oscALevel, 0.55f },
            { oscBWave, 3 }, { oscBOct, 0 }, { oscBUnison, 3 }, { oscBDetune, 10 }, { oscBLevel, 0.5f },
            { subLevel, 0.2f }, { noiseLevel, 0.02f }, { drift, 0.2f },
            { filtType, 0 }, { filtCutoff, 3500 }, { filtRes, 0.1f }, { filtEnv, 0.2f }, { filtKey, 0.5f },
            { ampA, 1.5f }, { ampD, 3.f }, { ampS, 0.9f }, { ampR, 4.f }, { fA, 2.f }, { fD, 3.f }, { fS, 0.5f }, { fR, 3.f },
            { lfoShape, 0 }, { lfoRate, 0.15f }, { lfoPitch, 0.03f }, { lfoFilter, 0.15f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 11000 }, { wow, 0.08f }, { heat, 0.15f }, { crackle, 0.f },
            { chorusMix, 0.5f }, { chorusRate, 0.2f }, { chorusDepth, 0.5f }, { delayMix, 0.35f }, { delayTime, 0.75f }, { delayFb, 0.55f },
            { verbMix, 0.75f }, { verbSize, 0.98f }, { verbDamp, 0.2f }, { width, 1.f }, { master, -7.f } } },

        { "Drive Night Cruise", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 4 }, { oscADetune, 16 }, { oscALevel, 0.7f },
            { oscBWave, 4 }, { oscBOct, -1 }, { oscBLevel, 0.45f },
            { subLevel, 0.2f }, { noiseLevel, 0.f }, { drift, 0.25f },
            { filtType, 0 }, { filtCutoff, 1500 }, { filtRes, 0.25f }, { filtEnv, 0.4f }, { filtKey, 0.3f }, { filtDrive, 0.2f },
            { ampA, 0.3f }, { ampD, 2.f }, { ampS, 0.85f }, { ampR, 1.5f }, { fA, 0.8f }, { fD, 2.f }, { fS, 0.5f }, { fR, 1.5f },
            { lfoShape, 0 }, { lfoRate, 0.3f }, { lfoPitch, 0.02f }, { lfoFilter, 0.12f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 10000 }, { wow, 0.06f }, { heat, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.6f }, { chorusRate, 0.35f }, { chorusDepth, 0.5f }, { delayMix, 0.25f }, { delayTime, 0.375f }, { delayFb, 0.45f },
            { verbMix, 0.45f }, { verbSize, 0.88f }, { width, 1.f }, { master, -7.f } } },

        // ---- Synthwave: general 80s ----
        { "Juno Dream Pad", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 2 }, { oscADetune, 6 }, { oscALevel, 0.7f },
            { oscBWave, 4 }, { oscBOct, 0 }, { oscBLevel, 0.4f }, { subLevel, 0.3f }, { noiseLevel, 0.f }, { drift, 0.15f },
            { filtType, 1 }, { filtCutoff, 1600 }, { filtRes, 0.15f }, { filtEnv, 0.35f }, { filtKey, 0.3f }, { filtDrive, 0.1f },
            { ampA, 0.4f }, { ampD, 2.f }, { ampS, 0.9f }, { ampR, 1.8f }, { fA, 0.8f }, { fD, 2.f }, { fS, 0.5f }, { fR, 1.5f },
            { lfoRate, 0.3f }, { lfoPitch, 0.01f }, { lfoFilter, 0.08f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 11000 }, { wow, 0.05f }, { heat, 0.25f }, { crackle, 0.f },
            { chorusMix, 0.85f }, { chorusRate, 0.55f }, { chorusDepth, 0.6f }, { delayMix, 0.15f }, { verbMix, 0.4f }, { verbSize, 0.85f }, { width, 1.f }, { master, -7.f } } },

        { "DX Tine Piano", "Synthwave", {
            { oscAWave, 5 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 3 }, { oscBOct, 2 }, { oscBLevel, 0.2f }, { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.05f },
            { filtType, 0 }, { filtCutoff, 6000 }, { filtRes, 0.05f }, { filtEnv, 0.5f }, { filtKey, 0.7f },
            { ampA, 0.002f }, { ampD, 2.5f }, { ampS, 0.2f }, { ampR, 0.8f }, { fA, 0.002f }, { fD, 0.5f }, { fS, 0.1f }, { fR, 0.5f },
            { lfoRate, 5.f }, { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 12000 }, { wow, 0.03f }, { heat, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.5f }, { chorusRate, 0.7f }, { chorusDepth, 0.45f }, { delayMix, 0.15f }, { verbMix, 0.4f }, { verbSize, 0.85f }, { width, 0.9f }, { master, -6.f } } },

        { "OB-Xa Jump Brass", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 3 }, { oscADetune, 14 }, { oscALevel, 0.8f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBFine, -10 }, { oscBUnison, 2 }, { oscBDetune, 8 }, { oscBLevel, 0.55f },
            { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { filtType, 1 }, { filtCutoff, 1400 }, { filtRes, 0.15f }, { filtEnv, 0.7f }, { filtKey, 0.3f }, { filtDrive, 0.35f },
            { ampA, 0.006f }, { ampD, 0.8f }, { ampS, 0.8f }, { ampR, 0.3f }, { fA, 0.01f }, { fD, 0.4f }, { fS, 0.4f }, { fR, 0.3f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 12000 }, { wow, 0.03f }, { heat, 0.45f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { chorusRate, 0.3f }, { chorusDepth, 0.35f }, { delayMix, 0.15f }, { verbMix, 0.3f }, { verbSize, 0.8f }, { width, 0.9f }, { master, -7.f } } },

        { "Take On Me Lead", "Synthwave", {
            { oscAWave, 1 }, { oscAUnison, 1 }, { oscALevel, 0.75f },
            { oscBWave, 4 }, { oscBOct, 0 }, { oscBFine, 6 }, { oscBLevel, 0.5f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.1f },
            { voiceMode, 1 }, { glide, 0.02f },
            { filtType, 0 }, { filtCutoff, 4000 }, { filtRes, 0.15f }, { filtEnv, 0.3f }, { filtKey, 0.5f }, { filtDrive, 0.2f },
            { ampA, 0.003f }, { ampD, 0.4f }, { ampS, 0.8f }, { ampR, 0.15f }, { fA, 0.003f }, { fD, 0.3f }, { fS, 0.4f },
            { lfoShape, 0 }, { lfoRate, 5.5f }, { lfoPitch, 0.02f }, { lfoFilter, 0.f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 12000 }, { wow, 0.03f }, { heat, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.25f }, { delayMix, 0.3f }, { delayTime, 0.375f }, { delayFb, 0.4f }, { verbMix, 0.3f }, { verbSize, 0.8f }, { width, 0.7f }, { master, -6.f } } },

        { "Outrun Octave Bass", "Synthwave", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBLevel, 0.4f }, { subLevel, 0.3f }, { noiseLevel, 0.f }, { drift, 0.15f },
            { voiceMode, 1 }, { glide, 0.f },
            { filtType, 1 }, { filtCutoff, 600 }, { filtRes, 0.3f }, { filtEnv, 0.8f }, { filtKey, 0.3f }, { filtDrive, 0.4f },
            { ampA, 0.002f }, { ampD, 0.25f }, { ampS, 0.3f }, { ampR, 0.08f }, { fA, 0.002f }, { fD, 0.12f }, { fS, 0.05f }, { fR, 0.08f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 8000 }, { wow, 0.f }, { heat, 0.5f }, { dist, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.1f }, { delayMix, 0.1f }, { delayTime, 0.1875f }, { delayFb, 0.2f }, { verbMix, 0.05f }, { width, 0.3f }, { master, -5.f } } },

        { "Neon Pluck", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 3 }, { oscADetune, 12 }, { oscALevel, 0.7f },
            { oscBWave, 1 }, { oscBOct, 1 }, { oscBLevel, 0.25f }, { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.15f },
            { filtType, 0 }, { filtCutoff, 800 }, { filtRes, 0.3f }, { filtEnv, 0.9f }, { filtKey, 0.6f }, { filtDrive, 0.2f },
            { ampA, 0.002f }, { ampD, 0.6f }, { ampS, 0.f }, { ampR, 0.4f }, { fA, 0.002f }, { fD, 0.2f }, { fS, 0.f }, { fR, 0.2f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 11000 }, { wow, 0.03f }, { heat, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.4f }, { chorusRate, 0.4f }, { chorusDepth, 0.4f }, { delayMix, 0.45f }, { delayTime, 0.375f }, { delayFb, 0.55f },
            { verbMix, 0.4f }, { verbSize, 0.88f }, { width, 1.f }, { master, -6.f } } },

        { "Sunset Strings", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 5 }, { oscADetune, 20 }, { oscALevel, 0.65f },
            { oscBWave, 0 }, { oscBOct, 1 }, { oscBUnison, 2 }, { oscBDetune, 10 }, { oscBLevel, 0.3f },
            { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { filtType, 0 }, { filtCutoff, 3000 }, { filtRes, 0.1f }, { filtEnv, 0.25f }, { filtKey, 0.4f },
            { ampA, 0.6f }, { ampD, 2.f }, { ampS, 0.9f }, { ampR, 2.f }, { fA, 1.f }, { fD, 2.f }, { fS, 0.5f }, { fR, 2.f },
            { lfoShape, 0 }, { lfoRate, 0.4f }, { lfoPitch, 0.03f }, { lfoFilter, 0.05f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 11000 }, { wow, 0.06f }, { heat, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.8f }, { chorusRate, 0.7f }, { chorusDepth, 0.6f }, { delayMix, 0.2f }, { delayTime, 0.5f }, { delayFb, 0.4f },
            { verbMix, 0.55f }, { verbSize, 0.92f }, { width, 1.f }, { master, -7.f } } },

        // ---- 16-Bit: Mega Man X (SNES) ----
        { "X Opening Stage Lead", "16-Bit", {
            { oscAWave, 0 }, { oscAUnison, 2 }, { oscADetune, 10 }, { oscALevel, 0.75f },
            { oscBWave, 1 }, { oscBOct, 0 }, { oscBFine, 6 }, { oscBLevel, 0.5f }, { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.1f },
            { voiceMode, 1 }, { glide, 0.03f },
            { filtType, 0 }, { filtCutoff, 3200 }, { filtRes, 0.2f }, { filtEnv, 0.3f }, { filtKey, 0.5f }, { filtDrive, 0.5f },
            { ampA, 0.004f }, { ampD, 0.6f }, { ampS, 0.85f }, { ampR, 0.2f }, { fA, 0.004f }, { fD, 0.3f }, { fS, 0.4f },
            { lfoShape, 0 }, { lfoRate, 6.f }, { lfoPitch, 0.04f }, { lfoFilter, 0.f },
            { crushBits, 12 }, { crushRate, 32000 }, { lofiTone, 8500 }, { wow, 0.f }, { heat, 0.6f }, { dist, 0.4f }, { crackle, 0.f },
            { chorusMix, 0.15f }, { delayMix, 0.25f }, { delayTime, 0.25f }, { delayFb, 0.35f }, { verbMix, 0.25f }, { verbSize, 0.7f }, { width, 0.6f }, { master, -6.f } } },

        { "X Power Chord Pad", "16-Bit", {
            { oscAWave, 0 }, { oscAUnison, 4 }, { oscADetune, 14 }, { oscALevel, 0.7f },
            { oscBWave, 0 }, { oscBOct, -1 }, { oscBUnison, 2 }, { oscBDetune, 8 }, { oscBLevel, 0.5f },
            { subLevel, 0.2f }, { noiseLevel, 0.f }, { drift, 0.1f },
            { filtType, 0 }, { filtCutoff, 2200 }, { filtRes, 0.15f }, { filtEnv, 0.3f }, { filtKey, 0.3f }, { filtDrive, 0.6f },
            { ampA, 0.01f }, { ampD, 1.5f }, { ampS, 0.8f }, { ampR, 0.5f }, { fA, 0.01f }, { fD, 0.8f }, { fS, 0.4f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 12 }, { crushRate, 32000 }, { lofiTone, 8000 }, { wow, 0.f }, { heat, 0.7f }, { dist, 0.5f }, { crackle, 0.f },
            { chorusMix, 0.2f }, { delayMix, 0.1f }, { verbMix, 0.3f }, { verbSize, 0.75f }, { width, 0.9f }, { master, -8.f } } },

        { "X Slap Bass", "16-Bit", {
            { oscAWave, 4 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 2 }, { oscBOct, -1 }, { oscBLevel, 0.5f }, { subLevel, 0.3f }, { noiseLevel, 0.03f }, { drift, 0.05f },
            { voiceMode, 1 }, { glide, 0.f },
            { filtType, 0 }, { filtCutoff, 900 }, { filtRes, 0.35f }, { filtEnv, 1.f }, { filtKey, 0.4f }, { filtDrive, 0.4f },
            { ampA, 0.001f }, { ampD, 0.5f }, { ampS, 0.3f }, { ampR, 0.08f }, { fA, 0.001f }, { fD, 0.08f }, { fS, 0.05f }, { fR, 0.05f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 12 }, { crushRate, 32000 }, { lofiTone, 7000 }, { wow, 0.f }, { heat, 0.4f }, { dist, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.f }, { verbMix, 0.08f }, { verbSize, 0.5f }, { width, 0.2f }, { master, -5.f } } },

        { "X Storm Eagle Brass", "16-Bit", {
            { oscAWave, 4 }, { oscAUnison, 2 }, { oscADetune, 8 }, { oscALevel, 0.75f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBFine, 5 }, { oscBLevel, 0.5f }, { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.1f },
            { filtType, 0 }, { filtCutoff, 1800 }, { filtRes, 0.2f }, { filtEnv, 0.6f }, { filtKey, 0.4f }, { filtDrive, 0.3f },
            { ampA, 0.01f }, { ampD, 0.6f }, { ampS, 0.8f }, { ampR, 0.2f }, { fA, 0.03f }, { fD, 0.3f }, { fS, 0.4f },
            { lfoShape, 0 }, { lfoRate, 5.5f }, { lfoPitch, 0.02f }, { lfoFilter, 0.f },
            { crushBits, 12 }, { crushRate, 32000 }, { lofiTone, 8500 }, { wow, 0.f }, { heat, 0.4f }, { crackle, 0.f },
            { chorusMix, 0.2f }, { delayMix, 0.2f }, { delayTime, 0.25f }, { delayFb, 0.3f }, { verbMix, 0.3f }, { verbSize, 0.75f }, { width, 0.8f }, { master, -7.f } } },

        { "X Sigma Fortress Organ", "16-Bit", {
            { oscAWave, 1 }, { oscAUnison, 1 }, { oscALevel, 0.65f },
            { oscBWave, 4 }, { oscBOct, 1 }, { oscBLevel, 0.45f }, { subLevel, 0.4f }, { noiseLevel, 0.f }, { drift, 0.05f },
            { filtType, 0 }, { filtCutoff, 3500 }, { filtRes, 0.1f }, { filtEnv, 0.f }, { filtKey, 0.4f }, { filtDrive, 0.3f },
            { ampA, 0.003f }, { ampD, 1.f }, { ampS, 1.f }, { ampR, 0.1f }, { fA, 0.003f }, { fS, 1.f },
            { lfoShape, 0 }, { lfoRate, 6.f }, { lfoPitch, 0.015f }, { lfoFilter, 0.1f },
            { crushBits, 12 }, { crushRate, 32000 }, { lofiTone, 8000 }, { wow, 0.f }, { heat, 0.45f }, { dist, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.4f }, { chorusRate, 1.5f }, { chorusDepth, 0.4f }, { delayMix, 0.15f }, { verbMix, 0.35f }, { verbSize, 0.85f }, { width, 0.9f }, { master, -8.f } } },

        { "X Chill Penguin Bells", "16-Bit", {
            { oscAWave, 5 }, { oscAOct, 1 }, { oscAUnison, 1 }, { oscALevel, 0.7f },
            { oscBWave, 3 }, { oscBOct, 2 }, { oscBLevel, 0.35f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.05f },
            { filtType, 0 }, { filtCutoff, 7000 }, { filtRes, 0.f }, { filtEnv, 0.3f }, { filtKey, 0.8f },
            { ampA, 0.001f }, { ampD, 1.2f }, { ampS, 0.f }, { ampR, 0.8f }, { fA, 0.001f }, { fD, 0.3f }, { fS, 0.f }, { fR, 0.3f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 11 }, { crushRate, 32000 }, { lofiTone, 9000 }, { wow, 0.f }, { heat, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { delayMix, 0.35f }, { delayTime, 0.25f }, { delayFb, 0.45f }, { verbMix, 0.4f }, { verbSize, 0.85f }, { width, 1.f }, { master, -7.f } } },

        { "X Spark Mandrill Bass", "16-Bit", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 1 }, { oscBOct, -1 }, { oscBFine, 8 }, { oscBLevel, 0.5f }, { subLevel, 0.3f }, { noiseLevel, 0.f }, { drift, 0.1f },
            { voiceMode, 1 }, { glide, 0.02f },
            { filtType, 1 }, { filtCutoff, 700 }, { filtRes, 0.4f }, { filtEnv, 0.8f }, { filtKey, 0.3f }, { filtDrive, 0.5f },
            { ampA, 0.002f }, { ampD, 0.4f }, { ampS, 0.5f }, { ampR, 0.1f }, { fA, 0.002f }, { fD, 0.15f }, { fS, 0.1f }, { fR, 0.08f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 12 }, { crushRate, 32000 }, { lofiTone, 7000 }, { wow, 0.f }, { heat, 0.55f }, { dist, 0.25f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.1f }, { delayTime, 0.1875f }, { delayFb, 0.25f }, { verbMix, 0.05f }, { width, 0.3f }, { master, -5.f } } },

        { "X Highway Arp Pluck", "16-Bit", {
            { oscAWave, 1 }, { oscAUnison, 1 }, { oscALevel, 0.75f },
            { oscBWave, 4 }, { oscBOct, 1 }, { oscBLevel, 0.3f }, { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.05f },
            { filtType, 0 }, { filtCutoff, 1200 }, { filtRes, 0.3f }, { filtEnv, 0.9f }, { filtKey, 0.6f }, { filtDrive, 0.2f },
            { ampA, 0.001f }, { ampD, 0.35f }, { ampS, 0.f }, { ampR, 0.2f }, { fA, 0.001f }, { fD, 0.12f }, { fS, 0.f }, { fR, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 11 }, { crushRate, 32000 }, { lofiTone, 8500 }, { wow, 0.f }, { heat, 0.3f }, { crackle, 0.f },
            { chorusMix, 0.15f }, { delayMix, 0.4f }, { delayTime, 0.1875f }, { delayFb, 0.5f }, { verbMix, 0.25f }, { verbSize, 0.75f }, { width, 0.9f }, { master, -6.f } } },

        { "X Zero's Theme Lead", "16-Bit", {
            { oscAWave, 0 }, { oscAUnison, 3 }, { oscADetune, 12 }, { oscALevel, 0.7f },
            { oscBWave, 0 }, { oscBOct, 1 }, { oscBLevel, 0.3f }, { subLevel, 0.05f }, { noiseLevel, 0.f }, { drift, 0.1f },
            { voiceMode, 1 }, { glide, 0.05f },
            { filtType, 0 }, { filtCutoff, 2800 }, { filtRes, 0.25f }, { filtEnv, 0.35f }, { filtKey, 0.5f }, { filtDrive, 0.5f },
            { ampA, 0.01f }, { ampD, 0.8f }, { ampS, 0.85f }, { ampR, 0.3f }, { fA, 0.02f }, { fD, 0.4f }, { fS, 0.4f },
            { lfoShape, 0 }, { lfoRate, 5.5f }, { lfoPitch, 0.045f }, { lfoFilter, 0.f },
            { crushBits, 12 }, { crushRate, 32000 }, { lofiTone, 8500 }, { wow, 0.f }, { heat, 0.6f }, { dist, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { delayMix, 0.3f }, { delayTime, 0.375f }, { delayFb, 0.4f }, { verbMix, 0.35f }, { verbSize, 0.8f }, { width, 0.8f }, { master, -6.f } } },

        // ---- more famous 80s synth sounds ----
        { "BR Blade Runner Blues", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 2 }, { oscADetune, 6 }, { oscALevel, 0.7f },
            { oscBWave, 4 }, { oscBOct, 0 }, { oscBFine, -8 }, { oscBLevel, 0.55f },
            { subLevel, 0.1f }, { noiseLevel, 0.02f }, { drift, 0.35f },
            { voiceMode, 1 }, { glide, 0.18f },
            { filtType, 0 }, { filtCutoff, 1000 }, { filtRes, 0.35f }, { filtEnv, 0.55f }, { filtKey, 0.4f }, { filtDrive, 0.35f },
            { ampA, 0.15f }, { ampD, 1.5f }, { ampS, 0.9f }, { ampR, 1.2f }, { fA, 0.4f }, { fD, 1.5f }, { fS, 0.45f }, { fR, 1.f },
            { lfoShape, 0 }, { lfoRate, 5.f }, { lfoPitch, 0.05f }, { lfoFilter, 0.1f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 8500 }, { wow, 0.06f }, { heat, 0.45f }, { crackle, 0.f },
            { chorusMix, 0.25f }, { delayMix, 0.4f }, { delayTime, 0.75f }, { delayFb, 0.5f }, { verbMix, 0.65f }, { verbSize, 0.97f }, { verbDamp, 0.3f }, { width, 0.8f }, { master, -6.f } } },

        { "BR End Titles Pulse", "Synthwave", {
            { oscAWave, 1 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.75f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBFine, 6 }, { oscBLevel, 0.45f }, { subLevel, 0.3f }, { noiseLevel, 0.f }, { drift, 0.2f },
            { voiceMode, 1 }, { glide, 0.f },
            { filtType, 1 }, { filtCutoff, 650 }, { filtRes, 0.4f }, { filtEnv, 0.85f }, { filtKey, 0.3f }, { filtDrive, 0.4f },
            { ampA, 0.002f }, { ampD, 0.3f }, { ampS, 0.35f }, { ampR, 0.12f }, { fA, 0.002f }, { fD, 0.16f }, { fS, 0.08f }, { fR, 0.1f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 8000 }, { wow, 0.f }, { heat, 0.5f }, { dist, 0.1f }, { crackle, 0.f },
            { chorusMix, 0.15f }, { delayMix, 0.3f }, { delayTime, 0.375f }, { delayFb, 0.5f }, { verbMix, 0.25f }, { verbSize, 0.85f }, { width, 0.7f }, { master, -5.f } } },

        { "T Main Theme Lead", "Synthwave", {
            { oscAWave, 5 }, { oscAUnison, 2 }, { oscADetune, 12 }, { oscALevel, 0.65f },
            { oscBWave, 1 }, { oscBOct, 0 }, { oscBFine, 7 }, { oscBLevel, 0.55f },
            { subLevel, 0.15f }, { noiseLevel, 0.03f }, { drift, 0.25f },
            { voiceMode, 1 }, { glide, 0.06f },
            { filtType, 2 }, { filtCutoff, 1800 }, { filtRes, 0.55f }, { filtEnv, 0.5f }, { filtKey, 0.4f }, { filtDrive, 0.55f },
            { ampA, 0.01f }, { ampD, 1.2f }, { ampS, 0.8f }, { ampR, 0.6f }, { fA, 0.01f }, { fD, 0.6f }, { fS, 0.35f }, { fR, 0.4f },
            { lfoShape, 0 }, { lfoRate, 4.5f }, { lfoPitch, 0.02f }, { lfoFilter, 0.f },
            { crushBits, 13 }, { crushRate, 40000 }, { lofiTone, 10000 }, { wow, 0.04f }, { heat, 0.6f }, { dist, 0.25f }, { crackle, 0.f },
            { chorusMix, 0.25f }, { delayMix, 0.3f }, { delayTime, 0.5f }, { delayFb, 0.45f }, { verbMix, 0.45f }, { verbSize, 0.9f }, { verbDamp, 0.25f }, { width, 0.9f }, { master, -6.f } } },

        { "T Tech Noir Sequence", "Synthwave", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 4 }, { oscBOct, -1 }, { oscBFine, 9 }, { oscBLevel, 0.5f }, { subLevel, 0.35f }, { noiseLevel, 0.f }, { drift, 0.25f },
            { voiceMode, 1 }, { glide, 0.f },
            { filtType, 1 }, { filtCutoff, 450 }, { filtRes, 0.5f }, { filtEnv, 0.9f }, { filtKey, 0.3f }, { filtDrive, 0.55f },
            { ampA, 0.002f }, { ampD, 0.28f }, { ampS, 0.25f }, { ampR, 0.1f }, { fA, 0.002f }, { fD, 0.14f }, { fS, 0.05f }, { fR, 0.08f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 7000 }, { wow, 0.f }, { heat, 0.6f }, { dist, 0.25f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.35f }, { delayTime, 0.1875f }, { delayFb, 0.45f }, { verbMix, 0.15f }, { verbSize, 0.7f }, { width, 0.5f }, { master, -5.f } } },

        { "Africa Kalimba", "Synthwave", {
            { oscAWave, 5 }, { oscAOct, 1 }, { oscAUnison, 1 }, { oscALevel, 0.75f },
            { oscBWave, 3 }, { oscBOct, 1 }, { oscBLevel, 0.4f }, { subLevel, 0.f }, { noiseLevel, 0.f }, { drift, 0.05f },
            { filtType, 0 }, { filtCutoff, 5000 }, { filtRes, 0.05f }, { filtEnv, 0.4f }, { filtKey, 0.8f },
            { ampA, 0.001f }, { ampD, 0.7f }, { ampS, 0.f }, { ampR, 0.4f }, { fA, 0.001f }, { fD, 0.15f }, { fS, 0.f }, { fR, 0.2f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 11000 }, { wow, 0.03f }, { heat, 0.15f }, { crackle, 0.f },
            { chorusMix, 0.3f }, { chorusRate, 0.5f }, { chorusDepth, 0.35f }, { delayMix, 0.3f }, { delayTime, 0.375f }, { delayFb, 0.4f }, { verbMix, 0.35f }, { verbSize, 0.8f }, { width, 0.9f }, { master, -6.f } } },

        { "Axel F Lead", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 1 }, { oscALevel, 0.75f },
            { oscBWave, 1 }, { oscBOct, 0 }, { oscBFine, 8 }, { oscBLevel, 0.55f }, { subLevel, 0.1f }, { noiseLevel, 0.f }, { drift, 0.2f },
            { voiceMode, 1 }, { glide, 0.02f },
            { filtType, 1 }, { filtCutoff, 2600 }, { filtRes, 0.3f }, { filtEnv, 0.5f }, { filtKey, 0.5f }, { filtDrive, 0.3f },
            { ampA, 0.003f }, { ampD, 0.5f }, { ampS, 0.7f }, { ampR, 0.15f }, { fA, 0.003f }, { fD, 0.25f }, { fS, 0.3f }, { fR, 0.15f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 11000 }, { wow, 0.03f }, { heat, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.2f }, { delayMix, 0.25f }, { delayTime, 0.25f }, { delayFb, 0.35f }, { verbMix, 0.25f }, { verbSize, 0.75f }, { width, 0.6f }, { master, -6.f } } },

        { "Upside Down Arp", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 2 }, { oscADetune, 9 }, { oscALevel, 0.7f },
            { oscBWave, 1 }, { oscBOct, -1 }, { oscBLevel, 0.4f }, { subLevel, 0.15f }, { noiseLevel, 0.f }, { drift, 0.3f },
            { filtType, 1 }, { filtCutoff, 700 }, { filtRes, 0.35f }, { filtEnv, 0.75f }, { filtKey, 0.4f }, { filtDrive, 0.35f },
            { ampA, 0.002f }, { ampD, 0.45f }, { ampS, 0.15f }, { ampR, 0.3f }, { fA, 0.002f }, { fD, 0.2f }, { fS, 0.1f }, { fR, 0.2f },
            { lfoShape, 0 }, { lfoRate, 0.25f }, { lfoPitch, 0.f }, { lfoFilter, 0.2f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 9000 }, { wow, 0.05f }, { heat, 0.45f }, { crackle, 0.f },
            { chorusMix, 0.2f }, { delayMix, 0.35f }, { delayTime, 0.375f }, { delayFb, 0.5f }, { verbMix, 0.35f }, { verbSize, 0.85f }, { width, 0.9f }, { master, -6.f } } },

        { "Miami Vice Pad", "Synthwave", {
            { oscAWave, 0 }, { oscAUnison, 4 }, { oscADetune, 15 }, { oscALevel, 0.65f },
            { oscBWave, 5 }, { oscBOct, 1 }, { oscBLevel, 0.3f }, { subLevel, 0.2f }, { noiseLevel, 0.f }, { drift, 0.25f },
            { filtType, 0 }, { filtCutoff, 2000 }, { filtRes, 0.15f }, { filtEnv, 0.35f }, { filtKey, 0.3f },
            { ampA, 0.5f }, { ampD, 2.f }, { ampS, 0.9f }, { ampR, 2.f }, { fA, 1.f }, { fD, 2.f }, { fS, 0.5f }, { fR, 1.5f },
            { lfoShape, 0 }, { lfoRate, 0.3f }, { lfoPitch, 0.02f }, { lfoFilter, 0.1f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 11000 }, { wow, 0.05f }, { heat, 0.25f }, { crackle, 0.f },
            { chorusMix, 0.7f }, { chorusRate, 0.45f }, { chorusDepth, 0.55f }, { delayMix, 0.3f }, { delayTime, 0.5f }, { delayFb, 0.45f }, { verbMix, 0.55f }, { verbSize, 0.92f }, { width, 1.f }, { master, -7.f } } },

        { "Enjoy The Silence Lead", "Synthwave", {
            { oscAWave, 4 }, { oscAUnison, 2 }, { oscADetune, 7 }, { oscALevel, 0.75f },
            { oscBWave, 0 }, { oscBOct, 1 }, { oscBLevel, 0.3f }, { subLevel, 0.05f }, { noiseLevel, 0.f }, { drift, 0.15f },
            { voiceMode, 1 }, { glide, 0.03f },
            { filtType, 0 }, { filtCutoff, 2200 }, { filtRes, 0.3f }, { filtEnv, 0.5f }, { filtKey, 0.5f }, { filtDrive, 0.3f },
            { ampA, 0.004f }, { ampD, 0.7f }, { ampS, 0.75f }, { ampR, 0.3f }, { fA, 0.004f }, { fD, 0.35f }, { fS, 0.3f }, { fR, 0.25f },
            { lfoShape, 0 }, { lfoRate, 5.f }, { lfoPitch, 0.015f }, { lfoFilter, 0.f },
            { crushBits, 14 }, { crushRate, 44100 }, { lofiTone, 10000 }, { wow, 0.04f }, { heat, 0.35f }, { crackle, 0.f },
            { chorusMix, 0.35f }, { delayMix, 0.4f }, { delayTime, 0.375f }, { delayFb, 0.5f }, { verbMix, 0.4f }, { verbSize, 0.88f }, { width, 0.9f }, { master, -6.f } } },

        { "Moroder Disco Bass", "Synthwave", {
            { oscAWave, 0 }, { oscAOct, -1 }, { oscAUnison, 1 }, { oscALevel, 0.8f },
            { oscBWave, 0 }, { oscBOct, 0 }, { oscBFine, 4 }, { oscBLevel, 0.35f }, { subLevel, 0.35f }, { noiseLevel, 0.f }, { drift, 0.2f },
            { voiceMode, 1 }, { glide, 0.f },
            { filtType, 1 }, { filtCutoff, 550 }, { filtRes, 0.35f }, { filtEnv, 0.85f }, { filtKey, 0.3f }, { filtDrive, 0.45f },
            { ampA, 0.002f }, { ampD, 0.22f }, { ampS, 0.2f }, { ampR, 0.08f }, { fA, 0.002f }, { fD, 0.1f }, { fS, 0.05f }, { fR, 0.06f },
            { lfoPitch, 0.f }, { lfoFilter, 0.f },
            { crushBits, 16 }, { crushRate, 48000 }, { lofiTone, 8000 }, { wow, 0.f }, { heat, 0.5f }, { dist, 0.08f }, { crackle, 0.f },
            { chorusMix, 0.f }, { delayMix, 0.3f }, { delayTime, 0.1875f }, { delayFb, 0.35f }, { verbMix, 0.05f }, { width, 0.4f }, { master, -5.f } } },
    };
    return presets;
}
