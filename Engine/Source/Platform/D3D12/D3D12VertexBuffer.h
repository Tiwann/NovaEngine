#pragma once
#include "Rendering/VertexBuffer.h"

struct ID3D12Resource;

namespace Nova
{
    class D3D12VertexBuffer : public VertexBuffer
    {
    public:
        explicit D3D12VertexBuffer(Renderer* Owner);

        ID3D12Resource* GetHandle() const;
        bool Initialize(const VertexBufferCreateInfo& CreateInfo) override;
        void SetDebugName(const String& Name) override;
        void Destroy() override;

    private:
        ID3D12Resource* m_Handle = nullptr;
    };
}
