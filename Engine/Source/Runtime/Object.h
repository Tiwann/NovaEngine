#pragma once
#include "Containers/String.h"
#include "RefCounted.h"

namespace Nova
{
    class Object : public RefCounted
    {
    public:
        Object();
        explicit Object(String name);
        ~Object() override = default;

        const String& GetObjectName() const;
        void SetObjectName(const String& newName);
    protected:
        String m_Name;
    };
}
