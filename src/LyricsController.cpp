#include "BeatSinger/LyricsController.hpp"

#include "BeatSinger/Config.hpp"
#include "BeatSinger/LocalFileLyricsProvider.hpp"
#include "BeatSinger/Logger.hpp"

#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/BeatmapLevel.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "shared/SongLoader/CustomBeatmapLevel.hpp"
#include "TMPro/TextAlignmentOptions.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"

#include <algorithm>
#include <memory>

DEFINE_TYPE(BeatSinger, LyricsController);

namespace
{
    BeatSinger::LyricsController* activeController = nullptr;

    std::string ToStdString(StringW value)
    {
        return value ? static_cast<std::string>(value) : std::string();
    }

    template <class TObject>
    TObject* FindObject()
    {
        return UnityEngine::Object::FindObjectOfType<TObject*>();
    }

    BeatSinger::LevelInfo BuildLevelInfo()
    {
        BeatSinger::LevelInfo info;
        auto setupData = FindObject<GlobalNamespace::GameplayCoreSceneSetupData>();
        if (!setupData)
            return info;

        auto level = setupData->__cordl_internal_get_beatmapLevel();
        if (!level)
            return info;

        info.levelId = ToStdString(level->__cordl_internal_get_levelID());
        info.songName = ToStdString(level->__cordl_internal_get_songName());
        info.songAuthorName = ToStdString(level->__cordl_internal_get_songAuthorName());
        info.songSubName = ToStdString(level->__cordl_internal_get_songSubName());
        info.beatsPerMinute = level->__cordl_internal_get_beatsPerMinute();
        info.songDuration = level->__cordl_internal_get_songDuration();
        info.songTimeOffset = level->__cordl_internal_get_songTimeOffset();

        auto customLevel = il2cpp_utils::try_cast<SongCore::SongLoader::CustomBeatmapLevel>(level).value_or(nullptr);
        if (customLevel)
            info.customLevelPath = std::string(customLevel->get_customLevelPath());

        return info;
    }
}

namespace BeatSinger
{
    void AttachLyricsController()
    {
        if (activeController)
            return;

        auto host = UnityEngine::GameObject::New_ctor("BeatSinger");
        UnityEngine::Object::DontDestroyOnLoad(host);
        activeController = host->AddComponent<BeatSinger::LyricsController*>();
    }

    void DestroyLyricsController()
    {
        if (!activeController)
            return;

        auto gameObject = activeController->get_gameObject();
        UnityEngine::Object::Destroy(gameObject);
        activeController = nullptr;
    }
}

namespace BeatSinger
{
    void LyricsController::Awake()
    {
        activeUntil = 0.0f;
        activeSubtitleIndex = 0;
        lyricsLoaded = false;
        levelInfo = BuildLevelInfo();
        audioTimeSyncController = FindObject<GlobalNamespace::AudioTimeSyncController>();

        BuildTextObject();
        LoadLyrics();
    }

    void LyricsController::OnDestroy()
    {
        if (pendingHttpRequest)
        {
            pendingHttpRequest->Abort();
            pendingHttpRequest->Dispose();
            pendingHttpRequest = nullptr;
        }

        ClearText();
        if (activeController == this)
            activeController = nullptr;
    }

    void LyricsController::BuildTextObject()
    {
        auto textObject = UnityEngine::GameObject::New_ctor("BeatSinger Lyrics");
        textObject->get_transform()->SetParent(get_transform(), false);
        textObject->get_transform()->set_localPosition(UnityEngine::Vector3(0.0f, GetConfig().VerticalOffset.GetValue(), 0.0f));
        textObject->get_transform()->set_localRotation(UnityEngine::Quaternion::get_identity());
        float textScale = GetConfig().TextScale.GetValue();
        textObject->get_transform()->set_localScale(UnityEngine::Vector3(textScale, textScale, textScale));

        textMesh = textObject->AddComponent<TMPro::TextMeshPro*>();
        textMesh->set_alignment(TMPro::TextAlignmentOptions::Center);
        textMesh->set_fontSize(4.0f);
        textMesh->set_enableWordWrapping(true);
        textMesh->set_color(UnityEngine::Color::get_white());
        textMesh->set_text("");
    }

