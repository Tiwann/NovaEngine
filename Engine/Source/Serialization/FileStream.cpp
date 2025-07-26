#include "FileStream.h"
#include <cstdio>

namespace Nova
{
    static StringView GetMode(const OpenModeFlags openMode)
    {
        switch (openMode) {
        case (uint32_t)OpenModeFlagBits::Read: return "r";
        case (uint32_t)OpenModeFlagBits::Write: return "w";
        case (uint32_t)OpenModeFlagBits::ReadText: return "r";
        case (uint32_t)OpenModeFlagBits::ReadBinary: return "rb";
        case (uint32_t)OpenModeFlagBits::WriteText: return "w";
        case (uint32_t)OpenModeFlagBits::WriteBinary: return "wb";
        case (uint32_t)OpenModeFlagBits::ReadWriteText: return "w+";
        case (uint32_t)OpenModeFlagBits::ReadWriteBinary: return "wb+";
        default: return "";
        }
    }
    
    FileStream::FileStream(StringView filepath, const OpenModeFlags openMode): Stream(openMode), m_Filepath(std::move(filepath))
    {
        m_Handle = fopen(*m_Filepath, GetMode(openMode));
        m_Opened = m_Handle;
    }

    bool FileStream::Open(const StringView& filepath, const OpenModeFlags openMode)
    {
        if(m_Opened) Close();
        m_Filepath = filepath;
        m_OpenMode = openMode;

        m_Handle = fopen(*m_Filepath, GetMode(openMode));
        return m_Opened = m_Handle;
    }

    Stream::SizeType FileStream::Read(void* outBuffer, const SizeType size)
    {
        return fread(outBuffer, 1, size, m_Handle);
    }

    Stream::SizeType FileStream::Write(const void* inBuffer, const SizeType count)
    {
        return fwrite(inBuffer, 1, count, m_Handle);
    }

    bool FileStream::Seek(const Nova::Seek seekMode, const OffsetType Offset)
    {
        const auto GetSeekMode = [](const Nova::Seek SeekMode) -> int
        {
            return SeekMode == Seek::Begin ? SEEK_SET : SeekMode == Seek::Current ? SEEK_CUR : SEEK_END;
        };
            
        const int Result = fseek(m_Handle, Offset, GetSeekMode(seekMode));
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


    StandardStream StandardStream::StandardIn = StandardStream(stdin, OpenModeFlagBits::Read | OpenModeFlagBits::Text);
    StandardStream StandardStream::StandardOut = StandardStream(stdout, OpenModeFlagBits::Write | OpenModeFlagBits::Text);
    StandardStream StandardStream::StandardError = StandardStream(stderr, OpenModeFlagBits::Write | OpenModeFlagBits::Text);
    
    StandardStream::StandardStream(FILE* handle, const OpenModeFlags& openMode): Stream(openMode), m_Handle(handle)
    {
        m_Opened = m_Handle;
    }

    Stream::SizeType StandardStream::Read(void* outBuffer, SizeType size)
    {
        return -1;
    }

    Stream::SizeType StandardStream::Write(const void* inBuffer, SizeType count)
    {
        return fprintf(m_Handle, "%s", (const char*)inBuffer);
    }

    bool StandardStream::Seek(Nova::Seek seekMode, OffsetType offset)
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
