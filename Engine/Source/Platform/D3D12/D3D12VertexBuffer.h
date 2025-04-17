#pragma once
#include "Rendering/VertexBuffer.h"

struct ID3D12Resource;

namespace Nova
{
    class D3D12VertexBuffer : public VertexBuffer
    {
    public:
        explicit D3D12VertexBuffer(Renderer* Renderer);
        explicit D3D12VertexBuffer(Renderer* Renderer, const Vertex* Data, size_t Count);
        ~D3D12VertexBuffer() override;

        void SendData(const Vertex* Data, size_t Count) override;
        void Bind() const override;

        ID3D12Resource* GetHandle() const;
    private:
        ID3D12Resource* m_Handle = nullptr;
        bool m_Ready = false;
    };
}
