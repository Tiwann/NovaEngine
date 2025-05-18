#pragma once
#include "RendererObject.h"
#include "Runtime/Format.h"

namespace Nova
{
    class Renderer;

    struct IndexBufferCreateInfo
    {
        Format Format;
        const void* Data;
        size_t Size;
    };

    class IndexBuffer : public RendererObject<IndexBufferCreateInfo>
    {
    public:
        explicit IndexBuffer(Renderer* Owner) : RendererObject("IndexBuffer", Owner) {}

        size_t GetCount() const { return m_Count; }
        size_t GetSize() const { return m_Size; }
    protected:
        size_t m_Count = 0;
        size_t m_Size = 0;
    };
}
