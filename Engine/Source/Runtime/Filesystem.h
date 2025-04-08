#pragma once
#include "Containers/Buffer.h"
#include "LogCategory.h"
#include "Types.h"
#include "Containers/String.h"
#include "DialogFilters.h"
#include <filesystem>
#include <CoreExport.h>


namespace Nova
{
    using Path = std::filesystem::path;
    NOVA_DECLARE_LOG_CATEGORY_STATIC(Filesystem, "FILESYSTEM")
    
    class CORE_API File
    {
    public:
        File() = delete;
        
        static bool             Exists(const Path& Filepath);
        static bool             Create(const Path& Filepath);
        static Buffer<u8>       ReadToBuffer(const Path& Filepath);
        static String           ReadToString(const Path& Filepath);
        static Path             OpenFileDialog(const String& Title, const Path& DefaultPath, const DialogFilters& Filters);
        static Path             SaveFileDialog(const String& Title, const Path& DefaultPath, const DialogFilters& Filters);
        static String           BytesToString(u64 Bytes);
        static Path             GetDirectory(const Path& Filepath);
        static Path             GetAbsolutePath(const Path& Relative, const Path& Filepath);
    };
    
    class CORE_API Directory
    {
    public:
        Directory() = delete;
        
        static bool         Exists(const Path& Directory);
        static bool         Create(const Path& Directory);
        static Path         OpenFolderDialog(const String& Title, const Path& DefaultPath);
        static Array<Path>  GetFiles(const Path& Directory);
        static Path         GetUserDirectory();
        static Path         GetDownloadsDirectory();
        static Path         GetDocumentsDirectory();
        static Path         GetDesktopDirectory();
        static Path         GetMusicDirectory();
        static Path         GetImagesDirectory();
        static Path         GetVideosDirectory();
        static Path         GetCurrentWorkingDirectory();
    };
}
