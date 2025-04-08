#pragma once
#include "Vertex.h"
#include "Runtime/Buffer.h"
#include "Containers/Array.h"

namespace Nova
{
    
    class VertexBuffer
    {
    public:
        VertexBuffer();
        VertexBuffer(const Vertex* Data, size_t Count);
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void SendData(const Vertex* Data, size_t Count);
        void         SendData(const Array<Vertex>& Vertices);

        static VertexBuffer* Create();
        static VertexBuffer* Create(const Vertex* Data, size_t Count);
        static VertexBuffer* Create(const Array<Vertex>& Vertices);
        size_t GetCount() const;
    protected:
        BufferView<Vertex> m_Data;
    };
}
