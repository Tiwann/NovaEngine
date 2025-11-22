#include "Runtime/Path.h"
#include "Containers/StringConversion.h"
#include <Windows.h>


namespace Nova
{
    StringView Path::GetUserDirectory()
    {
        static String userDirectory;
        if (userDirectory.IsEmpty())
        {
            WideString::CharacterType buffer[MAX_PATH] = {0};
            const DWORD length = GetEnvironmentVariableW(L"USERPROFILE", buffer, MAX_PATH);
            userDirectory = StringConvertToMultibyte(WideStringView(buffer, length));
        }

        return userDirectory;
    }

    StringView Path::GetDocumentsDirectory()
    {
        static String documentsDirectory = Combine(GetUserDirectory(), "Documents");
        return documentsDirectory;
    }

    StringView Path::GetMusicDirectory()
    {
        static String musicDirectory = Combine(GetUserDirectory(), "Music");
        return musicDirectory;
    }

    StringView Path::GetDownloadsDirectory()
    {
        static String downloadsDirectory = Combine(GetUserDirectory(), "Downloads");
        return downloadsDirectory;
    }

    StringView Path::GetDesktopDirectory()
    {
        static String desktopDirectory = Combine(GetUserDirectory(), "Desktop");
        return desktopDirectory;
    }
}
