#pragma once
#include "Graphics/VertexBuffer.h"

struct ID3D12Resource;

namespace Nova
{
    class D3D12VertexBuffer : public VertexBuffer
    {
    public:
        D3D12VertexBuffer() = default;
        D3D12VertexBuffer(const Vertex* Data, size_t Count);
        ~D3D12VertexBuffer() override;

        void SendData(const Vertex* Data, size_t Count) override;
        void Bind() const override;

        ID3D12Resource* GetHandle() const;
    private:
        ID3D12Resource* m_Handle = nullptr;
        bool m_Ready = false;
    };
}
