#include "BeatSinger/SettingsViewController.hpp"

#include "BeatSinger/Config.hpp"

#include "bsml/shared/BSML-Lite.hpp"

DEFINE_TYPE(BeatSinger, SettingsViewController);

namespace BeatSinger
{
    void SettingsViewController::DidActivate(bool firstActivation, bool, bool)
    {
        if (!firstActivation)
            return;

        auto container = BSML::Lite::CreateScrollableSettingsContainer(get_transform());
        BSML::Lite::CreateText(container->get_transform(), "Beat Singer");
        BSML::Lite::CreateText(container->get_transform(), GetConfig().Enabled.GetValue() ? "Lyrics enabled" : "Lyrics disabled");
        BSML::Lite::CreateText(container->get_transform(), GetConfig().HttpEnabled.GetValue() ? "HTTP provider enabled" : "HTTP provider disabled");

        // The config keys are intentionally stable so existing Quest installs can
        // keep settings across future BSML layout changes.
        GetConfig().Save();
    }
}
