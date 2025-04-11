#pragma once
#include "Vertex.h"
#include "Containers/Buffer.h"
#include "Containers/Array.h"
#include "Renderer.h"

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

        static VertexBuffer* Create(GraphicsApi const& GraphicsApi);
        static VertexBuffer* Create(const Vertex* Data, size_t Count, GraphicsApi const& GraphicsApi);
        static VertexBuffer* Create(const Array<Vertex>& Vertices, GraphicsApi const& GraphicsApi);
        size_t GetCount() const;
    protected:
        BufferView<Vertex> m_Data;
    };
}
