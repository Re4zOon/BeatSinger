#include "BeatSinger/LocalFileLyricsProvider.hpp"

#include "BeatSinger/Logger.hpp"
#include "BeatSinger/LyricsParser.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace
{
    constexpr auto FallbackCustomLevelsPath = "/sdcard/ModData/com.beatgames.beatsaber/Mods/SongCore/CustomLevels";

    bool ReadFile(std::filesystem::path const& path, std::string& content)
    {
        std::ifstream stream(path, std::ios::in | std::ios::binary);
        if (!stream)
            return false;

        std::ostringstream buffer;
        buffer << stream.rdbuf();
        content = buffer.str();
        return true;
    }
}

namespace BeatSinger
{
    std::string LocalFileLyricsProvider::Id() const
    {
        return "local";
    }

    bool LocalFileLyricsProvider::CanResolve(LevelInfo const& levelInfo) const
    {
        return !levelInfo.customLevelPath.empty() || !levelInfo.levelId.empty();
    }

    bool LocalFileLyricsProvider::Resolve(LevelInfo const& levelInfo, std::vector<Subtitle>& subtitles)
    {
        if (!levelInfo.customLevelPath.empty() && ResolveInDirectory(levelInfo.customLevelPath, subtitles))
            return true;

        std::string fallback = ResolveFallbackDirectory(levelInfo);
        if (!fallback.empty() && ResolveInDirectory(fallback, subtitles))
            return true;

        return false;
    }

    bool LocalFileLyricsProvider::ResolveInDirectory(std::string const& directory, std::vector<Subtitle>& subtitles) const
    {
        std::filesystem::path base(directory);
        std::string content;
        std::string error;

        auto jsonPath = base / "lyrics.json";
        if (std::filesystem::exists(jsonPath) && ReadFile(jsonPath, content))
        {
            if (ParseLyricsJson(content, subtitles, error))
            {
                GetLogger().info("Loaded local lyrics.json from {}", jsonPath.c_str());
                return true;
            }

            GetLogger().warn("Failed to parse {}: {}", jsonPath.c_str(), error.c_str());
            subtitles.clear();
            return false;
        }

        auto srtPath = base / "lyrics.srt";
        if (std::filesystem::exists(srtPath) && ReadFile(srtPath, content))
        {
            if (ParseLyricsSrt(content, subtitles, error))
            {
                GetLogger().info("Loaded local lyrics.srt from {}", srtPath.c_str());
                return true;
            }

            GetLogger().warn("Failed to parse {}: {}", srtPath.c_str(), error.c_str());
            subtitles.clear();
            return false;
        }

        return false;
    }

    std::string LocalFileLyricsProvider::ResolveFallbackDirectory(LevelInfo const& levelInfo) const
    {
        std::filesystem::path root(FallbackCustomLevelsPath);
        if (!std::filesystem::exists(root))
            return {};

        for (auto const& entry : std::filesystem::directory_iterator(root))
        {
            if (!entry.is_directory())
                continue;

            auto folder = entry.path().filename().string();
            if (!levelInfo.levelId.empty() && folder.find(levelInfo.levelId) != std::string::npos)
                return entry.path().string();
        }

        return {};
    }
}
