#pragma once
#include "Containers/String.h"
#include "Containers/StringView.h"

#include "Runtime/DialogFilters.h"

namespace Nova
{
    class Window;

    struct Path
    {
#ifdef NOVA_PLATFORM_WINDOWS
        static inline String::CharacterType s_Separator = '\\';
        static inline String::CharacterType s_OtherSeparator = '/';

#else
        static inline String::CharacterType s_Separator = '/';
        static inline String::CharacterType s_OtherSeparator = '\\';
#endif
        static String Combine(const StringView path, const StringView other);

        template<typename... Args>
        static String Combine(const StringView path, const StringView other, const Args&... args)
        {
            return Combine(Combine(path, other), args...);
        }


        static StringView GetEngineDirectory();
        static String GetEngineAssetsDirectory();
        static String GetEngineAssetPath(const StringView filepath);

#ifdef NOVA_CLIENT
        static StringView GetApplicationDirectory()
        {
            return NOVA_APPLICATION_DIR;
        }

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
        static bool IsFile(StringView path);
        static bool IsDirectory(StringView path);
        static Array<String> GetFiles(StringView path);
    };
}
