#pragma once
#include "Containers/String.h"
#include "RefCounted.h"
#include "RTTI.h"

namespace Nova
{
    class Object : public RefCounted
    {
        NOVA_DECLARE_CLASS(Object);
    public:
        Object();
        explicit Object(String name);
        ~Object() override = default;

        virtual void Destroy() {}
        const String& GetObjectName() const;
        void SetObjectName(const String& newName);
    protected:
        String m_Name;
    };
}
