#pragma once

#include <string>
#include <utility>
#include <vector>

namespace BeatSinger
{
    template <class T>
    class ConfigValue
    {
    public:
        ConfigValue() = default;

        explicit ConfigValue(T value) : value(std::move(value))
        {
        }

        T const& GetValue() const
        {
            return value;
        }

        void SetValue(T newValue)
        {
            value = std::move(newValue);
        }

    private:
        T value;
    };

    class BeatSingerConfig
    {
    public:
        ConfigValue<bool> Enabled{true};
        ConfigValue<float> DisplayDelay{-0.1f};
        ConfigValue<float> HideDelay{0.0f};
        ConfigValue<float> VerticalOffset{4.0f};
        ConfigValue<float> TextScale{0.42f};
        ConfigValue<std::vector<std::string>> ProviderOrder{std::vector<std::string>({"local", "http"})};
        ConfigValue<bool> HttpEnabled{false};
        ConfigValue<std::string> HttpBaseUrl{""};
        ConfigValue<int> HttpTimeoutSeconds{8};

        void Load();
        void Save() const;
    };

    BeatSingerConfig& GetConfig();
}
