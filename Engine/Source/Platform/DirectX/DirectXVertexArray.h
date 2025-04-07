#pragma once
#include "Core/Graphics/VertexArray.h"

namespace Nova
{
    class DirectXVertexArray : public VertexArray
    {
    public:
        DirectXVertexArray() = default;
        void Bind() const override;
        void Unbind() const override;
        void SetBufferLayout(const VertexBufferLayout& Layout) override;
    };
}
