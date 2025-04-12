#include "D3D12VertexBuffer.h"
#include "Platform/PlatformRenderer.h"
#include "Runtime/Application.h"
#include <directx/d3dx12_barriers.h>

namespace Nova
{
    D3D12VertexBuffer::~D3D12VertexBuffer()
    {
        D3D12Renderer* Renderer = g_Application->GetRenderer<D3D12Renderer>();
        Renderer->WaitDeviceIdle();
        if (m_Handle)
        {
            m_Handle->Release();
            m_Handle = nullptr;
        }
        m_Ready = false;
    }

    D3D12VertexBuffer::D3D12VertexBuffer(const Vertex* Data, size_t Count) : VertexBuffer(Data, Count)
    {
        const D3D12Renderer* Renderer = g_Application->GetRenderer<D3D12Renderer>();
        m_Handle = Renderer->CreateBuffer(L"Vertex Buffer", D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, Count * sizeof(Vertex));
        if (!m_Handle)
        {
            NOVA_DIRECTX_ERROR("Failed to create Vertex Buffer");
            g_Application->RequireExit(ExitCode::Error);
            return;
        }
        D3D12VertexBuffer::SendData(Data, Count);
    }

    void D3D12VertexBuffer::SendData(const Vertex* Data, size_t Count)
    {
        VertexBuffer::SendData(Data, Count);
        D3D12Renderer* Renderer = g_Application->GetRenderer<D3D12Renderer>();

        if (m_Handle && m_Ready)
        {
            m_Ready = false;
            Renderer->WaitDeviceIdle();
            m_Handle->Release();
            m_Handle = Renderer->CreateBuffer(L"Vertex Buffer", D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, Count * sizeof(Vertex));
            if (!m_Handle)
            {
                NOVA_DIRECTX_ERROR("Failed to create Vertex Buffer");
                g_Application->RequireExit(ExitCode::Error);
                return;
            }
        }

        ID3D12Device9* Device = Renderer->GetDevice();
        ID3D12Resource* UploadHeap = Renderer->CreateBuffer(L"Vertex Buffer Upload Heap", D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, Count * sizeof(Vertex));
        if (!UploadHeap)
        {
            NOVA_DIRECTX_ERROR("Failed to create Vertex Buffer Upload Heap");
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

        memcpy(MappedData, Data, Count * sizeof(Vertex));
        UploadHeap->Unmap(0, nullptr);

        ID3D12GraphicsCommandList* Cmd = nullptr;
        ID3D12CommandAllocator* CommandAllocator = Renderer->GetCurrentCommandAllocator();
        if (DX_FAILED(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator, nullptr, IID_PPV_ARGS(&Cmd))))
        {
            NOVA_DIRECTX_ERROR("Failed to create Command List!");
            UploadHeap->Release();
            return;
        }
        Cmd->SetName(L"Vertex Buffer Command Buffer (One Time Copy)");
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

    void D3D12VertexBuffer::Bind() const
    {
        const D3D12Renderer* Renderer = g_Application->GetRenderer<D3D12Renderer>();
        ID3D12GraphicsCommandList* Cmd = Renderer->GetCurrentGraphicsCommandBuffer();

        D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
        VertexBufferView.BufferLocation = m_Handle->GetGPUVirtualAddress();
        VertexBufferView.SizeInBytes = (UINT)m_Data.Size();
        VertexBufferView.StrideInBytes = sizeof(Vertex);
        Cmd->IASetVertexBuffers(0, 1, &VertexBufferView);
    }

    ID3D12Resource* D3D12VertexBuffer::GetHandle() const
    {
        return m_Handle;
    }
}
