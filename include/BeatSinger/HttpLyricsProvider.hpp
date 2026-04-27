#pragma once

#include "BeatSinger/LyricsProvider.hpp"

namespace BeatSinger
{
    class HttpLyricsProvider final : public LyricsProvider
    {
    public:
        std::string Id() const override;
        bool CanResolve(LevelInfo const& levelInfo) const override;
        bool Resolve(LevelInfo const& levelInfo, std::vector<Subtitle>& subtitles) override;
        UnityEngine::Networking::UnityWebRequest* BeginResolve(LevelInfo const& levelInfo) override;
        bool CompleteResolve(UnityEngine::Networking::UnityWebRequest* request, std::vector<Subtitle>& subtitles) override;
    };
}
