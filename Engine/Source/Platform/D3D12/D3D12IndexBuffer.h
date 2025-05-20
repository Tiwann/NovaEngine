#pragma once
#include "Rendering/IndexBuffer.h"

struct ID3D12Resource;
namespace Nova
{
    class D3D12IndexBuffer : public IndexBuffer
    {
    public:
        explicit D3D12IndexBuffer(Renderer* Renderer);

        bool Initialize(const IndexBufferCreateInfo& CreateInfo) override;
        void SetDebugName(const String& Name) override;
        void Destroy() override;

        ID3D12Resource* GetHandle() const;
    private:
        ID3D12Resource* m_Handle = nullptr;
    };
}
