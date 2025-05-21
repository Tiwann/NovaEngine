#pragma once
#include "Rendering/VertexBuffer.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Nova
{
   class VulkanRenderer;
}

namespace Nova
{
   class VulkanVertexBuffer : public VertexBuffer
   {
   public:
      explicit VulkanVertexBuffer(Renderer* Owner);
      bool Initialize(const VertexBufferCreateInfo& CreateInfo) override;
      void SetDebugName(const String& Name) override;
      void Destroy() override;

      VkBuffer GetHandle() const;
      VkBuffer* GetHandlePtr();

   private:
      VkBuffer m_Handle = nullptr;
      VmaAllocation m_Allocation = nullptr;
      VmaAllocationInfo m_AllocationInfo = VmaAllocationInfo{ };
   };
}
