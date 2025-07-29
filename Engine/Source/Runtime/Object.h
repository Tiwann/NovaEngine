#pragma once
#include "Containers/String.h"

namespace Nova
{
    class Object
    {
    public:
        Object();
        explicit Object(String name);
        virtual ~Object() = default;

        const String& GetObjectName() const;
        void SetObjectName(const String& newName);

        template <typename Derived> requires std::is_base_of_v<Object, Derived>
        Derived* As() { return dynamic_cast<Derived*>(this); }

        template <typename Derived> requires std::is_base_of_v<Object, Derived>
        const Derived* As() const { return dynamic_cast<const Derived*>(this); }

    protected:
        String m_Name;
    };
}
