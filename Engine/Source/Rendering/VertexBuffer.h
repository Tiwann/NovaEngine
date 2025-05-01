#pragma once
#include "Vertex.h"
#include "Containers/Buffer.h"
#include "Containers/Array.h"
#include "Runtime/Object.h"

namespace Nova
{
    class Renderer;

    class VertexBuffer : public Object
    {
    public:
        explicit VertexBuffer(Renderer* Renderer);
        explicit VertexBuffer(Renderer* Renderer, const Vertex* Data, size_t Count);
        ~VertexBuffer() override = default;

        virtual void Bind() const = 0;
        virtual void SendData(const Vertex* Data, size_t Count);
        void         SendData(const Array<Vertex>& Vertices);
        size_t GetCount() const;
    protected:
        Renderer* m_Renderer = nullptr;
        BufferView<Vertex> m_Data;
    };
}
