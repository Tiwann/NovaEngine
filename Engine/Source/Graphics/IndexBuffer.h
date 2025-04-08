#pragma once
#include "Containers/Buffer.h"
#include "Runtime/Types.h"
#include "Containers/Array.h"

namespace Nova
{
    class IndexBuffer
    {
    public:
        IndexBuffer();
        IndexBuffer(const u32* Indices, size_t Count);
        virtual ~IndexBuffer() = default;
        
        virtual void Bind() const = 0;
        virtual void SendData(const u32* Indices, size_t Count);
        void         SendData(const Array<u32>& Indices);

        static IndexBuffer* Create();
        static IndexBuffer* Create(const u32* Indices, size_t Count);
        static IndexBuffer* Create(const Array<u32>& Indices);

        size_t Count() const;
    protected:
        BufferView<u32> m_Data;
    };
}
