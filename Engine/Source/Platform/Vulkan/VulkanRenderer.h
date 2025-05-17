#pragma once
#include <vk_mem_alloc.h>

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

namespace Nova
{
    class VulkanSwapchain;
    class VulkanCommandPool;
    class VulkanCommandBuffer;

    struct VkFrameData
    {
        VulkanCommandBuffer* CommandBuffer = nullptr;
        VkImageView ColorImageView = nullptr;
        VkImage ColorImage = nullptr;
        VkImageView DepthImageView = nullptr;
        VkImage DepthImage = nullptr;
        VmaAllocation DepthImageAllocation = nullptr;
        VmaAllocationInfo DepthImageAllocationInfo;
        VkFormat Format;
        VkSemaphore SubmitSemaphore = nullptr;
        VkSemaphore PresentSemaphore = nullptr;
        VkFence Fence = nullptr;
    };

    struct VkFunctionPointers
    {
        #if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        PFN_vkCreateDebugUtilsMessengerEXT  vkCreateDebugUtilsMessengerEXT = nullptr;
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
        #endif
        PFN_vkCmdSetColorBlendEquationEXT   vkCmdSetColorBlendEquationEXT = nullptr;
        PFN_vkCmdSetColorBlendEnableEXT     vkCmdSetColorBlendEnableEXT = nullptr;
    };



    class VulkanRenderer final : public Renderer
    {
    public:
        explicit VulkanRenderer(Application* Owner);
        ~VulkanRenderer() override = default;
        
        bool Initialize() override;
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
        void DrawIndexed(size_t IndexCount, u64 Offset) override;
        void SetBlending(bool Enabled) override;
        void SetCullMode(CullMode Mode) override;
        void SetDepthCompareOperation(CompareOperation DepthFunction) override;
        void SetBlendFunction(BlendFactor ColorSource, BlendFactor ColorDest, BlendOperation ColorOperation, BlendFactor AlphaSource, BlendFactor AlphaDest, BlendOperation AlphaOperation) override;
        void SetBlendFunction(BlendFactor Source, BlendFactor Destination, BlendOperation Operation) override;
        void BindPipeline(Pipeline* Pipeline) override;
        void BindVertexBuffer(VertexBuffer* Buffer, u64 Offset) override;
        void BindIndexBuffer(IndexBuffer* Buffer, u64 Offset) override;
        void UpdateUniformBuffer(UniformBuffer* Buffer, u64 Offset, u64 Size, const void* Data) override;


        PresentMode GetPresentMode() override;
        VkInstance GetInstance() const;
        VkPhysicalDevice GetPhysicalDevice() const;
        VkDevice GetDevice() const;
        VkQueue GetGraphicsQueue() const;
        VkQueue GetPresentQueue() const;
        VkSurfaceKHR GetSurface() const;
        VkSwapchainKHR GetSwapchain() const;
        u32 GetGraphicsQueueFamily() const;
        u32 GetPresentQueueFamily() const;
        VmaAllocator GetAllocator() const;

        BufferView<VkFrameData> GetFrameData() const;
        VkFrameData GetCurrentFrameData() const;
        VulkanCommandBuffer* GetCurrentCommandBuffer() const;
        VulkanCommandPool* GetCommandPool() const;
        VkImage GetCurrentImage() const;
        VkImageView GetCurrentImageView() const;
        u32 GetImageCount() const;
        VkDescriptorPool GetDescriptorPool() const;
        void WaitIdle() const;
        const VkFunctionPointers& GetFunctionPointers() const;
        u32 GetCurrentFrameIndex() const;
        VulkanRendererTypeConvertor Convertor;
    private:
        u32                               m_CurrentFrameIndex = 0;
        u32                               m_NewFrameIndex = 0;
        VkInstance                        m_Instance = nullptr;
        VkFunctionPointers                m_FunctionPointers = {  };
        #if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        VkDebugUtilsMessengerEXT          m_DebugMessenger = nullptr;
        #endif
        VkPhysicalDevice                  m_PhysicalDevice = nullptr;
        VkDevice                          m_Device = nullptr;
        VkSurfaceKHR                      m_Surface = nullptr;
        u32                               m_GraphicsQueueIndex = U32_MAX;
        u32                               m_PresentQueueIndex = U32_MAX;
        u32                               m_ImageCount = 0;
        VkQueue                           m_GraphicsQueue = nullptr;
        VkQueue                           m_PresentQueue = nullptr;
        VkPresentModeKHR                  m_PresentMode;
        VkSwapchainKHR                    m_Swapchain = nullptr;
        VkFrameData                       m_Frames[3] = { };
        VmaAllocator                      m_Allocator = nullptr;
        VkDescriptorPool                  m_DescriptorPool = nullptr;
        VulkanCommandPool*                m_CommandPool = nullptr;
    };
}
