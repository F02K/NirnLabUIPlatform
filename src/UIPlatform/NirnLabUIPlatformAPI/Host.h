#pragma once

#include <cstdint>

namespace NL::UI
{
    enum class HostRuntime : std::uint32_t
    {
        Unknown = 0,
        OblivionRemastered = 1,
    };

    struct HostInfo
    {
        std::uint32_t structSize = sizeof(HostInfo);
        HostRuntime runtime = HostRuntime::Unknown;
        const char* runtimeName = "Unknown";
        const char* gameName = "";
        const char* integrationName = "NirnLabUIPlatform";
    };

    inline constexpr HostInfo GetCompileTimeHostInfo()
    {
        return {
            sizeof(HostInfo),
            HostRuntime::OblivionRemastered,
            "OblivionRemastered",
            "Oblivion Remastered",
            "Oblivion Remastered"
        };
    }
}
