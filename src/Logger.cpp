#include "BeatSinger/Logger.hpp"

static Paper::LoggerContext logger("BeatSinger");

namespace BeatSinger
{
    Paper::LoggerContext& GetLogger()
    {
        return logger;
    }
}
