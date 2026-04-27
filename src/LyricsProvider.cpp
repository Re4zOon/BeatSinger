#include "BeatSinger/LyricsProvider.hpp"

namespace BeatSinger
{
    UnityEngine::Networking::UnityWebRequest* LyricsProvider::BeginResolve(LevelInfo const&)
    {
        return nullptr;
    }

    bool LyricsProvider::CompleteResolve(UnityEngine::Networking::UnityWebRequest*, std::vector<Subtitle>&)
    {
        return false;
    }
}
