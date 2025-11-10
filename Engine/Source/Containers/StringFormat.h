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
    String StringFormat(const StringView& format, const Args&... args)
    {
        std::string formatted = std::vformat(static_cast<std::string_view>(format), std::make_format_args(args...));
        return {formatted.data(), formatted.size()};
    }

    template<Character T, typename... Args>
    uint32_t StringScanf(const StringView string, const StringViewBase<T> format, Args*... args)
    {
        if constexpr (typeid(T) == typeid(char))
        {
            return std::sscanf(string.Data(), format.Data(), args...);
        } else if constexpr (typeid(T) == typeid(wchar_t))
        {
            return std::swscanf(string.Data(), format.Data(), args...);
        } else
        {
            static_assert(false, "Cannot sscanf with this string type!");
            return 0;
        }
    }
}
