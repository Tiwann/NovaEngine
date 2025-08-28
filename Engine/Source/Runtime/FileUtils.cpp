#include "FileUtils.h"
#include "Serialization/FileStream.h"

namespace Nova::FileUtils
{
    Array<uint8_t> ReadToBuffer(const StringView filepath)
    {
        FileStream stream(filepath, OpenModeFlagBits::ReadBinary);
        if (!stream.IsOpened()) return {};

        const size_t fileSize = stream.GetSize();
        Array<uint8_t> buffer(fileSize);
        const size_t read = stream.Read(buffer.Data(), fileSize);
        if (read != fileSize) return {};

        return std::move(buffer);
    }

    String ReadToString(const StringView filepath)
    {
        FileStream stream(filepath, OpenModeFlagBits::ReadBinary);
        if (!stream.IsOpened()) return "";

        stream.Seek(Seek::End, 0);
        const FileStream::OffsetType fileSize = stream.Tell();
        stream.Seek(Seek::Begin, 0);

        String result;
        const FileStream::SizeType read = stream.ReadString(result, fileSize);
        if (read != (FileStream::SizeType)fileSize) return "";
        return result;
    }
}
