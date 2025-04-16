#include "D3D12Texture2D.h"
#include "Runtime/Application.h"
#include "Platform/PlatformRenderer.h"

#include <directx/d3dx12_barriers.h>
#include <directx/d3dx12_resource_helpers.h>


namespace Nova
{
    D3D12Texture2D::D3D12Texture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot)
        : Texture2D(Name, Width, Height, Params, Slot)
    {
    }

    void D3D12Texture2D::SetTextureParameters(const TextureParams& Params)
    {
    }

    void D3D12Texture2D::SetData(u8* Data, u32 Width, u32 Height, const Format& Format)
    {
        m_Width = Width;
        m_Height = Height;
        m_Params.Format = Format;
        D3D12Renderer* Renderer = g_Application->GetRenderer<D3D12Renderer>();
        m_Handle = Renderer->CreateTexture2D(L"Texture2D", m_Width, m_Height, Format);
        if (!m_Handle)
        {
            NOVA_DIRECTX_ERROR("Failed to create Texture2D resource!");
            return;
        }

        const size_t UploadBufferSize = GetRequiredIntermediateSize(m_Handle, 0, 1);
        ID3D12Resource* UploadHeap = Renderer->CreateBuffer(L"Texture2D Upload Heap", D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, UploadBufferSize);
        if (!UploadHeap)
        {
            NOVA_DIRECTX_ERROR("Failed to create Texture2D Upload Heap!");
            return;
        }

        ID3D12GraphicsCommandList* Cmd = Renderer->CreateOneTimeCommandBuffer();
        if (!Cmd)
        {
            NOVA_DIRECTX_ERROR("Failed to create Texture2D Command Buffer!");
            UploadHeap->Release();
            return;
        }

        D3D12_SUBRESOURCE_DATA SubresourceData;
        SubresourceData.pData = Data;
        SubresourceData.RowPitch = (LONG_PTR)(Width * GetFormatSize(m_Params.Format));
        SubresourceData.SlicePitch = SubresourceData.RowPitch * Height;
        UpdateSubresources(Cmd, m_Handle, UploadHeap, 0, 0, 1, &SubresourceData);

        const CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_Handle,
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        Cmd->ResourceBarrier(1, &Barrier);
        Cmd->Close();

        ID3D12CommandQueue* Queue = Renderer->GetCommandQueue();
        ID3D12CommandList* const Commands[]{ Cmd };
        Queue->ExecuteCommandLists(1, Commands);

        Renderer->WaitDeviceIdle();
        UploadHeap->Release();
        Cmd->Release();
    }

    void D3D12Texture2D::Bind() const
    {
    }

    void D3D12Texture2D::Unbind() const
    {
    }

    uintptr_t D3D12Texture2D::GetHandle() const
    {
        return reinterpret_cast<uintptr_t>(m_Handle);
    }
}
