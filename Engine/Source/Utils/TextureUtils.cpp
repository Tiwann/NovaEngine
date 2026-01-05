#include "TextureUtils.h"
#include "External/stb_image.h"
#include "Rendering/Device.h"

namespace Nova
{
    Ref<Texture> LoadTexture(Ref<Device>& device, const StringView filepath)
    {
        stbi_set_flip_vertically_on_load(true);
        int32_t width = 0, height = 0;
        stbi_uc* pixels = stbi_load(*filepath, &width, &height, nullptr, STBI_rgb_alpha);
        if (!pixels) return nullptr;

        const BufferView pixelsView(pixels, width * height * 4 * sizeof(stbi_uc));
        const TextureCreateInfo createInfo = TextureCreateInfo::Texture2D(width, height, Format::R8G8B8A8_SRGB, 1, 1, pixelsView);

        Ref<Texture> texture = device->CreateTexture(createInfo);
        if (!texture)
        {
            stbi_image_free(pixels);
            return nullptr;
        }

        stbi_image_free(pixels);
        return texture;
    }

    Ref<Texture> LoadTexture(Ref<Device>& device, const void* data, const size_t dataSize)
    {
        stbi_set_flip_vertically_on_load(true);
        int32_t width = 0, height = 0;
        stbi_uc* pixels = stbi_load_from_memory((const uint8_t*)data, (int)dataSize, &width, &height, nullptr, STBI_rgb_alpha);
        if (!pixels) return nullptr;

        const BufferView pixelsView(pixels, width * height * 4 * sizeof(stbi_uc));
        const TextureCreateInfo createInfo = TextureCreateInfo::Texture2D(width, height, Format::R8G8B8A8_SRGB, 1, 1, pixelsView);

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
