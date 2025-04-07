#pragma once
#include "Core/Graphics/VertexArray.h"

namespace Nova
{
    class VulkanVertexArray : public VertexArray
    {
    public:
        VulkanVertexArray() : VertexArray(){}
        void Bind() const override;
        void Unbind() const override;
        void SetBufferLayout(const VertexBufferLayout& Layout) override;
    };
}
