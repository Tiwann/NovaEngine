#include "TextureUtils.h"
#include "External/stb_image.h"
#include "Rendering/Device.h"

namespace Nova
{
    Ref<Texture> LoadTexture(Ref<Device>& device, StringView filepath)
    {
        stbi_set_flip_vertically_on_load(true);
        int32_t width = 0, height = 0;
        stbi_uc* pixels = stbi_load(*filepath, &width, &height, nullptr, STBI_rgb_alpha);
        if (!pixels) return nullptr;

        TextureCreateInfo createInfo;
        createInfo.device = device;
        createInfo.width = width;
        createInfo.height = height;
        createInfo.depth = 1;
        createInfo.format = Format::R8G8B8A8_UNORM;
        createInfo.mips = 1;
        createInfo.sampleCount = 1;
        createInfo.usageFlags = TextureUsageFlagBits::Sampled;
        createInfo.data = pixels;
        createInfo.dataSize = width * height * 4 * sizeof(stbi_uc);

        Ref<Texture> texture = device->CreateTexture(createInfo);
        if (!texture)
        {
            stbi_image_free(pixels);
            return nullptr;
        }

        stbi_image_free(pixels);
        return texture;
    }

    Ref<Texture> LoadTexture(Ref<Device>& device, const void* data, size_t dataSize)
    {
        stbi_set_flip_vertically_on_load(true);
        int32_t width = 0, height = 0;
        stbi_uc* pixels = stbi_load_from_memory((const uint8_t*)data, (int)dataSize, &width, &height, nullptr, STBI_rgb_alpha);
        if (!pixels) return nullptr;

        TextureCreateInfo createInfo;
        createInfo.device = device;
        createInfo.width = width;
        createInfo.height = height;
        createInfo.depth = 1;
        createInfo.format = Format::R8G8B8A8_SRGB;
        createInfo.mips = 1;
        createInfo.sampleCount = 1;
        createInfo.usageFlags = TextureUsageFlagBits::Sampled;
        createInfo.data = pixels;
        createInfo.dataSize = width * height * 4 * sizeof(stbi_uc);

        Ref<Texture> texture = device->CreateTexture(createInfo);
        if (!texture)
        {
            stbi_image_free(pixels);
            return nullptr;
        }

        stbi_image_free(pixels);
        return texture;
    }
}
