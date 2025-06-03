#include "D3D12ImGuiRenderer.h"
#include "Platform/PlatformRenderer.h"
#include "Runtime/Window.h"
#include "Runtime/DesktopWindow.h"
#include "Runtime/Application.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_dx12.h>
#include <GLFW/glfw3.h>
#include <ImGuizmo.h>


namespace Nova
{
    bool D3D12ImGuiRenderer::Initialize(Application* Application)
    {
        ImGuiRenderer::Initialize(Application);
        DesktopWindow* Window = Application->GetWindow()->As<DesktopWindow>();
        GLFWwindow* NativeWindow = Window->GetHandle();
        
        if(!ImGui_ImplGlfw_InitForOther(NativeWindow, true))
        {
            NOVA_IMGUI_ERROR("Failed to initialize ImGui for GLFW(DX12)!");
            return false;
        }

        const D3D12Renderer* Renderer = Application->GetRenderer()->As<D3D12Renderer>();
        ID3D12Device9* Device = Renderer->GetDevice();
        ID3D12DescriptorHeap* DescriptorHeap = Renderer->GetImGuiDescriptorHeap();
        const u64 CPUHandle = DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr;
        const u64 GPUHandle = DescriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr;
        
        if (!ImGui_ImplDX12_Init(Device, (i32)Renderer->GetImageCount(), DXGI_FORMAT_R8G8B8A8_UNORM, DescriptorHeap, {CPUHandle}, {GPUHandle}))
        {
            NOVA_IMGUI_ERROR("Failed to initialize ImGui for DX12!");
            return false;
        }
        return true;
    }

    void D3D12ImGuiRenderer::Shutdown()
    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(m_Context);
    }

    void D3D12ImGuiRenderer::BeginFrame()
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }
    

    void D3D12ImGuiRenderer::Render()
    {
        ImGui::Render();
        ImDrawData* Data = ImGui::GetDrawData();
        const D3D12Renderer* Renderer = g_Application->GetRenderer()->As<D3D12Renderer>();
        ID3D12GraphicsCommandList* Cmd = Renderer->GetCurrentGraphicsCommandBuffer();
        ID3D12DescriptorHeap* DescriptorHeap = Renderer->GetImGuiDescriptorHeap();
        Cmd->SetDescriptorHeaps(1, &DescriptorHeap);
        ImGui_ImplDX12_RenderDrawData(Data, Cmd);
    }
}
