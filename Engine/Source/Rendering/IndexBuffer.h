#pragma once
#include "Containers/Buffer.h"
#include "Runtime/Types.h"
#include "Containers/Array.h"

namespace Nova
{
    class Renderer;

    class IndexBuffer
    {
    public:
        explicit IndexBuffer(Renderer* Renderer);
        explicit IndexBuffer(Renderer* Renderer, const u32* Indices, size_t Count);
        virtual ~IndexBuffer() = default;
        
        virtual void Bind() const = 0;
        virtual void SendData(const u32* Indices, size_t Count);
        void         SendData(const Array<u32>& Indices);
        size_t       Count() const;
    protected:
        Renderer* m_Renderer = nullptr;
        BufferView<u32> m_Data;
    };
}
