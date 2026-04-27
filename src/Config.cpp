#include "BeatSinger/Config.hpp"

#include "BeatSinger/Logger.hpp"

#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/prettywriter.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/stringbuffer.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace
{
    constexpr auto ConfigPath = "/sdcard/ModData/com.beatgames.beatsaber/Configs/BeatSinger.json";

    bool ReadFile(std::string const& path, std::string& content)
    {
        std::ifstream stream(path, std::ios::in | std::ios::binary);
        if (!stream)
            return false;

        std::ostringstream buffer;
        buffer << stream.rdbuf();
        content = buffer.str();
        return true;
    }

    template <class T>
    void ReadValue(rapidjson::Value const& object, char const* key, BeatSinger::ConfigValue<T>& target)
    {
    }

    template <>
    void ReadValue<bool>(rapidjson::Value const& object, char const* key, BeatSinger::ConfigValue<bool>& target)
    {
        if (object.HasMember(key) && object[key].IsBool())
            target.SetValue(object[key].GetBool());
    }

    template <>
    void ReadValue<float>(rapidjson::Value const& object, char const* key, BeatSinger::ConfigValue<float>& target)
    {
        if (object.HasMember(key) && object[key].IsNumber())
            target.SetValue(object[key].GetFloat());
    }

    template <>
    void ReadValue<int>(rapidjson::Value const& object, char const* key, BeatSinger::ConfigValue<int>& target)
    {
        if (object.HasMember(key) && object[key].IsInt())
            target.SetValue(object[key].GetInt());
    }

    template <>
    void ReadValue<std::string>(rapidjson::Value const& object, char const* key, BeatSinger::ConfigValue<std::string>& target)
    {
        if (object.HasMember(key) && object[key].IsString())
            target.SetValue(object[key].GetString());
    }

    void ReadProviderOrder(rapidjson::Value const& object, BeatSinger::ConfigValue<std::vector<std::string>>& target)
    {
        if (!object.HasMember("providerOrder") || !object["providerOrder"].IsArray())
            return;

        std::vector<std::string> providers;
        for (auto const& value : object["providerOrder"].GetArray())
        {
            if (value.IsString())
                providers.emplace_back(value.GetString());
        }

        if (!providers.empty())
            target.SetValue(std::move(providers));
    }

    template <class Writer>
    void WriteStringArray(Writer& writer, char const* key, std::vector<std::string> const& values)
    {
        writer.Key(key);
        writer.StartArray();
        for (auto const& value : values)
            writer.String(value.c_str());
        writer.EndArray();
    }
}

namespace BeatSinger
{
    void BeatSingerConfig::Load()
    {
        std::string content;
        if (!ReadFile(ConfigPath, content))
        {
            Save();
            return;
        }

        rapidjson::Document document;
        document.Parse(content.c_str());
        if (document.HasParseError() || !document.IsObject())
        {
            GetLogger().warn("BeatSinger config is invalid; defaults will be used.");
            Save();
            return;
        }

        ReadValue(document, "enabled", Enabled);
        ReadValue(document, "displayDelay", DisplayDelay);
        ReadValue(document, "hideDelay", HideDelay);
        ReadValue(document, "verticalOffset", VerticalOffset);
        ReadValue(document, "textScale", TextScale);
        ReadProviderOrder(document, ProviderOrder);
        ReadValue(document, "httpEnabled", HttpEnabled);
        ReadValue(document, "httpBaseUrl", HttpBaseUrl);
        ReadValue(document, "httpTimeoutSeconds", HttpTimeoutSeconds);
    }

    void BeatSingerConfig::Save() const
    {
        std::filesystem::create_directories(std::filesystem::path(ConfigPath).parent_path());

        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        writer.StartObject();
        writer.Key("enabled");
        writer.Bool(Enabled.GetValue());
        writer.Key("displayDelay");
        writer.Double(DisplayDelay.GetValue());
        writer.Key("hideDelay");
        writer.Double(HideDelay.GetValue());
        writer.Key("verticalOffset");
        writer.Double(VerticalOffset.GetValue());
        writer.Key("textScale");
        writer.Double(TextScale.GetValue());
        WriteStringArray(writer, "providerOrder", ProviderOrder.GetValue());
        writer.Key("httpEnabled");
        writer.Bool(HttpEnabled.GetValue());
        writer.Key("httpBaseUrl");
        writer.String(HttpBaseUrl.GetValue().c_str());
        writer.Key("httpTimeoutSeconds");
        writer.Int(HttpTimeoutSeconds.GetValue());
        writer.EndObject();

        std::ofstream stream(ConfigPath, std::ios::out | std::ios::binary | std::ios::trunc);
        stream << buffer.GetString();
    }

    BeatSingerConfig& GetConfig()
    {
        static BeatSingerConfig config;
        return config;
    }
}
