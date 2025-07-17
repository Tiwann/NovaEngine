#include "FileUtils.h"
#include "Serialization/FileStream.h"

namespace Nova::File
{
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