    void LyricsController::LoadLyrics()
    {
        if (!GetConfig().Enabled.GetValue())
        {
            GetLogger().info("BeatSinger is disabled by config.");
            return;
        }

        if (!audioTimeSyncController)
        {
            GetLogger().warn("AudioTimeSyncController was not found; lyrics will not display.");
            return;
        }

        std::vector<std::unique_ptr<LyricsProvider>> providers;
        providers.emplace_back(std::make_unique<LocalFileLyricsProvider>());

        for (auto const& providerId : GetConfig().ProviderOrder.GetValue())
        {
            if (providerId == "http")
            {
                pendingHttpRequest = httpProvider.BeginResolve(levelInfo);
                if (pendingHttpRequest)
                {
                    GetLogger().info("Started HTTP lyrics lookup.");
                    return;
                }

                continue;
            }

            auto provider = std::find_if(providers.begin(), providers.end(), [&](auto const& candidate) {
                return candidate->Id() == providerId;
            });

            if (provider == providers.end() || !(*provider)->CanResolve(levelInfo))
                continue;

            subtitles.clear();
            if ((*provider)->Resolve(levelInfo, subtitles) && !subtitles.empty())
            {
                lyricsLoaded = true;
                activeSubtitleIndex = 0;
                GetLogger().info("Loaded {} subtitles from {} provider.", subtitles.size(), (*provider)->Id().c_str());
                return;
            }
        }

        GetLogger().info("No lyrics found for {} - {}.", levelInfo.songAuthorName.c_str(), levelInfo.songName.c_str());
    }

    void LyricsController::Update()
    {
        if (pendingHttpRequest && pendingHttpRequest->get_isDone())
        {
            subtitles.clear();
            if (httpProvider.CompleteResolve(pendingHttpRequest, subtitles) && !subtitles.empty())
            {
                lyricsLoaded = true;
                activeSubtitleIndex = 0;
                GetLogger().info("Loaded {} subtitles from http provider.", subtitles.size());
            }
            else
            {
                GetLogger().info("HTTP provider did not resolve lyrics.");
            }

            pendingHttpRequest = nullptr;
        }

        if (!lyricsLoaded || !GetConfig().Enabled.GetValue() || !audioTimeSyncController)
            return;

        float songTime = audioTimeSyncController->get_songTime();

        if (activeUntil > 0.0f && songTime >= activeUntil)
            ClearText();

        while (activeSubtitleIndex < static_cast<int>(subtitles.size()) &&
               subtitles[activeSubtitleIndex].startTimeSeconds + GetConfig().DisplayDelay.GetValue() <= songTime)
        {
            auto const& subtitle = subtitles[activeSubtitleIndex];
            float endTime = subtitle.HasEndTime()
                ? subtitle.endTimeSeconds
                : (activeSubtitleIndex + 1 < static_cast<int>(subtitles.size())
                    ? subtitles[activeSubtitleIndex + 1].startTimeSeconds
                    : songTime + 3.0f);

            float duration = std::max(0.1f, endTime - songTime + GetConfig().HideDelay.GetValue());
            ShowText(StringW(subtitle.text), duration);
            activeSubtitleIndex++;
        }
    }

    void LyricsController::ShowText(StringW text, float durationSeconds)
    {
        if (!textMesh)
            return;

        textMesh->set_text(text);
        activeUntil = audioTimeSyncController ? audioTimeSyncController->get_songTime() + durationSeconds : 0.0f;
    }

    void LyricsController::ClearText()
    {
        if (textMesh)
            textMesh->set_text("");
        activeUntil = 0.0f;
    }
}
