#pragma once
#include "Renderer.h"
#include "Containers/Buffer.h"
#include "Runtime/Types.h"
#include "Containers/Array.h"
#include "Renderer.h"

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

        static IndexBuffer* Create(GraphicsApi const& GraphicsApi);
        static IndexBuffer* Create(const u32* Indices, size_t Count, GraphicsApi const& GraphicsApi);
        static IndexBuffer* Create(const Array<u32>& Indices, GraphicsApi const& GraphicsApi);

        size_t Count() const;
    protected:
        BufferView<u32> m_Data;
    };
}
