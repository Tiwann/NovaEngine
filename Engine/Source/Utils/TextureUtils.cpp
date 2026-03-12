#include "TextureUtils.h"
#include "External/stb_image.h"
#include "Rendering/Buffer.h"
#include "Rendering/CommandBuffer.h"
#include "Rendering/Fence.h"
#include "Rendering/Queue.h"
#include "Rendering/RenderDevice.h"
#include "Rendering/ResourceBarrier.h"

namespace Nova::TextureUtils
{
    bool UploadTextureData(Ref<RenderDevice>& device, Ref<ITexture>& texture, const uint32_t arrayLayer, const uint32_t mipLevel, const void* data, const size_t dataSize)
    {
        BufferCreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.size = dataSize;
        stagingBufferCreateInfo.usage = BufferUsage::StagingBuffer;

        Ref<Buffer> stagingBuffer = device->CreateBuffer(stagingBufferCreateInfo);
        if (!stagingBuffer) return false;
        if (!stagingBuffer->WriteData(data, 0, dataSize))
        {
            stagingBuffer->Destroy();
            return false;
        }

        const ResourceState initialState = texture->GetState();

        TextureBarrier toTransferBarrier;
        toTransferBarrier.texture = texture;
        toTransferBarrier.sourceAccess = GetSourceAccessFlags(initialState);
        toTransferBarrier.destAccess = GetDestAccessFlags(ResourceState::TransferDest);
        toTransferBarrier.destState = ResourceState::TransferDest;

        TextureBarrier toInitialState;
        toInitialState.texture = texture;
        toInitialState.sourceAccess = GetSourceAccessFlags(ResourceState::TransferDest);
        toInitialState.destAccess = GetDestAccessFlags(initialState);
        toInitialState.destState = initialState;

        Ref<CommandBuffer> cmdBuffer = device->CreateCommandBuffer();
        if (!cmdBuffer)
        {
            stagingBuffer->Destroy();
            return false;
        }

        if (!cmdBuffer->Begin({CommandBufferUsageFlagBits::OneTimeSubmit}))
        {
            stagingBuffer->Destroy();
            cmdBuffer->Free();
            return false;
        }

        cmdBuffer->TextureBarrier(toTransferBarrier);
        cmdBuffer->BufferToTextureCopy(*stagingBuffer, *texture, 0, dataSize, arrayLayer, mipLevel);
        cmdBuffer->TextureBarrier(toInitialState);

        cmdBuffer->End();

        const Queue* queue = device->GetGraphicsQueue();
        queue->Submit(cmdBuffer, nullptr, nullptr);
        stagingBuffer->Destroy();
        cmdBuffer->Free();
        return true;
    }

    Ref<ITexture> LoadTexture(Ref<RenderDevice>& device, const StringView filepath)
    {
        stbi_set_flip_vertically_on_load(true);
        int32_t width = 0, height = 0;
        stbi_uc* pixels = stbi_load(*filepath, &width, &height, nullptr, STBI_rgb_alpha);
        const size_t pixelsSize = width * height * 4 * sizeof(stbi_uc);
        if (!pixels) return nullptr;

        const TextureCreateInfo createInfo = TextureCreateInfo::Texture2D(width, height, Format::R8G8B8A8_SRGB, 1, 1);
        Ref<ITexture> texture = device->CreateTexture(createInfo);
        if (!texture)
        {
            stbi_image_free(pixels);
            return nullptr;
        }

        if (!UploadTextureData(device, texture, 0, 0, pixels, pixelsSize))
        {
            stbi_image_free(pixels);
            texture->Destroy();
            return nullptr;
        }

        stbi_image_free(pixels);
        return texture;
    }

    Ref<ITexture> LoadTexture(Ref<RenderDevice>& device, const void* data, const size_t dataSize)
    {
        stbi_set_flip_vertically_on_load(true);
        int32_t width = 0, height = 0;
        stbi_uc* pixels = stbi_load_from_memory((const uint8_t*)data, (int)dataSize, &width, &height, nullptr, STBI_rgb_alpha);
        const size_t pixelsSize = width * height * 4 * sizeof(stbi_uc);
        if (!pixels) return nullptr;

        const TextureCreateInfo createInfo = TextureCreateInfo::Texture2D(width, height, Format::R8G8B8A8_SRGB, 1, 1);
        Ref<ITexture> texture = device->CreateTexture(createInfo);
        if (!texture)
        {
            stbi_image_free(pixels);
            return nullptr;
        }

        if (!UploadTextureData(device, texture, 0, 0, pixels, pixelsSize))
        {
            stbi_image_free(pixels);
            texture->Destroy();
            return nullptr;
        }

        stbi_image_free(pixels);
        return texture;
    }


}
