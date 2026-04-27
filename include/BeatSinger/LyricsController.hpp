#pragma once

#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "TMPro/TextMeshPro.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"

#include "BeatSinger/LevelInfo.hpp"
#include "BeatSinger/HttpLyricsProvider.hpp"
#include "BeatSinger/Subtitle.hpp"

#include <memory>
#include <vector>

DECLARE_CLASS_CODEGEN(BeatSinger, LyricsController, UnityEngine::MonoBehaviour) {
    DECLARE_DEFAULT_CTOR();
    DECLARE_SIMPLE_DTOR();

    DECLARE_INSTANCE_FIELD(GlobalNamespace::AudioTimeSyncController*, audioTimeSyncController);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshPro*, textMesh);
    DECLARE_INSTANCE_FIELD(float, activeUntil);
    DECLARE_INSTANCE_FIELD(int, activeSubtitleIndex);
    DECLARE_INSTANCE_FIELD(bool, lyricsLoaded);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);
    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_INSTANCE_METHOD(void, LoadLyrics);
    DECLARE_INSTANCE_METHOD(void, ClearText);
    DECLARE_INSTANCE_METHOD(void, ShowText, StringW text, float durationSeconds);
    DECLARE_INSTANCE_METHOD(void, BuildTextObject);

public:
    std::vector<BeatSinger::Subtitle> subtitles;
    BeatSinger::LevelInfo levelInfo;
    UnityEngine::Networking::UnityWebRequest* pendingHttpRequest = nullptr;
    BeatSinger::HttpLyricsProvider httpProvider;
};

namespace BeatSinger
{
    void AttachLyricsController();
    void DestroyLyricsController();
}
