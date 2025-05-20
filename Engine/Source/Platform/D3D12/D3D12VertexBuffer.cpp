#include "D3D12VertexBuffer.h"
#include "Platform/PlatformRenderer.h"
#include "Runtime/Application.h"
#include "Runtime/Log.h"
#include <directx/d3dx12_barriers.h>

namespace Nova
{
    D3D12VertexBuffer::D3D12VertexBuffer(Renderer* Renderer) : VertexBuffer(Renderer)
    {

    }

    D3D12VertexBuffer::D3D12VertexBuffer(Renderer* Renderer, const Vertex* Data, size_t Count) : VertexBuffer(Renderer, Data, Count)
    {
        const D3D12Renderer* CastedRenderer = dynamic_cast<D3D12Renderer*>(m_Renderer);
        m_Handle = CastedRenderer->CreateBuffer(D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, Count * sizeof(Vertex));
        if (!m_Handle)
        {
            NOVA_DIRECTX_ERROR("Failed to create Vertex Buffer");
            g_Application->RequireExit(ExitCode::Error);
            return;
        }
        D3D12VertexBuffer::SendData(Data, Count);
    }

    D3D12VertexBuffer::~D3D12VertexBuffer()
    {
        D3D12Renderer* CastedRenderer = dynamic_cast<D3D12Renderer*>(m_Renderer);
        CastedRenderer->WaitIdle();
        if (m_Handle)
        {
            m_Handle->Release();
            m_Handle = nullptr;
        }
        m_Ready = false;
    }

    void D3D12VertexBuffer::SendData(const Vertex* Data, size_t Count)
    {
        VertexBuffer::SendData(Data, Count);
        D3D12Renderer* CastedRenderer = dynamic_cast<D3D12Renderer*>(m_Renderer);

        if (m_Handle && m_Ready)
        {
            m_Ready = false;
            CastedRenderer->WaitIdle();
            m_Handle->Release();
            m_Handle = CastedRenderer->CreateBuffer(D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, Count * sizeof(Vertex));
            if (!m_Handle)
            {
                NOVA_DIRECTX_ERROR("Failed to create Vertex Buffer");
                g_Application->RequireExit(ExitCode::Error);
                return;
            }
        }

        ID3D12Device9* Device = CastedRenderer->GetDevice();
        ID3D12Resource* UploadHeap = CastedRenderer->CreateBuffer(D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, Count * sizeof(Vertex));
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
        ID3D12CommandAllocator* CommandAllocator = CastedRenderer->GetCurrentCommandAllocator();
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

        ID3D12CommandQueue* Queue = CastedRenderer->GetCommandQueue();
        ID3D12CommandList* const Commands[]{ Cmd };
        Queue->ExecuteCommandLists(1, Commands);


        CastedRenderer->WaitIdle();
        UploadHeap->Release();
        Cmd->Release();
        m_Ready = true;
    }

    void D3D12VertexBuffer::Bind() const
    {
        D3D12Renderer* CastedRenderer = dynamic_cast<D3D12Renderer*>(m_Renderer);
        ID3D12GraphicsCommandList* Cmd = CastedRenderer->GetCurrentGraphicsCommandBuffer();

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
