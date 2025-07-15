#pragma once
#include "Containers/String.h"
#include "Export.h"

namespace Nova
{
    class Object
    {
    public:
        Object();
        explicit Object(String Name);
        virtual ~Object() = default;

        const String& GetObjectName() const;
        void SetObjectName(const String& NewName);

        template <typename Derived> requires IsBaseOfValue<Object, Derived>
        Derived* As() { return dynamic_cast<Derived*>(this); }

        template <typename Derived> requires IsBaseOfValue<Object, Derived>
        const Derived* As() const { return dynamic_cast<const Derived*>(this); }

    protected:
        String m_Name;
    };
}
