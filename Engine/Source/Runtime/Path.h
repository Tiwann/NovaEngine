#pragma once
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Containers/StringFormat.h"

namespace Nova
{
    struct Path
    {
        static String Combine(StringView path, StringView other)
        {
            return StringFormat("{}/{}", path, other);
        }

        template<typename... Args>
        static String Combine(StringView path, StringView other, Args&&... args)
        {
            return StringFormat("{}/{}", StringFormat("{}/{}", path, other), std::forward<Args>(args)...);
        }
    };
}
