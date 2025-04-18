#pragma once
#include "Rendering/Renderer.h"
#include "Runtime/LogCategory.h"
#include "Rendering/Vertex.h"
#include "Rendering/SamplerAddressMode.h"
#include "Rendering/Filter.h"
#include "Runtime/Format.h"
#include "Rendering/FrontFace.h"
#include "Rendering/PolygonMode.h"
#include "Rendering/PrimitiveTopology.h"
#include "Rendering/ShaderStage.h"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "VulkanRendererTypeConvertor.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(Vulkan, "VULKAN");

#define NOVA_VULKAN_ERROR(str, ...) NOVA_LOG(Vulkan, Verbosity::Error, str, __VA_ARGS__)
#define NOVA_VULKAN_WARNING(str, ...) NOVA_LOG(Vulkan, Verbosity::Warning, str, __VA_ARGS__)
#define NOVA_VULKAN_SUCCESS(str, ...) NOVA_LOG(Vulkan, Verbosity::Info, str, __VA_ARGS__)
#define VK_LAYER_KHRONOS_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"
#define VK_FAILED(Res) (((VkResult)(Res)) != VK_SUCCESS)
#define VK_CHECK_MSG(Res, Msg, ...) do { if(VK_FAILED((Res))) NOVA_VULKAN_ERROR((Msg), __VA_ARGS__); }  while(0)

namespace Nova
{
    struct VkFrameData
    {
        VkCommandBuffer CommandBuffer;
        VkImageView ImageView;
        VkImage Image;
        VkFormat Format;
        VkSemaphore SubmitSemaphore;
        VkSemaphore PresentSemaphore;
        VkFence Fence;
    };

    struct VkFunctionPointers
    {
        #if defined(NOVA_DEBUG)
        PFN_vkCreateDebugUtilsMessengerEXT  vkCreateDebugUtilsMessengerEXT = nullptr;
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
        #endif
        PFN_vkCmdSetColorBlendEquationEXT   vkCmdSetColorBlendEquationEXT = nullptr;
        PFN_vkCmdSetColorBlendEnableEXT     vkCmdSetColorBlendEnableEXT = nullptr;
        PFN_vkCreateShadersEXT              vkCreateShadersEXT = nullptr;
        PFN_vkDestroyShaderEXT              vkDestroyShaderEXT = nullptr;
        PFN_vkCmdBindShadersEXT             vkCmdBindShadersEXT = nullptr;
    };

    class VulkanRenderer final : public Renderer
    {
    public:
        explicit VulkanRenderer(Application* Owner);
        ~VulkanRenderer() override = default;
        
        bool Initialize() override;
        void Destroy() override;
        bool BeginFrame() override;
        void EndFrame() override;
        void ClearDepth(float Depth) override;
        void ClearColor(const Color& color) override;
        void Clear(const Color& Color, float Depth) override;
        void Present() override;
        void SetViewportRect(Vector2 Position, Vector2 Size) override;
        void Draw(VertexArray* VAO, u32 NumVert, Shader* Shader) override;
        void DrawIndexed(VertexArray* VertexArray, VertexBuffer* VertexBuffer, IndexBuffer* IndexBuffer, Shader* Shader) override;
        void SetBlending(bool Enabled) override;
        void SetCullMode(CullMode Mode) override;
        void SetDepthCompareOperation(CompareOperation DepthFunction) override;
        void SetBlendFunction(BlendFactor ColorSource, BlendFactor ColorDest, BlendOperation ColorOperation, BlendFactor AlphaSource, BlendFactor AlphaDest, BlendOperation AlphaOperation) override;
        void SetBlendFunction(BlendFactor Source, BlendFactor Destination, BlendOperation Operation) override;

        void BindPipeline(Pipeline* Pipeline) override;
        
        VkInstance GetInstance() const;
        VkPhysicalDevice GetPhysicalDevice() const;
        VkDevice GetDevice() const;
        VkQueue GetGraphicsQueue() const;
        VkQueue GetPresentQueue() const;
        VkSurfaceKHR GetSurface() const;
        VkSwapchainKHR GetSwapchain() const;
        u32 GetGraphicsQueueFamily() const;
        VmaAllocator GetAllocator() const;
        
        BufferView<VkFrameData> GetFrameData() const;
        VkFrameData GetCurrentFrameData() const;
        VkCommandBuffer GetCurrentCommandBuffer() const;
        VkImage GetCurrentImage() const;
        VkImageView GetCurrentImageView() const;
        u32 GetImageCount() const;
        VkDescriptorPool GetDescriptorPool() const;
        VkCommandPool GetCommandPool() const;
        void WaitIdle() const;
        const VkFunctionPointers& GetFunctionPointers() const;
        VulkanRendererTypeConvertor Convertor;
        static constexpr VkComponentMapping DefaultComponentMapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
    private:
        u32                               m_CurrentFrameIndex = 0;
        u32                               m_NewFrameIndex = 0;
        VkInstance                        m_Instance = nullptr;
        VkFunctionPointers                m_FunctionPointers = {  };
        #ifdef NOVA_DEBUG
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
        VkColorSpaceKHR                   m_ImageColorSpace;
        VkPresentModeKHR                  m_PresentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
        VkSwapchainKHR                    m_Swapchain = nullptr;
        VkFormat                          m_SwapchainImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
        VkFrameData                       m_Frames[3] = { };
        VkCommandPool                     m_CommandPool = nullptr;
        VmaAllocator                      m_Allocator = nullptr;
        VkDescriptorPool                  m_DescriptorPool = nullptr;
    };
}
