#pragma once
#include "Containers/String.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_API Object
    {
    public:
        Object();
        explicit Object(String Name);
        virtual ~Object() = default;

        const String& GetObjectName() const;
        void SetObjectName(const String& NewName);

        template <typename Derived> requires IsBaseOfValue<Object, Derived>
        Derived* As() { return dynamic_cast<Derived*>(this); }

    protected:
        String m_Name;
    };
}
