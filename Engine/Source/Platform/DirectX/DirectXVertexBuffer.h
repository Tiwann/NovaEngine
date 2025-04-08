#pragma once
#include "Graphics/VertexBuffer.h"

struct ID3D12Resource;

namespace Nova
{
    class DirectXVertexBuffer : public VertexBuffer
    {
    public:
        DirectXVertexBuffer() = default;
        DirectXVertexBuffer(const Vertex* Data, size_t Count);
        ~DirectXVertexBuffer() override;

        void SendData(const Vertex* Data, size_t Count) override;
        void Bind() const override;

        ID3D12Resource* GetHandle() const;
    private:
        ID3D12Resource* m_Handle = nullptr;
        bool m_Ready = false;
    };
}
