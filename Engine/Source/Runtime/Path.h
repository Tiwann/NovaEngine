#pragma once
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Containers/StringFormat.h"
#include "Runtime/DialogFilters.h"

namespace Nova
{
    class Window;

    struct Path
    {
        static String Combine(const StringView path, const StringView other)
        {
            return StringFormat("{}{}{}", path, s_Separator,other).ReplaceAll('/', s_Separator);
        }

        template<typename... Args>
        static String Combine(const StringView path, const StringView other, const Args&... args)
        {
            return Combine(Combine(path, other), args...);
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

        static StringView GetUserDirectory();
        static StringView GetDocumentsDirectory();
        static StringView GetMusicDirectory();
        static StringView GetDownloadsDirectory();
        static StringView GetDesktopDirectory();

        static String OpenFileDialog(StringView title, StringView defaultPath, const DialogFilters& filters, Window& owningWindow);
        static String SaveFileDialog(StringView title, StringView defaultPath, const DialogFilters& filters, Window& owningWindow);
        static bool Exists(StringView path);
#ifdef NOVA_PLATFORM_WINDOWS
        static inline String::CharacterType s_Separator = '\\';
#else
        static inline String::CharacterType s_Separator = '/';
#endif
    };
}
