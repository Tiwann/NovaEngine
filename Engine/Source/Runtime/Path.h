#pragma once
#include "Window.h"
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Containers/StringFormat.h"
#include "Runtime/DialogFilters.h"

namespace Nova
{
    struct Path
    {
        static String Combine(const StringView path, const StringView other)
        {
            return StringFormat("{}{}{}", path, s_Separator,other).ReplaceAll('/', '\\');
        }

        template<typename... Args>
        static String Combine(const StringView path, Args&&... args)
        {
            return StringFormat("{}{}{}", path, s_Separator, std::forward<Args>(args)...).ReplaceAll('/', '\\');
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

        static String OpenFileDialog(StringView title, StringView defaultPath, const DialogFilters& filters, Window& owningWindow);
        static String SaveFileDialog(StringView title, StringView defaultPath, const DialogFilters& filters, Window& owningWindow);

#ifdef NOVA_PLATFORM_WINDOWS
        static inline String::CharacterType s_Separator = '\\';
#else
        static inline String::CharacterType s_Separator = '/';
#endif
    };
}
