#pragma once
#include "Rendering/VertexArray.h"

namespace Nova
{
    class D3D12VertexArray : public VertexArray
    {
    public:
        D3D12VertexArray() = default;
        void Bind() const override;
        void Unbind() const override;
        void SetBufferLayout(const VertexLayout& Layout) override;
    };
}
