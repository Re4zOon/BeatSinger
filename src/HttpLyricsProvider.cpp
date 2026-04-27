#include "BeatSinger/HttpLyricsProvider.hpp"

#include "BeatSinger/Config.hpp"
#include "BeatSinger/LevelInfo.hpp"
#include "BeatSinger/Logger.hpp"
#include "BeatSinger/LyricsParser.hpp"

#include "UnityEngine/Networking/DownloadHandler.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"

namespace BeatSinger
{
    namespace
    {
        std::string ToStdString(StringW value)
        {
            return value ? static_cast<std::string>(value) : std::string();
        }
    }

    std::string HttpLyricsProvider::Id() const
    {
        return "http";
    }

    bool HttpLyricsProvider::CanResolve(LevelInfo const&) const
    {
        return GetConfig().HttpEnabled.GetValue() && !GetConfig().HttpBaseUrl.GetValue().empty();
    }

    bool HttpLyricsProvider::Resolve(LevelInfo const& levelInfo, std::vector<Subtitle>& subtitles)
    {
        auto request = BeginResolve(levelInfo);
        if (!request)
            return false;

        GetLogger().warn("HTTP Resolve was called synchronously; request will be ignored to avoid blocking gameplay.");
        request->Dispose();
        subtitles.clear();
        return false;
    }

    UnityEngine::Networking::UnityWebRequest* HttpLyricsProvider::BeginResolve(LevelInfo const& levelInfo)
    {
        if (!CanResolve(levelInfo))
            return nullptr;

        auto baseUrl = GetConfig().HttpBaseUrl.GetValue();
        if (!baseUrl.empty() && baseUrl.back() == '/')
            baseUrl.pop_back();

        std::string url = baseUrl + "/" + LegacyLyricsHash(levelInfo);
        auto request = UnityEngine::Networking::UnityWebRequest::Get(StringW(url));
        request->set_timeout(GetConfig().HttpTimeoutSeconds.GetValue());
        request->SendWebRequest();
        return request;
    }

    bool HttpLyricsProvider::CompleteResolve(UnityEngine::Networking::UnityWebRequest* request, std::vector<Subtitle>& subtitles)
    {
        bool failed =
            request->get_result() != UnityEngine::Networking::UnityWebRequest::Result::Success ||
            request->get_responseCode() < 200 ||
            request->get_responseCode() >= 300;

        if (failed)
        {
            GetLogger().warn("HTTP lyrics request failed: {}", ToStdString(request->get_error()).c_str());
            request->Dispose();
            return false;
        }

        std::string body = ToStdString(request->get_downloadHandler()->get_text());
        std::string contentType = ToStdString(request->GetResponseHeader("Content-Type"));
        std::string error;
        bool parsed = contentType.find("json") != std::string::npos
            ? ParseLyricsJson(body, subtitles, error)
            : ParseLyricsSrt(body, subtitles, error);

        if (!parsed)
        {
            GetLogger().warn("HTTP lyrics response could not be parsed: {}", error.c_str());
            subtitles.clear();
        }

        request->Dispose();
        return parsed;
    }
}
