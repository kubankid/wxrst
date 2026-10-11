#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <string>

// User presets live as XML files in ~/Documents/WXRST/Presets. They show up in the "User" category.
struct UserPreset
{
    juce::String name;
    juce::File file;
    std::vector<std::pair<std::string, float>> values; // param id -> real-world value
};

namespace UserPresets
{
    inline juce::File directory()
    {
        auto d = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory).getChildFile ("WXRST").getChildFile ("Presets");
        d.createDirectory();
        return d;
    }

    inline std::vector<UserPreset> loadAll()
    {
        std::vector<UserPreset> out;
        for (auto& f : directory().findChildFiles (juce::File::findFiles, false, "*.wxrst"))
        {
            auto xml = juce::XmlDocument::parse (f);
            if (xml == nullptr || ! xml->hasTagName ("WXRSTPreset")) continue;
            UserPreset p;
            p.name = xml->getStringAttribute ("name", f.getFileNameWithoutExtension());
            p.file = f;
            for (auto* e : xml->getChildWithTagNameIterator ("param"))
                p.values.push_back ({ e->getStringAttribute ("id").toStdString(), (float) e->getDoubleAttribute ("value") });
            out.push_back (std::move (p));
        }
        std::sort (out.begin(), out.end(), [] (auto& a, auto& b) { return a.name.compareIgnoreCase (b.name) < 0; });
        return out;
    }

    inline juce::File save (const juce::String& name, juce::AudioProcessorValueTreeState& apvts)
    {
        juce::XmlElement xml ("WXRSTPreset");
        xml.setAttribute ("name", name);
        for (auto* p : apvts.processor.getParameters())
            if (auto* rp = dynamic_cast<juce::RangedAudioParameter*> (p))
            {
                auto* e = xml.createNewChildElement ("param");
                e->setAttribute ("id", rp->paramID);
                e->setAttribute ("value", (double) rp->convertFrom0to1 (rp->getValue()));
            }
        auto f = directory().getChildFile (juce::File::createLegalFileName (name) + ".wxrst");
        xml.writeTo (f);
        return f;
    }
}
