#pragma once
#include "Rendering/VertexArray.h"

namespace Nova
{
    class VulkanVertexArray : public VertexArray
    {
    public:
        VulkanVertexArray() : VertexArray(){}
        void Bind() const override;
        void Unbind() const override;
        void SetBufferLayout(const VertexLayout& Layout) override;
    };
}
