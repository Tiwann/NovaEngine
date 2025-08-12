#pragma once
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Containers/StringFormat.h"

namespace Nova
{
    struct Path
    {
        static String Combine(const StringView path, const StringView other)
        {
            return StringFormat("{}/{}", path, other);
        }

        template<typename... Args>
        static String Combine(const StringView path, const StringView other, Args&&... args)
        {
            return StringFormat("{}/{}", StringFormat("{}/{}", path, other), std::forward<Args>(args)...);
        }

        static StringView GetEngineDirectory() { return NOVA_ENGINE_ROOT_DIR; }
        static String GetEngineAssetsDirectory() { return Combine(GetEngineDirectory(), "Assets"); }
        static String GetEngineAssetPath(const StringView filepath) { return Combine(GetEngineAssetsDirectory(), filepath); }

        #ifdef NOVA_CLIENT
        static String GetAssetPath(const StringView filepath)
        {
            return Combine(StringView(NOVA_APPLICATION_DIR), "Assets", filepath);
        }
        #endif
    };
}
