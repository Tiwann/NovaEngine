#pragma once
#include "Runtime/Object.h"

namespace Nova
{
    class Renderer;

    template <typename CreateInfoType>
    class RendererObject : public Object
    {
    public:
        explicit RendererObject(const String& Name, Renderer* Owner) : Object(Name), m_Owner(Owner){}

        virtual bool Initialize(const CreateInfoType& CreateInfo) = 0;
        virtual void SetDebugName(const String& Name){}
        virtual void Destroy() = 0;

        Renderer* GetOwner() const { return m_Owner; }
    protected:
        Renderer* m_Owner = nullptr;
    };
}
