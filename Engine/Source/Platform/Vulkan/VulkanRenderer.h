#pragma once
#include "Rendering/Renderer.h"
#include "Runtime/LogCategory.h"
#include "Rendering/Vertex.h"
#include "VulkanRendererTypeConvertor.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(Vulkan, "VULKAN");
#define NOVA_VULKAN_ERROR(str, ...) NOVA_LOG(Vulkan, Verbosity::Error, str, __VA_ARGS__)
#define NOVA_VULKAN_WARNING(str, ...) NOVA_LOG(Vulkan, Verbosity::Warning, str, __VA_ARGS__)
#define NOVA_VULKAN_SUCCESS(str, ...) NOVA_LOG(Vulkan, Verbosity::Info, str, __VA_ARGS__)
#define VK_LAYER_KHRONOS_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"
#define VK_FAILED(Res) (((VkResult)(Res)) != VK_SUCCESS)

typedef struct VmaAllocator_T* VmaAllocator;
typedef struct VmaAllocation_T* VmaAllocation;

namespace Nova
{

    class VulkanCommandPool;
    class VulkanCommandBuffer;
    class VulkanSwapchain;
    class VulkanRenderTarget;
    class VulkanFence;
    class VulkanSemaphore;
    class VulkanSurface;

    struct VulkanFrameData
    {
        VulkanCommandBuffer* CommandBuffer = nullptr;
        VulkanSemaphore* SubmitSemaphore = nullptr;
        VulkanSemaphore* PresentSemaphore = nullptr;
        VulkanFence* Fence = nullptr;
    };

    struct VkFunctionPointers
    {
        #if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        PFN_vkCreateDebugUtilsMessengerEXT  vkCreateDebugUtilsMessengerEXT = nullptr;
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
        #endif
        PFN_vkCmdSetColorBlendEquationEXT   vkCmdSetColorBlendEquationEXT = nullptr;
        PFN_vkCmdSetColorBlendEnableEXT     vkCmdSetColorBlendEnableEXT = nullptr;
        PFN_vkCreateShadersEXT              vkCreateShadersEXT = nullptr;
        PFN_vkDestroyShaderEXT              vkDestroyShaderEXT = nullptr;
        PFN_vkCmdBindShadersEXT             vkCmdBindShadersEXT = nullptr;
        PFN_vkSetDebugUtilsObjectNameEXT    vkSetDebugUtilsObjectNameEXT = nullptr;
    };



    class VulkanRenderer final : public Renderer
    {
    public:
        explicit VulkanRenderer(Application* Owner);
        ~VulkanRenderer() override = default;
        
        bool Initialize(const RendererCreateInfo& CreateInfo) override;
        void Destroy() override;
        bool BeginFrame() override;
        void BeginRendering() override;
        void EndRendering() override;
        void EndFrame() override;
        void ClearDepth(float Depth) override;
        void ClearColor(const Color& color) override;
        void Clear(const Color& Color, float Depth) override;
        void Present() override;
        void SetViewport(const Viewport& Viewport) override;
        void SetScissor(const Scissor& Scissor) override;
        void Draw(size_t VertexCount, size_t FirstVertex) override;
        void DrawIndexed(size_t IndexCount, size_t Offset) override;
        void SetBlending(bool Enabled) override;
        void SetCullMode(CullMode Mode) override;
        void SetDepthCompareOperation(CompareOperation DepthFunction) override;
        void SetBlendFunction(BlendFactor ColorSource, BlendFactor ColorDest, BlendOperation ColorOperation, BlendFactor AlphaSource, BlendFactor AlphaDest, BlendOperation AlphaOperation) override;
        void SetBlendFunction(BlendFactor Source, BlendFactor Destination, BlendOperation Operation) override;
        void BindPipeline(Pipeline* Pipeline) override;
        void UpdateUniformBuffer(UniformBuffer* Buffer, u64 Offset, u64 Size, const void* Data) override;
        void BindVertexBuffer(VertexBuffer* Buffer, u64 Offset) override;
        void BindIndexBuffer(IndexBuffer* Buffer, u64 Offset) override;
        void WaitIdle() const override;

        PresentMode GetPresentMode() const;
        VkInstance GetInstance() const;
        VkPhysicalDevice GetPhysicalDevice() const;
        VkDevice GetDevice() const;
        VkQueue GetGraphicsQueue() const;
        VkQueue GetPresentQueue() const;
        VulkanSurface* GetSurface() const;
        VulkanSwapchain* GetSwapchain() const;
        VmaAllocator GetAllocator() const;

        u32 GetCurrentFrameIndex() const;
        u32 GetGraphicsQueueFamily() const;
        u32 GetPresentQueueFamily() const;
        BufferView<VulkanFrameData> GetFrameData() const;
        VulkanFrameData GetCurrentFrameData() const;
        VulkanCommandBuffer* GetCurrentCommandBuffer() const;
        u32 GetImageCount() const;
        VkDescriptorPool GetDescriptorPool() const;
        VulkanCommandPool* GetCommandPool() const;

        const VkFunctionPointers& GetFunctionPointers() const;

        VulkanRendererTypeConvertor Convertor;

    private:
        VkPhysicalDevice SelectPhysicalDevice(VkInstance Instance) const;
    private:
        u32                               m_CurrentFrameIndex = 0;
        u32                               m_NewFrameIndex = 0;
        VkInstance                        m_Instance = nullptr;
        VkFunctionPointers                m_FunctionPointers = { nullptr };
        #if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        VkDebugUtilsMessengerEXT          m_DebugMessenger = nullptr;
        #endif
        VkPhysicalDevice                  m_PhysicalDevice = nullptr;
        VkDevice                          m_Device = nullptr;
        VulkanSurface*                    m_Surface = nullptr;
        u32                               m_GraphicsQueueIndex = U32_MAX;
        u32                               m_PresentQueueIndex = U32_MAX;
        VkQueue                           m_GraphicsQueue = nullptr;
        VkQueue                           m_PresentQueue = nullptr;
        VulkanSwapchain*                  m_Swapchain = nullptr;
        VulkanCommandPool*                m_CommandPool = nullptr;
        VulkanRenderTarget*               m_RenderTarget = nullptr;
        VulkanFrameData                       m_Frames[3] = { };
        VmaAllocator                      m_Allocator = nullptr;
        VkDescriptorPool                  m_DescriptorPool = nullptr;
    };
}
