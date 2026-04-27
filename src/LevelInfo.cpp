#include "BeatSinger/LevelInfo.hpp"

#include <array>
#include <sstream>

namespace
{
    std::string Base64Encode(std::string const& input)
    {
        static constexpr char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string output;
        int value = 0;
        int bits = -6;

        for (unsigned char c : input)
        {
            value = (value << 8) + c;
            bits += 8;
            while (bits >= 0)
            {
                output.push_back(alphabet[(value >> bits) & 0x3f]);
                bits -= 6;
            }
        }

        if (bits > -6)
            output.push_back(alphabet[((value << 8) >> (bits + 8)) & 0x3f]);

        while (output.size() % 4 != 0)
            output.push_back('=');

        return output;
    }
}

namespace BeatSinger
{
    std::string LegacyLyricsHash(LevelInfo const& levelInfo)
    {
        std::ostringstream id;
        id << levelInfo.songName << ", "
           << levelInfo.songAuthorName << ", "
           << levelInfo.songSubName << ", "
           << levelInfo.beatsPerMinute << ", "
           << levelInfo.songDuration << ", "
           << levelInfo.songTimeOffset;

        return Base64Encode(id.str());
    }
}
