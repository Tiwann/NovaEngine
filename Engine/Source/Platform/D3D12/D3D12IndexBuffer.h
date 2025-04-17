#pragma once
#include "Rendering/IndexBuffer.h"

struct ID3D12Resource;
namespace Nova
{
    class D3D12IndexBuffer : public IndexBuffer
    {
    public:
        explicit D3D12IndexBuffer(Renderer* Renderer);
        explicit D3D12IndexBuffer(Renderer* Renderer, const u32* Indices, size_t Count);
        ~D3D12IndexBuffer() override;
        void SendData(const u32* Indices, size_t Count) override;
        void Bind() const override;

        ID3D12Resource* GetHandle() const;
    private:
        ID3D12Resource* m_Handle = nullptr;
        bool m_Ready = false;
    };
}
