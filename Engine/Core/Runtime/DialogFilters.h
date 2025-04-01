#pragma once
#include "Containers/String.h"
#include "Containers/Array.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_API DialogFilters
    {
    public:
        struct Filter
        {
            String Name;
            Array<String> Extensions;
            bool operator==(const Filter& Other) const;
        };
        static Filter All;

    public:
        DialogFilters();
        DialogFilters(const Array<Filter>& Filters);
        ~DialogFilters();

        void AddFilter(const String& Name, const Array<String>& Extensions);
        void AddFilter(const Filter& Filter);
        void RemoveFilter(const String& Name, const Array<String>& Extensions);
        void RemoveFilter(const Filter& Filter);
        Filter* GetFilter(const String& Name) const;
        
        ScopedBuffer<char> GetFilterString() const;
        
        static DialogFilters SoundFilters;
        static DialogFilters ModelFilters;
        static DialogFilters ImageFilters;

    private:
        Array<Filter> m_Filters;
    };
}
