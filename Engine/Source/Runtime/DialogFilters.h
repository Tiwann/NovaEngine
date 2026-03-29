#pragma once
#include "Containers/String.h"
#include "Containers/Array.h"
#include "Containers/StringView.h"

namespace Nova
{
    class DialogFilters
    {
    public:
        struct Filter
        {
            String name;
            Array<String> extensions;
            bool operator==(const Filter& other) const;
        };
        static const Filter All;
        static const Filter WAV;
        static const Filter OGG;
        static const Filter MP3;
        static const Filter FLAC;
        static const Filter PNG;
        static const Filter WEBP;
        static const Filter JPEG;
        static const Filter TARGA;
        static const Filter DDS;
        static const Filter ASTC;
        static const Filter BMP;
        static const Filter FBX;
        static const Filter GLTF;
        static const Filter GLB;
        static const Filter OBJ;
        static const Filter DAE;
        static const Filter TTF;
        static const Filter OTF;
    public:
        DialogFilters();
        DialogFilters(const String& name, const Array<String>& extensions);
        DialogFilters(const Filter& filter);
        DialogFilters(const Array<Filter>& filters);
        ~DialogFilters();

        void AddFilter(const String& name, const Array<String>& extensions);
        void AddFilter(const Filter& filter);

        template<typename... Ext>
        void AddFilter(const String& name, Ext&&... extensions)
        {
            AddFilter(name, { std::forward<Ext>(extensions)... });
        }

        void RemoveFilter(const String& name, const Array<String>& extensions);
        void RemoveFilter(const Filter& filter);
        Filter* GetFilter(const String& name) const;
        
        String GetFilterString() const;
        
        static const DialogFilters AudioFilters;
        static const DialogFilters ModelFilters;
        static const DialogFilters ImageFilters;
        static const DialogFilters FontFilters;
    private:
        Array<Filter> m_Filters;
    };
}
