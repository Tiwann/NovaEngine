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
