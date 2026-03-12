#pragma once

namespace Nova
{
    struct TextureCreateInfo;

    class DeviceAllocator
    {
    public:
        virtual bool AllocateTexture(const TextureCreateInfo& createInfo);
        
    };
}