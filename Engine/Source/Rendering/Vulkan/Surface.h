#pragma once
#include "Rendering/Surface.h"

typedef struct VkSurfaceKHR_T* VkSurfaceKHR;

namespace Nova { class Device; }

namespace Nova::Vulkan
{
    class Surface final : public Nova::Surface
    {
    public:
        bool Initialize(const SurfaceCreateInfo& createInfo) override;
        void Destroy() override;

        VkSurfaceKHR GetHandle() const;
        const VkSurfaceKHR* GetHandlePtr() const;
    private:
        VkSurfaceKHR m_Handle = nullptr;
    };
}