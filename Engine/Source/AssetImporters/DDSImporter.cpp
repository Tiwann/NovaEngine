#include "DDSImporter.h"
#include "../IO/FileStream.h"
#include "Runtime/FileUtils.h"
#include "Runtime/Path.h"

namespace Nova
{
    bool DDSImporter::LoadFromFile(const StringView filepath)
    {
        if (!FilepathIsValid(filepath)) return false;

        FileStream stream(filepath, OpenModeFlagBits::ReadBinary);
        return LoadFromStream(stream);
    }

    bool DDSImporter::LoadFromStream(Stream& stream)
    {
        if (!stream.Rewind()) return false;

        uint32_t ddsMagic = 0;
        stream.Read(ddsMagic);
        if (ddsMagic != 0x20534444)
        {
            char* magic = (char*)&ddsMagic;
            std::println(std::cout, "[DDS LOADER] Wrong magic id \"{}\". Aborting.", StringView(magic, 4));
            return false;
        }

        DDSHeader header;
        if (stream.ReadObject(header) != sizeof(DDSHeader))
            return false;


        if (header.pixelFormat.flags & DDSPixelFormatFlagBits::FourCC)
        {
            const auto& fourCC = header.pixelFormat.fourCC;
            if (Memory::Memcmp(fourCC, "DX10", 4))
            {
                DDSHeaderDXT10 headerDXT10;
                stream.ReadObject(headerDXT10);
            }
        }


        return true;
    }

    Array<String> DDSImporter::SupportedExtensions() const
    {
        return { "dds" };
    }

    Ref<TextureImporter::Texture> DDSImporter::CreateTexture() const
    {
        return nullptr;
    }
}
