#include "BeatSinger/LyricsParser.hpp"

#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <regex>
#include <sstream>
#include <utility>

namespace
{
    float ParseSrtTime(std::smatch const& match, int offset)
    {
        float seconds = std::stof(match[offset + 3].str());
        return std::stof(match[offset + 1].str()) * 3600.0f
            + std::stof(match[offset + 2].str()) * 60.0f
            + seconds;
    }

    void TrimTrailingNewline(std::string& text)
    {
        while (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
            text.pop_back();
    }
}

namespace BeatSinger
{
    bool ParseLyricsJson(std::string const& json, std::vector<Subtitle>& subtitles, std::string& error)
    {
        rapidjson::Document document;
        document.Parse(json.c_str());

        if (document.HasParseError() || !document.IsArray())
        {
            error = "lyrics.json must contain a JSON array";
            return false;
        }

        std::vector<Subtitle> parsed;
        parsed.reserve(document.Size());

        for (auto const& value : document.GetArray())
        {
            if (!value.IsObject() || !value.HasMember("text") || !value["text"].IsString())
            {
                error = "lyrics.json entries must include string text";
                return false;
            }

            Subtitle subtitle;
            subtitle.text = value["text"].GetString();

            if (value.HasMember("startTimeSeconds") && value["startTimeSeconds"].IsNumber())
                subtitle.startTimeSeconds = value["startTimeSeconds"].GetFloat();
            else if (value.HasMember("time") && value["time"].IsNumber())
                subtitle.startTimeSeconds = value["time"].GetFloat();
            else if (value.HasMember("time") && value["time"].IsObject() && value["time"].HasMember("total") && value["time"]["total"].IsNumber())
                subtitle.startTimeSeconds = value["time"]["total"].GetFloat();
            else
            {
                error = "lyrics.json entries must include time or startTimeSeconds";
                return false;
            }

            if (value.HasMember("endTimeSeconds") && value["endTimeSeconds"].IsNumber())
                subtitle.endTimeSeconds = value["endTimeSeconds"].GetFloat();
            else if (value.HasMember("end") && value["end"].IsNumber())
                subtitle.endTimeSeconds = value["end"].GetFloat();

            parsed.push_back(std::move(subtitle));
        }

        std::sort(parsed.begin(), parsed.end(), [](Subtitle const& left, Subtitle const& right) {
            return left.startTimeSeconds < right.startTimeSeconds;
        });

        subtitles = std::move(parsed);
        return true;
    }

    bool ParseLyricsSrt(std::string const& srt, std::vector<Subtitle>& subtitles, std::string& error)
    {
        static std::regex const timingPattern(R"((\d+):(\d+):(\d+[,.]\d+) *--> *(\d+):(\d+):(\d+[,.]\d+))");

        std::istringstream input(srt);
        std::string line;
        std::vector<Subtitle> parsed;

        while (std::getline(input, line))
        {
            if (line.empty() || line == "\r")
                continue;

            std::string timingLine;
            if (std::all_of(line.begin(), line.end(), [](unsigned char c) { return std::isdigit(c) || c == '\r'; }))
            {
                if (!std::getline(input, timingLine))
                    break;
            }
            else
            {
                timingLine = line;
            }

            std::replace(timingLine.begin(), timingLine.end(), ',', '.');

            std::smatch match;
            if (!std::regex_search(timingLine, match, timingPattern))
            {
                error = "invalid SRT timing line";
                return false;
            }

            Subtitle subtitle;
            subtitle.startTimeSeconds = ParseSrtTime(match, 0);
            subtitle.endTimeSeconds = ParseSrtTime(match, 3);

            std::ostringstream text;
            while (std::getline(input, line))
            {
                if (line.empty() || line == "\r")
                    break;
                text << line << '\n';
            }

            subtitle.text = text.str();
            TrimTrailingNewline(subtitle.text);

            if (!subtitle.text.empty())
                parsed.push_back(std::move(subtitle));
        }

        std::sort(parsed.begin(), parsed.end(), [](Subtitle const& left, Subtitle const& right) {
            return left.startTimeSeconds < right.startTimeSeconds;
        });

        subtitles = std::move(parsed);
        return true;
    }
}
