#pragma once

#include "BeatSinger/LevelInfo.hpp"
#include "BeatSinger/Subtitle.hpp"

#include "UnityEngine/Networking/UnityWebRequest.hpp"

#include <string>
#include <vector>

namespace BeatSinger
{
    class LyricsProvider
    {
    public:
        virtual ~LyricsProvider() = default;
        virtual std::string Id() const = 0;
        virtual bool CanResolve(LevelInfo const& levelInfo) const = 0;
        virtual bool Resolve(LevelInfo const& levelInfo, std::vector<Subtitle>& subtitles) = 0;
        virtual UnityEngine::Networking::UnityWebRequest* BeginResolve(LevelInfo const& levelInfo);
        virtual bool CompleteResolve(UnityEngine::Networking::UnityWebRequest* request, std::vector<Subtitle>& subtitles);
    };
}
