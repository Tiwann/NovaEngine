#pragma once
#include "Containers/String.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_API Object
    {
    public:
        Object();
        Object(String Name);
        virtual ~Object() = default;

        const String& GetName() const;
        void SetName(const String& NewName);
    protected:
        String m_Name;
    };
}
