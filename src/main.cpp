#include "BeatSinger/Config.hpp"
#include "BeatSinger/LyricsController.hpp"
#include "BeatSinger/Logger.hpp"
#include "BeatSinger/SettingsViewController.hpp"

#include "GlobalNamespace/GameCoreSceneSetup.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "custom-types/shared/register.hpp"
#include "bsml/shared/BSML.hpp"

using namespace BeatSinger;

namespace
{
    std::string ToStdString(StringW value)
    {
        return value ? static_cast<std::string>(value) : std::string();
    }
}

MAKE_HOOK_MATCH(GameCoreSceneSetup_InstallBindings, &GlobalNamespace::GameCoreSceneSetup::InstallBindings, void, GlobalNamespace::GameCoreSceneSetup* self)
{
    GameCoreSceneSetup_InstallBindings(self);
    AttachLyricsController();
}

MAKE_HOOK_MATCH(SceneManager_SetActiveScene, &UnityEngine::SceneManagement::SceneManager::SetActiveScene, bool, UnityEngine::SceneManagement::Scene scene)
{
    bool result = SceneManager_SetActiveScene(scene);
    std::string sceneName = ToStdString(scene.get_name());

    if (sceneName != "GameCore")
        DestroyLyricsController();

    return result;
}

extern "C" void setup(CModInfo& info)
{
    info.id = MOD_ID;
    info.version = VERSION;
    GetConfig().Load();
    GetLogger().info("Completed setup for {} {}.", MOD_ID, VERSION);
}

extern "C" void load()
{
    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();
    BSML::Init();

    INSTALL_HOOK(GetLogger(), GameCoreSceneSetup_InstallBindings);
    INSTALL_HOOK(GetLogger(), SceneManager_SetActiveScene);

    BSML::Register::RegisterSettingsMenu<BeatSinger::SettingsViewController*>("Beat Singer", true);

    GetLogger().info("BeatSinger loaded for Quest Beat Saber 1.40.8_7379.");
}
