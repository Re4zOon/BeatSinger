#pragma once

#include "BeatSinger/Subtitle.hpp"

#include <string>
#include <vector>

namespace BeatSinger
{
    bool ParseLyricsJson(std::string const& json, std::vector<Subtitle>& subtitles, std::string& error);
    bool ParseLyricsSrt(std::string const& srt, std::vector<Subtitle>& subtitles, std::string& error);
}
