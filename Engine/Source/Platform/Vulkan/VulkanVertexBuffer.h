#pragma once
#include "Core/Graphics/VertexBuffer.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Hydro
{
   class VulkanRenderer;
}

namespace Hydro
{
   class VulkanVertexBuffer : public VertexBuffer
   {
   public:
      VulkanVertexBuffer();
      ~VulkanVertexBuffer() override;
      VulkanVertexBuffer(const Vertex* Data, size_t Count);

      void SendData(const Vertex* Data, size_t Count) override;
      void Bind() const override;
   private:
      VulkanRenderer* m_Renderer = nullptr;
      VkBuffer m_Handle = nullptr;
      VmaAllocation m_Allocation = nullptr;
      VmaAllocationInfo m_AllocationInfo = VmaAllocationInfo{ };
   };
}
