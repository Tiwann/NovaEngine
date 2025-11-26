#include "FileStream.h"
#include <cstdio>

#include "Containers/StringConversion.h"

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
        Open(filepath, openMode);
    }

    bool FileStream::Open(const StringView& filepath, const OpenModeFlags openMode)
    {
#ifdef NOVA_PLATFORM_WINDOWS
        WideString wFilepath = StringConvertToWide(filepath);
        m_Handle = _wfopen(*wFilepath, *StringConvertToWide(GetMode(openMode)));
        m_Opened = m_Handle;
#else
        m_Handle = fopen(*m_Filepath, *GetMode(openMode));
        m_Opened = m_Handle;
#endif
        return m_Opened = m_Handle;
    }

    Stream::SizeType FileStream::ReadRaw(void* outBuffer, const SizeType size)
    {
        return fread(outBuffer, 1, size, m_Handle);
    }

    Stream::SizeType FileStream::WriteRaw(const void* inBuffer, const SizeType size)
    {
        return fwrite(inBuffer, 1, size, m_Handle);
    }

    bool FileStream::Seek(const Nova::Seek seekMode, const OffsetType offset)
    {
        const auto GetSeekMode = [](const Nova::Seek SeekMode) -> int
        {
            return SeekMode == Seek::Begin ? SEEK_SET : SeekMode == Seek::Current ? SEEK_CUR : SEEK_END;
        };
            
        const int Result = fseek(m_Handle, offset, GetSeekMode(seekMode));
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
        const OffsetType result = Tell();
        Seek(Seek::Begin, 0);
        return result;
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

    Stream::SizeType StandardStream::ReadRaw(void* outBuffer, SizeType size)
    {
        return -1;
    }

    Stream::SizeType StandardStream::WriteRaw(const void* inBuffer, SizeType size)
    {
        return fprintf(m_Handle, "%*s", int(size), (const char*)inBuffer);
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
