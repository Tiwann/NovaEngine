#include "DialogFilters.h"
#include "Containers/StringFormat.h"

namespace Nova
{
    DialogFilters::DialogFilters() = default;
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

    DialogFilters::Filter DialogFilters::All = {"All Files", {"*"}};

    bool DialogFilters::Filter::operator==(const Filter& other) const
    {
        return name == other.name && extensions == other.extensions;
    }

    DialogFilters::DialogFilters(const Array<Filter>& Filters)
    {
        m_Filters = Filters;
    }

    DialogFilters DialogFilters::ImageFilters = Array {
        Filter{ .name = "Portable Network Graphics", .extensions = { "png" }},
        Filter{ .name = "Joint Photographic Experts Group", .extensions = { "jpg", "jpeg" }},
        Filter{ .name = "Windows Bitmap", .extensions = { "bmp" }},
        Filter{ .name = "Truevision Targa", .extensions = { "tga" }},
        All
    };

    DialogFilters DialogFilters::SoundFilters = Array {
        Filter{ .name = "All Audio Formats", .extensions = { "wav", "mp3", "ogg", "flac" }},
        Filter{ .name = "Waveform Audio", .extensions = { "wav" }},
        Filter{ .name = "MPEG Audio Layer III", .extensions = { "mp3" }},
        Filter{ .name = "Vorbis", .extensions = { "ogg" }},
        Filter{ .name = "Free Lossless Audio Codec", .extensions = { "flac" }},
        All
    };

    DialogFilters DialogFilters::ModelFilters = Array {
        Filter{ .name = "All Model Formats", .extensions = { "fbx", "gltf", "glb", "obj", "dae" }},
        Filter{ .name = "Autodesk Filmbox", .extensions = { "fbx" }},
        Filter{ .name = "GL Transmission Format", .extensions = { "gltf", "glb" }},
        Filter{ .name = "Wavefront", .extensions = { "obj" }},
        Filter{ .name = "Collada", .extensions = { "dae" }},
        All
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
            memcpy(result.Data() + currentCount, *currentDisplayName, currentDisplayName.Count());
            memcpy(result.Data() + currentCount + currentDisplayName.Count() + 1 , *currentExtension, currentExtension.Count());
            currentCount += currentDisplayName.Count() + currentExtension.Count() + 1 + 1;
        }
        return result;
    }
}
