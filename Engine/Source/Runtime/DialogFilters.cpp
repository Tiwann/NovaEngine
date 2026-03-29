#include "DialogFilters.h"
#include "Containers/StringFormat.h"

namespace Nova
{
    DialogFilters::DialogFilters() = default;

    DialogFilters::DialogFilters(const String& name, const Array<String>& extensions)
    {
       AddFilter(name, extensions);
    }

    DialogFilters::DialogFilters(const Filter& filter)
    {
        m_Filters.Add(filter);
    }

    DialogFilters::~DialogFilters()
    {

    }

    void DialogFilters::RemoveFilter(const String& name, const Array<String>& extensions)
    {
        m_Filters.Remove({name, extensions});
    }

    void DialogFilters::RemoveFilter(const Filter& filter)
    {
        m_Filters.Remove(filter);
    }

    DialogFilters::Filter* DialogFilters::GetFilter(const String& name) const
    {
        Filter* Result = m_Filters.Single([&name](const Filter& filter) { return filter.name == name; });
        return Result;
    }

    void DialogFilters::AddFilter(const String& name, const Array<String>& extensions)
    {
        m_Filters.Add({ name, extensions });
    }

    void DialogFilters::AddFilter(const Filter& filter)
    {
        m_Filters.Add(filter);
    }

    const DialogFilters::Filter DialogFilters::All = {"All Files", {"*"}};
    const DialogFilters::Filter DialogFilters::WAV = {"Waveform Audio", {"wav"}};
    const DialogFilters::Filter DialogFilters::OGG = {"Vorbis", {"ogg"}};
    const DialogFilters::Filter DialogFilters::MP3 = {"MPEG Audio Layer III", {"mp3"}};
    const DialogFilters::Filter DialogFilters::FLAC = {"Free Lossless Audio Codec", {"flac"}};
    const DialogFilters::Filter DialogFilters::PNG = {"Portable Network Graphics", {"png"}};
    const DialogFilters::Filter DialogFilters::WEBP = {"Google Webp", {"webp"}};
    const DialogFilters::Filter DialogFilters::JPEG = {"Joint Photographic Experts Group", {"jpg", "jpeg"}};
    const DialogFilters::Filter DialogFilters::TARGA = {"Truevision Targa", {"tga"}};
    const DialogFilters::Filter DialogFilters::DDS = {"DirectDraw Surface", {"dds"}};
    const DialogFilters::Filter DialogFilters::ASTC = {"Adaptive Scalable Texture", {"astc"}};
    const DialogFilters::Filter DialogFilters::BMP = {"Windows Bitmap", {"bmp"}};
    const DialogFilters::Filter DialogFilters::FBX = {"Autodesk Filmbox", {"fbx"}};
    const DialogFilters::Filter DialogFilters::GLTF = {"GL Transmission Format", {"gltf", "glb"}};
    const DialogFilters::Filter DialogFilters::OBJ = {"Wavefront OBJ", {"obj"}};
    const DialogFilters::Filter DialogFilters::DAE = {"Collada", {"dae"}};
    const DialogFilters::Filter DialogFilters::TTF = {"TrueType Font", {"ttf", "dfont"}};
    const DialogFilters::Filter DialogFilters::OTF = {"OpenType Font", {"otf", "otc", "ttf", "ttc"}};

    bool DialogFilters::Filter::operator==(const Filter& other) const
    {
        return name == other.name && extensions == other.extensions;
    }

    DialogFilters::DialogFilters(const Array<Filter>& Filters)
    {
        m_Filters = Filters;
    }

    const DialogFilters DialogFilters::ImageFilters = Array {
        {"All Image Formats", {"png", "jpg", "jpeg", "bmp", "tga", "dds", "astc"}},
        PNG, JPEG, BMP, TARGA, DDS, ASTC, All
    };

    const DialogFilters DialogFilters::AudioFilters = Array {
        Filter{ .name = "All Audio Formats", .extensions = { "wav", "mp3", "ogg", "flac" }},
        WAV, MP3, OGG, FLAC, All
    };

    const DialogFilters DialogFilters::ModelFilters = Array {
        Filter{ .name = "All Model Formats", .extensions = { "fbx", "gltf", "glb", "obj", "dae" }},
        FBX, GLTF, OBJ, DAE, All
    };

    const DialogFilters DialogFilters::FontFilters = Array {
        Filter{ .name = "All Font Formats", .extensions = { "ttf", "ttc", "otf", "otc", "dfont" }},
        TTF, OTF
    };

    String DialogFilters::GetFilterString() const
    {
        Array<String> displayNames;
        Array<String> extensions;

        for (const Filter& filter : m_Filters)
        {
            String extensionsComma;
            String extensionsSemiColon;
            for (size_t i = 0; i < filter.extensions.Count(); i++)
            {
                if (i != filter.extensions.Count() - 1)
                {
                    extensionsComma.Append(StringFormat(".{}, ", filter.extensions[i]));
                    extensionsSemiColon.Append(StringFormat("*.{};", filter.extensions[i]));
                } else
                {
                    extensionsComma.Append(StringFormat(".{}", filter.extensions[i]));
                    extensionsSemiColon.Append(StringFormat("*.{}", filter.extensions[i]));
                }
            }
            displayNames.Add(StringFormat("{} ({})", filter.name, extensionsComma));
            extensions.Add(extensionsSemiColon);
        }

        size_t totalCount = 1;
        for (size_t i = 0; i < m_Filters.Count(); i++)
        {
            totalCount += displayNames[i].Count() + 1;
            totalCount += extensions[i].Count() + 1;
        }

        String result(totalCount);
        
        size_t currentCount = 0;
        for (size_t i = 0; i < m_Filters.Count(); i++)
        {
            const String& currentDisplayName = displayNames[i];
            const String& currentExtension = extensions[i];
            memcpy(*result + currentCount, *currentDisplayName, currentDisplayName.Count());
            memcpy(*result + currentCount + currentDisplayName.Count() + 1 , *currentExtension, currentExtension.Count());
            currentCount += currentDisplayName.Count() + currentExtension.Count() + 1 + 1;
        }
        return result;
    }
}
