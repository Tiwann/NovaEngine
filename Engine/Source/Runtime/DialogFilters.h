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
        static Filter All;
        static Filter WAV;
        static Filter OGG;
        static Filter MP3;
        static Filter FLAC;
        static Filter PNG;
        static Filter WEBP;
        static Filter JPEG;
        static Filter TARGA;
        static Filter DDS;
        static Filter ASTC;
        static Filter BMP;
        static Filter FBX;
        static Filter GLTF;
        static Filter GLB;
        static Filter OBJ;
        static Filter DAE;

    public:
        DialogFilters();
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
        
        static DialogFilters SoundFilters;
        static DialogFilters ModelFilters;
        static DialogFilters ImageFilters;

    private:
        Array<Filter> m_Filters;
    };
}
