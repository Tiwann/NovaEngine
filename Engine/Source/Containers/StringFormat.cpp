#include "StringFormat.h"


std::format_context::iterator std::formatter<Nova::String>::format(const Nova::String& Str, format_context& Context) const
{
    return formatter<string_view>::format(string_view(Str.Data(), Str.Count()), Context);
}

std::format_context::iterator std::formatter<Nova::StringView>::format(const Nova::StringView& Str,format_context& Context) const
{
    return formatter<string_view>::format(string_view(Str.Data(), Str.Count()), Context);
}
