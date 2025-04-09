#pragma once
#include "Graphics/IndexBuffer.h"

struct ID3D12Resource;
namespace Nova
{
    class D3D12IndexBuffer : public IndexBuffer
    {
    public:
        D3D12IndexBuffer() = default;
        D3D12IndexBuffer(const u32* Indices, size_t Count);
        ~D3D12IndexBuffer() override;
        void SendData(const u32* Indices, size_t Count) override;
        void Bind() const override;

        ID3D12Resource* GetHandle() const;
    private:
        ID3D12Resource* m_Handle;
        bool m_Ready = false;
    };
}
