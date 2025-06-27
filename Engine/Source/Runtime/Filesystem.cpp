#include "Filesystem.h"
#include "Application.h"
#include "Log.h"
#include "LogVerbosity.h"
#include "Containers/Array.h"
#include "Serialization/FileStream.h"


namespace Nova
{
    bool File::Exists(const Path& Filepath)
    {
        return exists(Filepath);
    }

    bool File::Create(const Path& Filepath)
    {
        NOVA_LOG(Filesystem, Verbosity::Warning, "Creating file: {}", Filepath.string());
        FileStream File(Filepath, OpenModeFlagBits::Write);
        if(!File.IsOpened()) return false;

        File.Seek(Seek::End, 0);
        if(const size_t FileSize = File.Tell(); FileSize > 0) return false;
        
        File.Close();
        return true;
    }

    Buffer<u8> File::ReadToBuffer(const Path& Filepath)
    {
        FileStream Stream(Filepath, OpenModeFlagBits::ReadBinary);
        if (!Stream.IsOpened())
        {
            NOVA_LOG(Filesystem, Verbosity::Error, "Failed to read file {} into buffer: File doesn't exist!", Filepath.string());
            return {};
        }

        const FileStream::OffsetType FileSize = Stream.GetSize();
        Buffer<u8> OutBuffer;
        Stream.ReadBuffer(OutBuffer, FileSize);
        Stream.Close();
        return OutBuffer;
    }
    
    String File::ReadToString(const Path& Filepath)
    {
        FileStream Stream(Filepath, OpenModeFlagBits::ReadBinary);
        if (!Stream.IsOpened()) return "";

        Stream.Seek(Seek::End, 0);
        const FileStream::OffsetType FileSize = Stream.Tell();
        Stream.Seek(Seek::Begin, 0);

        String Result;
        const FileStream::SizeType Read = Stream.ReadString(Result, FileSize);
        if (Read != (FileStream::SizeType)FileSize) return "";
        return Result;
    }
    
    
    String File::BytesToString(u64 Bytes)
    {
        constexpr u64 GB = 1024ULL * 1024ULL * 1024ULL;
        constexpr u64 MB = 1024ULL * 1024ULL;
        constexpr u64 KB = 1024ULL;

        char buffer[32 + 1] {};

        if (Bytes > GB)
            (void)snprintf(buffer, 32, "%.2f GB", (f32)Bytes / (f32)GB);
        else if (Bytes > MB)
            (void)snprintf(buffer, 32, "%.2f MB", (f32)Bytes / (f32)MB);
        else if (Bytes > KB)
            (void)snprintf(buffer, 32, "%.2f KB", (f32)Bytes / (f32)KB);
        else
            (void)snprintf(buffer, 32, "%.2f Bytes", (f32)Bytes);

        return buffer;
    }

    Path File::GetDirectory(const Path& Filepath)
    {
        return Filepath.parent_path();
    }

    Path File::GetAbsolutePath(const Path& Relative, const Path& Filepath)
    {
        return weakly_canonical(weakly_canonical(Relative) / Filepath);
    }

    bool Directory::Exists(const Path& Directory)
    {
        if(!std::filesystem::is_directory(Directory))
            return false;
        return std::filesystem::exists(Directory);
    }

    bool Directory::Create(const Path& Directory)
    {
        return std::filesystem::create_directory(Directory);
    }
    
    Array<Path> Directory::GetFiles(const Path& Directory)
    {
        if(!is_directory(Directory) || !Exists(Directory))
            return {};

        Array<Path> Result;
        for(const auto& Filepath : std::filesystem::directory_iterator(Directory))
        {
            if(is_regular_file(Filepath))
            {
                Result.Add(Filepath);
            }
        }
        return Result;
    }
    
    Path Directory::GetDownloadsDirectory()
    {
        return GetUserDirectory() / "Downloads";
    }

    Path Directory::GetDocumentsDirectory()
    {
        return GetUserDirectory() / "Documents";
    }

    Path Directory::GetDesktopDirectory()
    {
        return GetUserDirectory() / "Desktop";
    }

    Path Directory::GetMusicDirectory()
    {
        return GetUserDirectory() / "Music";
    }

    Path Directory::GetImagesDirectory()
    {
        return GetUserDirectory() / "Images";
    }

    Path Directory::GetVideosDirectory()
    {
        return GetUserDirectory() / "Videos";
    }

}


