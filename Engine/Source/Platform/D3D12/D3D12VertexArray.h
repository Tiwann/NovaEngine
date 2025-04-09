#pragma once
#include "Graphics/VertexArray.h"

namespace Nova
{
    class D3D12VertexArray : public VertexArray
    {
    public:
        D3D12VertexArray() = default;
        void Bind() const override;
        void Unbind() const override;
        void SetBufferLayout(const VertexBufferLayout& Layout) override;
    };
}
