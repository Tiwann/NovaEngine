#include "DialogFilters.h"
#include "Containers/StringFormat.h"

namespace Nova
{
    DialogFilters::DialogFilters() = default;
    DialogFilters::~DialogFilters() = default;

    void DialogFilters::RemoveFilter(const String& Name, const Array<String>& Extensions)
    {
        m_Filters.Remove({ .Name = Name, .Extensions = Extensions});
    }

    void DialogFilters::RemoveFilter(const Filter& Filter)
    {
        m_Filters.Remove(Filter);
    }

    DialogFilters::Filter* DialogFilters::GetFilter(const String& Name) const
    {
        Filter* Result = m_Filters.Single([&Name](const Filter& Filter) { return Filter.Name == Name; });
        return Result;
    }

    void DialogFilters::AddFilter(const String& Name, const Array<String>& Extensions)
    {
        m_Filters.Add({ Name, Extensions });
    }

    void DialogFilters::AddFilter(const Filter& Filter)
    {
        m_Filters.Add(Filter);
    }

    DialogFilters::Filter DialogFilters::All = { .Name= "All Files", .Extensions= {"*"}};

    bool DialogFilters::Filter::operator==(const Filter& Other) const
    {
        return Name == Other.Name && Extensions == Other.Extensions;
    }

    DialogFilters::DialogFilters(const Array<Filter>& Filters): m_Filters(Filters)
    {
        
    }

    DialogFilters DialogFilters::ImageFilters = Array<DialogFilters::Filter> {
        DialogFilters::Filter{ .Name = "Portable Network Graphics", .Extensions = { "png" }},
        DialogFilters::Filter{ .Name = "Joint Photographic Experts Group", .Extensions = { "jpg", "jpeg" }},
        DialogFilters::Filter{ .Name = "Windows Bitmap", .Extensions = { "bmp" }},
        DialogFilters::Filter{ .Name = "Truevision Targa", .Extensions = { "tga" }},
        DialogFilters::All
    };

    DialogFilters DialogFilters::SoundFilters = Array<DialogFilters::Filter> {
        DialogFilters::Filter{ .Name = "All Audio Formats", .Extensions = { "wav", "mp3", "ogg", "flac" }},
        DialogFilters::Filter{ .Name = "Waveform Audio", .Extensions = { "wav" }},
        DialogFilters::Filter{ .Name = "MPEG Audio Layer III", .Extensions = { "mp3" }},
        DialogFilters::Filter{ .Name = "Vorbis", .Extensions = { "ogg" }},
        DialogFilters::Filter{ .Name = "Free Lossless Audio Codec", .Extensions = { "flac" }},
        DialogFilters::All
    };
    
    DialogFilters DialogFilters::ModelFilters = Array<DialogFilters::Filter> {
        DialogFilters::Filter{ .Name = "All Model Formats", .Extensions = { "fbx", "gltf", "glb", "obj", "dae" }},
        DialogFilters::Filter{ .Name = "Autodesk Filmbox", .Extensions = { "fbx" }},
        DialogFilters::Filter{ .Name = "GL Transmission Format", .Extensions = { "gltf", "glb" }},
        DialogFilters::Filter{ .Name = "Wavefront", .Extensions = { "obj" }},
        DialogFilters::Filter{ .Name = "Collada", .Extensions = { "dae" }},
        DialogFilters::All
    };

    ScopedBuffer<char> DialogFilters::GetFilterString() const
    {
        Array<String> DisplayNames;
        Array<String> Extensions;
            
        for (const Filter& Filter : m_Filters)
        {
            String ExtensionsComma;
            String ExtensionsSemiColon;
            for (size_t i = 0; i < Filter.Extensions.Count(); i++)
            {
                if (i != Filter.Extensions.Count() - 1)
                {
                    ExtensionsComma.Append(Format(".{}, ", Filter.Extensions[i]));
                    ExtensionsSemiColon.Append(Format("*.{};", Filter.Extensions[i]));
                } else
                {
                    ExtensionsComma.Append(Format(".{}", Filter.Extensions[i]));
                    ExtensionsSemiColon.Append(Format("*.{}", Filter.Extensions[i]));
                }
            }
            DisplayNames.Add(Format("{} ({})", Filter.Name, ExtensionsComma));
            Extensions.Add(ExtensionsSemiColon);
        }

        size_t TotalCount = 1;
        for (size_t i = 0; i < m_Filters.Count(); i++)
        {
            TotalCount += DisplayNames[i].Count() + 1;
            TotalCount += Extensions[i].Count() + 1;
        }

        Buffer<char> Result(TotalCount);
        
        size_t CurrentCount = 0;
        for (size_t i = 0; i < m_Filters.Count(); i++)
        {
            const String& CurrentDisplayName = DisplayNames[i];
            const String& CurrentExtensions = Extensions[i];
            memcpy(Result.Data() + CurrentCount, *CurrentDisplayName, CurrentDisplayName.Count());
            memcpy(Result.Data() + CurrentCount + CurrentDisplayName.Count() + 1 , *CurrentExtensions, CurrentExtensions.Count());
            CurrentCount += CurrentDisplayName.Count() + CurrentExtensions.Count() + 1 + 1;
        }
        return Result;
    }
}
