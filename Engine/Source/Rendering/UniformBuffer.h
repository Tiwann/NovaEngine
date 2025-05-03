#pragma once
#include "Runtime/Object.h"

namespace Nova
{
    class Renderer;

    class UniformBuffer : public Object
    {
    public:
        explicit UniformBuffer(Renderer* Owner) : Object("Uniform Buffer"), m_Owner(Owner) {}
        ~UniformBuffer() override = default;

        virtual bool Allocate(size_t Size) = 0;
        virtual void Free() = 0;
        virtual void Copy(const void* Dest, size_t DestSize, size_t Offset = 0) = 0;

        Renderer* GetOwner() const { return m_Owner; }
    protected:
        Renderer* m_Owner = nullptr;
    };
}
