#pragma once

#include <string>

namespace BeatSinger
{
    struct LevelInfo
    {
        std::string levelId;
        std::string songName;
        std::string songAuthorName;
        std::string songSubName;
        float beatsPerMinute = 0.0f;
        float songDuration = 0.0f;
        float songTimeOffset = 0.0f;
        std::string customLevelPath;
    };

    std::string LegacyLyricsHash(LevelInfo const& levelInfo);
}
