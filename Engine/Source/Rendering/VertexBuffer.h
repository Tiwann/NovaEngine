#pragma once
#include "Vertex.h"
#include "Containers/Buffer.h"
#include "Containers/Array.h"

namespace Nova
{
    class Renderer;

    class VertexBuffer
    {
    public:
        explicit VertexBuffer(Renderer* Renderer);
        explicit VertexBuffer(Renderer* Renderer, const Vertex* Data, size_t Count);
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void SendData(const Vertex* Data, size_t Count);
        void         SendData(const Array<Vertex>& Vertices);
        size_t GetCount() const;
    protected:
        Renderer* m_Renderer = nullptr;
        BufferView<Vertex> m_Data;
    };
}
