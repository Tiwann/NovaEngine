#pragma once
#include "Containers/String.h"
#include "RefObject.h"

namespace Nova
{
    class Object : public RefObject
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
