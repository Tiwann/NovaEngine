#pragma once
#include "Graphics/IndexBuffer.h"

struct ID3D12Resource;
namespace Nova
{
    class DirectXIndexBuffer : public IndexBuffer
    {
    public:
        DirectXIndexBuffer() = default;
        DirectXIndexBuffer(const u32* Indices, size_t Count);
        ~DirectXIndexBuffer() override;
        void SendData(const u32* Indices, size_t Count) override;
        void Bind() const override;

        ID3D12Resource* GetHandle() const;
    private:
        ID3D12Resource* m_Handle;
        bool m_Ready = false;
    };
}
