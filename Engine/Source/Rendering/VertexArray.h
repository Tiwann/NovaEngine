#pragma once
#include "Renderer.h"

namespace Nova
{
    class VertexBufferLayout;
}

namespace Nova
{
    class VertexArray
    {
    public:
        VertexArray() = default;
        virtual ~VertexArray() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual void SetBufferLayout(const VertexBufferLayout& Layout) = 0;
        static VertexArray* Create(GraphicsApi const& GraphicsApi);
    };
}
