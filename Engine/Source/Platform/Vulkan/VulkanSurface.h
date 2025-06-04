#pragma once
#include "Rendering/Surface.h"

typedef struct VkSurfaceKHR_T* VkSurfaceKHR;

namespace Nova
{
    class VulkanSurface : public Surface
    {
    public:
        explicit VulkanSurface(Renderer* Owner) : Surface(Owner){}

        bool Initialize(const SurfaceCreateInfo& CreateInfo) override;
        void Destroy() override;
        void SetDebugName(const String& Name) override;

        VkSurfaceKHR GetHandle() const;
        const VkSurfaceKHR* GetHandlePtr() const;
    private:
        VkSurfaceKHR m_Handle = nullptr;
    };
}
