#pragma once
#include "Rendering/Surface.h"

typedef struct VkSurfaceKHR_T* VkSurfaceKHR;

namespace Nova::Rendering { class Device; }

namespace Nova::Vulkan
{
    class Surface : public Rendering::Surface
    {
    public:
        bool Initialize(const Rendering::SurfaceCreateInfo& createInfo) override;
        void Destroy() override;

        VkSurfaceKHR GetHandle() const;
        const VkSurfaceKHR* GetHandlePtr() const;
    private:
        VkSurfaceKHR m_Handle = nullptr;
    };
}