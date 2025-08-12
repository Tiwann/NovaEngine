#pragma once
#include <cstdint>

namespace Nova
{
    struct Version
    {
        uint16_t minor = 0;
        uint16_t major = 0;

        constexpr Version() = default;
        constexpr Version(const uint16_t major, const uint16_t minor) : minor(minor), major(major) {}
        constexpr Version(const uint32_t version) : minor(version & 0xFFFF), major(version >> 16) {}

        constexpr operator uint32_t() const { return major << 16 | minor; }
    };

    static constexpr Version NOVA_VERSION = { 0, 1 };
}
