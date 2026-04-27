#pragma once

#include <string>

namespace BeatSinger
{
    struct Subtitle
    {
        std::string text;
        float startTimeSeconds = 0.0f;
        float endTimeSeconds = -1.0f;

        bool HasEndTime() const
        {
            return endTimeSeconds >= 0.0f;
        }
    };
}
