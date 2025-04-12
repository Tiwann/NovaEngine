#pragma once
#include "String.h"
#include "StringView.h"
#include <format>

template<>
struct std::formatter<Nova::String> : std::formatter<std::string_view>
{
    std::format_context::iterator format(const Nova::String& Str, format_context& Context) const;
};

template<>
struct std::formatter<Nova::StringView> : std::formatter<std::string_view>
{
    std::format_context::iterator format(const Nova::StringView& Str, format_context& Context) const;
};

namespace Nova
{
    template <typename... Args>
    String StringFormat(const StringView& Fmt, const Args&... Arguments)
    {
        std::string Formatted = std::vformat(static_cast<std::string_view>(Fmt), std::make_format_args(Arguments...));
        return {Formatted.data(), Formatted.size()};
    }
}
