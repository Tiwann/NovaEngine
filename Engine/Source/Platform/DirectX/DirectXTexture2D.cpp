#include "DirectXTexture2D.h"
#include "Core/Application.h"
#include "Platform/PlatformRenderer.h"



namespace Hydro
{
    DirectXTexture2D::DirectXTexture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot)
        : Texture2D(Name, Width, Height, Params, Slot)
    {
    }

    void DirectXTexture2D::SetTextureParameters(const TextureParams& Params)
    {
    }

    void DirectXTexture2D::SetData(u8* Data, u32 Width, u32 Height, ImageFormat Format)
    {
        m_Width = Width;
        m_Height = Height;
        m_Format = Format;
        DirectXRenderer* Renderer = g_Application->GetRenderer<DirectXRenderer>();
        m_Resource = Renderer->CreateTexture2D(L"Texture2D", m_Width, m_Height, m_Format);
        if (!m_Resource)
        {
            NOVA_DIRECTX_ERROR("Failed to create Texture2D resource!");
            return;
        }

        const size_t UploadBufferSize = GetRequiredIntermediateSize(m_Resource, 0, 1);
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
        SubresourceData.RowPitch = (LONG_PTR)(Width * GetBytesPerPixel(m_Format));
        SubresourceData.SlicePitch = SubresourceData.RowPitch * Height;
        UpdateSubresources(Cmd, m_Resource, UploadHeap, 0, 0, 1, &SubresourceData);

        const CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_Resource,
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

    void DirectXTexture2D::SetData(const Ref<Image>& Image)
    {
        DirectXTexture2D::SetData((u8*)Image->GetData(), Image->GetWidth(), Image->GetHeight(), Image->GetFormat());
    }

    Ref<Image> DirectXTexture2D::GetImage() const
    {
        return nullptr;
    }

    void DirectXTexture2D::Bind() const
    {
    }

    void DirectXTexture2D::Unbind() const
    {
    }

    uintptr_t DirectXTexture2D::GetHandle() const
    {
        return reinterpret_cast<uintptr_t>(m_Resource);
    }

    bool DirectXTexture2D::GetPixels(Buffer<u8>& OutPixels) const
    {
        return false;
    }
}
