#include "D3D12IndexBuffer.h"
#include "Platform/PlatformRenderer.h"
#include "Runtime/Application.h"

#include <directx/d3dx12_barriers.h>

namespace Nova
{
    D3D12IndexBuffer::~D3D12IndexBuffer()
    {
        D3D12Renderer* Renderer = g_Application->GetRenderer<D3D12Renderer>();
        Renderer->WaitDeviceIdle();
        m_Handle->Release();
        m_Handle = nullptr;
        m_Ready = false;
    }

    D3D12IndexBuffer::D3D12IndexBuffer(const u32* Indices, size_t Count) : IndexBuffer(Indices, Count)
    {
        const D3D12Renderer* Renderer = g_Application->GetRenderer<D3D12Renderer>();
        m_Handle = Renderer->CreateBuffer(L"Index Buffer", D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, Count * sizeof(u32));
        if (!m_Handle)
        {
            NOVA_DIRECTX_ERROR("Failed to create Index Buffer");
            g_Application->RequireExit(ExitCode::Error);
            return;
        }

        D3D12IndexBuffer::SendData(Indices, Count);
    }

    void D3D12IndexBuffer::SendData(const u32* Indices, size_t Count)
    {
        IndexBuffer::SendData(Indices, Count);
        D3D12Renderer* Renderer = g_Application->GetRenderer<D3D12Renderer>();

        if (m_Handle && m_Ready)
        {
            m_Ready = false;
            Renderer->WaitDeviceIdle();
            m_Handle->Release();
        }

        m_Handle = Renderer->CreateBuffer(L"Index Buffer", D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, Count * sizeof(u32));
        if (!m_Handle)
        {
            NOVA_DIRECTX_ERROR("Failed to create Index Buffer");
            g_Application->RequireExit(ExitCode::Error);
            return;
        }
        
        ID3D12Device9* Device = Renderer->GetDevice();
        ID3D12Resource* UploadHeap = Renderer->CreateBuffer(L"Index Buffer Upload Heap", D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, Count * sizeof(u32));
        if (!UploadHeap)
        {
            NOVA_DIRECTX_ERROR("Failed to create Index Buffer Upload Heap");
            g_Application->RequireExit(ExitCode::Error);
            return;
        }

        void* MappedData = nullptr;
        constexpr D3D12_RANGE ReadRange { 0, 0 };
        if (DX_FAILED(UploadHeap->Map(0, &ReadRange, &MappedData)))
        {
            NOVA_DIRECTX_ERROR("Failed to map Upload Heap!");
            UploadHeap->Release();
            return;
        }

        memcpy(MappedData, Indices, Count * sizeof(u32));
        UploadHeap->Unmap(0, nullptr);

        ID3D12GraphicsCommandList* Cmd = nullptr;
        ID3D12CommandAllocator* CommandAllocator = Renderer->GetCurrentCommandAllocator();
        if (DX_FAILED(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator, nullptr, IID_PPV_ARGS(&Cmd))))
        {
            NOVA_DIRECTX_ERROR("Failed to create Command Buffer!");
            UploadHeap->Release();
            return;
        }
        Cmd->SetName(L"One-Time Command Buffer (Index Buffer)");
        Cmd->CopyResource(m_Handle, UploadHeap);
        const CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_Handle,
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        Cmd->ResourceBarrier(1, &Barrier);
        Cmd->Close();

        ID3D12CommandQueue* Queue = Renderer->GetCommandQueue();
        ID3D12CommandList* const Commands[]{ Cmd };
        Queue->ExecuteCommandLists(1, Commands);

        Renderer->WaitDeviceIdle();
        UploadHeap->Release();
        Cmd->Release();
        m_Ready = true;
    }

    void D3D12IndexBuffer::Bind() const
    {
        if (!m_Ready)
        {
            NOVA_DIRECTX_ERROR("Failed to bind Index Buffer: Not Ready yet!");
            return;
        }
        const D3D12Renderer* Renderer = g_Application->GetRenderer<D3D12Renderer>();
        ID3D12GraphicsCommandList* Cmd = Renderer->GetCurrentGraphicsCommandBuffer();

        D3D12_INDEX_BUFFER_VIEW BufferView;
        BufferView.BufferLocation = m_Handle->GetGPUVirtualAddress();
        BufferView.SizeInBytes = (UINT)m_Data.Size();
        BufferView.Format = DXGI_FORMAT_R32_UINT;
        Cmd->IASetIndexBuffer(&BufferView);
    }

    ID3D12Resource* D3D12IndexBuffer::GetHandle() const
    {
        return m_Handle;
    }
}
