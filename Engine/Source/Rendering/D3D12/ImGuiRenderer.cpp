#include "ImGuiRenderer.h"
#include "RenderDevice.h"
#include "CommandBuffer.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_dx12.h>

namespace Nova::D3D12
{
    bool ImGuiRenderer::Initialize(const ImGuiRendererCreateInfo& createInfo)
    {
        if (!Nova::ImGuiRenderer::Initialize(createInfo))
            return false;
        if (!createInfo.device) return false;
        if (!createInfo.window) return false;

        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        Queue* graphicsQueue = device->GetGraphicsQueue();

        ID3D12CommandQueue* queueHandle = graphicsQueue->GetHandle();
        ID3D12Device13* deviceHandle = device->GetHandle();

        ID3D12DescriptorHeap* descriptorHeap = nullptr;
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
        descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        descriptorHeapDesc.NumDescriptors = 1;

        if (DX_FAILED(deviceHandle->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap))))
            return false;
        if (m_DescriptorHeap) m_DescriptorHeap->Release();
        m_DescriptorHeap = descriptorHeap;

        ImGui_ImplDX12_InitInfo initInfo {};
        initInfo.CommandQueue = queueHandle;
        initInfo.Device = deviceHandle;
        initInfo.NumFramesInFlight = device->GetImageCount();
        initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        initInfo.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        initInfo.SrvDescriptorHeap = m_DescriptorHeap;
        if (!ImGui_ImplDX12_Init(&initInfo))
            return false;

        return true;
    }

    void ImGuiRenderer::Destroy()
    {
        if(m_DescriptorHeap) m_DescriptorHeap->Release();
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(m_Context);
    }

    void ImGuiRenderer::BeginFrame()
    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiRenderer::EndFrame()
    {
        ImGui::EndFrame();
    }

    void ImGuiRenderer::Render(Nova::CommandBuffer& commandBuffer)
    {
        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();
        if (!drawData) return;
        CommandBuffer& cmdBuffer = static_cast<CommandBuffer&>(commandBuffer);
        ID3D12GraphicsCommandList* cmdBufferHandle = cmdBuffer.GetHandle();
        cmdBufferHandle->SetDescriptorHeaps(1, &m_DescriptorHeap);
        ImGui_ImplDX12_RenderDrawData(drawData, cmdBufferHandle);
    }
}
