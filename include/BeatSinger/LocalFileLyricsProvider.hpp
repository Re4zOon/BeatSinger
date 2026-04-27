#pragma once

#include "BeatSinger/LyricsProvider.hpp"

namespace BeatSinger
{
    class LocalFileLyricsProvider final : public LyricsProvider
    {
    public:
        std::string Id() const override;
        bool CanResolve(LevelInfo const& levelInfo) const override;
        bool Resolve(LevelInfo const& levelInfo, std::vector<Subtitle>& subtitles) override;

    private:
        bool ResolveInDirectory(std::string const& directory, std::vector<Subtitle>& subtitles) const;
        std::string ResolveFallbackDirectory(LevelInfo const& levelInfo) const;
    };
}
