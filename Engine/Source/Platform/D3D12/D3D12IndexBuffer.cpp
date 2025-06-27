#include "D3D12IndexBuffer.h"
#include "Platform/PlatformRenderer.h"
#include "Runtime/Application.h"
#include "Runtime/Log.h"

#include <directx/d3dx12_barriers.h>

#include "Containers/StringConversion.h"


namespace Nova
{
    D3D12IndexBuffer::D3D12IndexBuffer(Renderer* Renderer) : IndexBuffer(Renderer)
    {
    }

    bool D3D12IndexBuffer::Initialize(const IndexBufferCreateInfo& CreateInfo)
    {
        const D3D12Renderer* Renderer = m_Owner->As<D3D12Renderer>();

        if (m_Handle)
        {
            Renderer->WaitIdle();
            m_Handle->Release();
        }


        m_Handle = Renderer->CreateBuffer(D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, CreateInfo.Size);
        if (!m_Handle)
            return false;

        ID3D12Device9* Device = Renderer->GetDevice();
        ID3D12Resource* UploadHeap = Renderer->CreateBuffer(D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, CreateInfo.Size);
        if (!UploadHeap)
            return false;

        (void)UploadHeap->SetName(L"Index Buffer Upload Heap");

        void* MappedData = nullptr;
        constexpr D3D12_RANGE ReadRange { 0, 0 };
        if (DX_FAILED(UploadHeap->Map(0, &ReadRange, &MappedData)))
        {
            UploadHeap->Release();
            return false;
        }

        memcpy(MappedData, CreateInfo.Data, CreateInfo.Size);
        UploadHeap->Unmap(0, nullptr);

        ID3D12GraphicsCommandList* Cmd = nullptr;
        ID3D12CommandAllocator* CommandAllocator = Renderer->GetCurrentCommandAllocator();
        if (DX_FAILED(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator, nullptr, IID_PPV_ARGS(&Cmd))))
        {
            UploadHeap->Release();
            return false;
        }

        (void)Cmd->SetName(L"Command Buffer (Index Buffer)");
        Cmd->CopyResource(m_Handle, UploadHeap);
        const CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_Handle,
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        Cmd->ResourceBarrier(1, &Barrier);
        (void)Cmd->Close();

        ID3D12CommandQueue* Queue = Renderer->GetCommandQueue();
        ID3D12CommandList* const Commands[]{ Cmd };
        Queue->ExecuteCommandLists(1, Commands);

        Renderer->WaitIdle();
        UploadHeap->Release();
        Cmd->Release();
        return true;
    }

    void D3D12IndexBuffer::SetDebugName(const String& Name)
    {
        if constexpr(!RendererIsDebug)
            return;
        (void)m_Handle->SetName(*StringConvertToWide(Name));
    }

    void D3D12IndexBuffer::Destroy()
    {
        if (!m_Handle)
            return;

        const D3D12Renderer* Renderer = m_Owner->As<D3D12Renderer>();
        Renderer->WaitIdle();
        m_Handle->Release();
        m_Handle = nullptr;
    }

    ID3D12Resource* D3D12IndexBuffer::GetHandle() const
    {
        return m_Handle;
    }
}
