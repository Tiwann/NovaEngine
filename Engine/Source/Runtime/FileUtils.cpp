#include "FileUtils.h"
#include "IO/FileStream.h"

namespace Nova::FileUtils
{
    Array<uint8_t> ReadToBuffer(const StringView filepath)
    {

        FileStream stream(filepath, OpenModeFlagBits::ReadBinary);
        if (!stream.IsOpened()) return {};

        const size_t fileSize = stream.GetSize();
        Array<uint8_t> buffer(fileSize);
        const size_t read = stream.ReadRaw(buffer.Data(), fileSize);
        if (read != fileSize) return {};

        return buffer;
    }

    String ReadToString(const StringView filepath)
    {
        FileStream stream(filepath, OpenModeFlagBits::ReadBinary);
        if (!stream.IsOpened()) return {};

        const size_t fileSize = stream.GetSize();
        String result(fileSize);
        const size_t read = stream.Read(result, fileSize);

        if (read != fileSize) return {};
        return result;
    }

    Array<uint8_t> ReadTillEnd(Stream& stream)
    {
        const size_t position = stream.Tell();

        stream.Seek(Seek::End, 0);
        const size_t endPosition = stream.Tell();
        stream.Seek(Seek::Begin, position);

        const size_t bytesToRead = endPosition - position;
        Array<uint8_t> result(bytesToRead);
        const size_t read = stream.ReadRaw(result.Data(), result.Size());
        if (read != bytesToRead) return {};
        return result;
    }
}
