#include "HelloCompute.h"
#include "Rendering/CommandBuffer.h"
#include "Rendering/Texture.h"
#include "Rendering/Shader.h"
#include "Rendering/Vulkan/Shader.h"
#include "Rendering/Vulkan/Texture.h"
#include "Rendering/Vulkan/Device.h"
#include "Runtime/Path.h"

void HelloCompute::OnInit()
{
    Application::OnInit();

    AssetDatabase& assetDatabase = GetAssetDatabase();
    const Ref<Window>& window = GetWindow();
    const Ref<Device>& device = GetDevice();

    TextureCreateInfo texCreateInfo;
    texCreateInfo.device = device;
    texCreateInfo.usageFlags = TextureUsageFlagBits::Storage;
    texCreateInfo.width = window->GetWidth();
    texCreateInfo.height = window->GetHeight();
    texCreateInfo.depth = 1;
    texCreateInfo.format = Format::R32G32B32A32_FLOAT;
    texCreateInfo.mips = 1;
    texCreateInfo.sampleCount = 1;
    texCreateInfo.data = nullptr;
    texCreateInfo.dataSize = 0;

    m_Texture = assetDatabase.CreateAsset<Vulkan::Texture>("Storage Texture");
    m_Texture->Initialize(texCreateInfo);

    ShaderCreateInfo shaderCreateInfo;
    shaderCreateInfo.device = device;
    shaderCreateInfo.slang = GetSlangSession();
    shaderCreateInfo.target = ShaderTarget::SPIRV;
    shaderCreateInfo.entryPoints.Add({ "computeMain", ShaderStageFlagBits::Compute });
    shaderCreateInfo.moduleInfo = { "HelloCompute", Path::GetAssetPath("Shaders/HelloCompute.slang") };

    m_Shader = assetDatabase.CreateAsset<Vulkan::Shader>("Compute");
    m_Shader->Initialize(shaderCreateInfo);

    ComputePipelineCreateInfo cpCreateInfo;
    cpCreateInfo.device = device;
    cpCreateInfo.shader = m_Shader;
    m_Pipeline.Initialize(cpCreateInfo);

    m_Shader->BindTexture(0, 0, *m_Texture);
}

void HelloCompute::OnPreRender(CommandBuffer& cmdBuffer)
{
    Application::OnRender(cmdBuffer);
    cmdBuffer.BindComputePipeline(m_Pipeline);
    cmdBuffer.BindShader(*m_Shader);

    constexpr uint32_t workGroupSizeX = 16;
    constexpr uint32_t workGroupSizeY = 16;

    const uint32_t numGroupsX = (m_Texture->GetWidth() + workGroupSizeX - 1) / workGroupSizeX;
    const uint32_t numGroupsY = (m_Texture->GetHeight() + workGroupSizeY - 1) / workGroupSizeY;

    cmdBuffer.Dispatch(numGroupsX, numGroupsY, 1);

    const Ref<Vulkan::Device>& device = GetDevice().As<Vulkan::Device>();
    auto* swapchain = device->GetSwapchain();
    const Vulkan::Texture& swapchainTexture = swapchain->GetCurrentTexture();
    cmdBuffer.Blit(*m_Texture, swapchainTexture, Filter::Linear);
}

namespace Nova
{
    extern "C" Application* CreateApplication()
    {
        return new HelloCompute;
    }
}
