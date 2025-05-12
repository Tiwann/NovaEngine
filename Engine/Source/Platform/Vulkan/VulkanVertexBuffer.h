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
      explicit VulkanVertexBuffer(Renderer* Renderer);
      explicit VulkanVertexBuffer(Renderer* Renderer, const Vertex* Data, size_t Count);
      ~VulkanVertexBuffer() override;

      void SendData(const Vertex* Data, size_t Count) override;
      void Bind() const override;

      VkBuffer GetHandle() const;
      VkBuffer* GetHandlePtr();

   private:
      VkBuffer m_Handle = nullptr;
      VmaAllocation m_Allocation = nullptr;
      VmaAllocationInfo m_AllocationInfo = VmaAllocationInfo{ };
   };
}
