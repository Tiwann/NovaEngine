#include "FileStream.h"
#include <cstdio>

namespace Nova
{
    static StringView GetMode(OpenModeFlags OpenMode)
    {
        switch (OpenMode) {
        case OpenModeFlagBits::Read: return "r";
        case OpenModeFlagBits::Write: return "w";
        case OpenModeFlagBits::ReadText: return "r";
        case OpenModeFlagBits::ReadBinary: return "rb";
        case OpenModeFlagBits::WriteText: return "w";
        case OpenModeFlagBits::WriteBinary: return "wb";
        case OpenModeFlagBits::ReadWriteText: return "w+";
        case OpenModeFlagBits::ReadWriteBinary: return "wb+";
        default: return "";
        }
    }
    
    FileStream::FileStream(Path Filepath, OpenModeFlags OpenMode): Stream(OpenMode), m_Filepath(std::move(Filepath))
    {
        m_Handle = fopen(m_Filepath.string().c_str(), GetMode(OpenMode));
        m_Opened = m_Handle;
    }

    bool FileStream::Open(const Path& Filepath, OpenModeFlags OpenMode)
    {
        if(m_Opened) Close();
        m_Filepath = Filepath;
        m_OpenMode = OpenMode;

        m_Handle = fopen(m_Filepath.string().c_str(), GetMode(OpenMode));
        return m_Opened = m_Handle;
    }

    Stream::SizeType FileStream::Read(void* OutBuffer, SizeType Count)
    {
        return fread(OutBuffer, 1, Count, m_Handle);
    }

    Stream::SizeType FileStream::Write(const void* InBuffer, SizeType Count)
    {
        return fwrite(InBuffer, 1, Count, m_Handle);
    }

    bool FileStream::Seek(Nova::Seek SeekMode, OffsetType Offset)
    {
        const auto GetSeekMode = [](Nova::Seek SeekMode) -> int
        {
            return SeekMode == Seek::Begin ? SEEK_SET : SeekMode == Seek::Current ? SEEK_CUR : SEEK_END;
        };
            
        const int Result = fseek(m_Handle, Offset, GetSeekMode(SeekMode));
        return Result == 0;
    }

    Stream::OffsetType FileStream::Tell() const
    {
        return ftell(m_Handle);
    }

    void FileStream::Close()
    {
        Stream::Close();
        (void)fclose(m_Handle);
        m_Handle = nullptr;
    }

    Stream::OffsetType FileStream::GetSize()
    {
        Seek(Seek::End, 0);
        const OffsetType Result = Tell();
        Seek(Seek::Begin, 0);
        return Result;
    }

    bool FileStream::IsGood() const
    {
        return !feof(m_Handle);
    }


    StandardStream StandardStream::StandardIn = StandardStream(stdin, OpenModeFlagBits::ReadText);
    StandardStream StandardStream::StandardOut = StandardStream(stdout, OpenModeFlagBits::WriteText);
    StandardStream StandardStream::StandardError = StandardStream(stderr, OpenModeFlagBits::WriteText);
    
    StandardStream::StandardStream(FILE* Handle, const OpenModeFlags& OpenMode): Stream(OpenMode), m_Handle(Handle)
    {
        m_Opened = m_Handle;
    }

    Stream::SizeType StandardStream::Read(void* OutBuffer, SizeType Count)
    {
        return -1;
    }

    Stream::SizeType StandardStream::Write(const void* InBuffer, SizeType Count)
    {
        return fprintf(m_Handle, "%s", (const char*)InBuffer);
    }

    bool StandardStream::Seek(Nova::Seek SeekMode, OffsetType Offset)
    {
        return false;
    }

    Stream::OffsetType StandardStream::Tell() const
    {
        return -1;
    }

    void StandardStream::Close()
    {
        Stream::Close();
        (void)fclose(m_Handle);
        m_Handle = nullptr;
    }

    bool StandardStream::IsGood() const
    {
        return !feof(m_Handle);
    }
}
