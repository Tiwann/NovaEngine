#include "TextureView.h"
#include "Texture.h"
#include "Conversions.h"
#include "RenderDevice.h"

#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    bool TextureView::Initialize(const TextureViewCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;

        VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_1D;
        if (createInfo.height > 1) viewType = VK_IMAGE_VIEW_TYPE_2D;
        if (createInfo.depth > 1) viewType = VK_IMAGE_VIEW_TYPE_3D;

        const Texture* texture = dynamic_cast<const Texture*>(createInfo.texture);
        if (!texture) return false;

        VkImageViewCreateInfo imageViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        imageViewCreateInfo.image = texture->GetImage();
        imageViewCreateInfo.viewType = viewType;
        imageViewCreateInfo.format = Convert<VkFormat>(createInfo.format);
        imageViewCreateInfo.subresourceRange.aspectMask = Convert<VkImageAspectFlags>(createInfo.aspectFlags);
        imageViewCreateInfo.subresourceRange.baseMipLevel = createInfo.baseMipLevel;
        imageViewCreateInfo.subresourceRange.levelCount = createInfo.mipCount;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        const RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        const VkDevice deviceHandle = device->GetHandle();
        vkDestroyImageView(deviceHandle, m_Handle, nullptr);
        if (vkCreateImageView(deviceHandle, &imageViewCreateInfo, nullptr, &m_Handle) != VK_SUCCESS)
            return false;

        m_Device = createInfo.device;
        m_Texture = createInfo.texture;
        m_Format = createInfo.format;
        m_AspectFlags = createInfo.aspectFlags;
        m_Width = createInfo.width;
        m_Height = createInfo.height;
        m_Depth = createInfo.depth;
        m_BaseMipLevel = createInfo.baseMipLevel;
        m_MipCount = createInfo.mipCount;
        return true;
    }

    void TextureView::Destroy()
    {
        const RenderDevice* device = static_cast<RenderDevice*>(m_Device);
        const VkDevice deviceHandle = device->GetHandle();
        vkDestroyImageView(deviceHandle, m_Handle, nullptr);
        m_Handle = nullptr;
    }

    VkImage TextureView::GetImage() const
    {
        const Texture* texture = dynamic_cast<const Texture*>(m_Texture);
        if (!texture) return nullptr;
        return texture->GetImage();
    }

    VkImageView TextureView::GetHandle() const
    {
        return m_Handle;
    }
}
