#include "TextureUtils.h"
#include "BufferUtils.h"
#include "External/stb_image.h"
#include "Rendering/Buffer.h"
#include "Rendering/CommandBuffer.h"
#include "Rendering/Fence.h"
#include "Rendering/Queue.h"
#include "Rendering/RenderDevice.h"
#include "Rendering/ResourceBarrier.h"
#include "Runtime/Common.h"

namespace Nova::TextureUtils
{
    bool UploadTextureData(Ref<RenderDevice>& device, Ref<Texture>& texture, const uint32_t arrayIndex, const uint32_t mipLevel, const void* data, const size_t dataSize)
    {
        Ref<Buffer> stagingBuffer = BufferUtils::CreateStagingBuffer(device, data, dataSize);
        if (!stagingBuffer) return false;
        NOVA_DEFER(stagingBuffer, &Buffer::Destroy);

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
        if (!cmdBuffer) return false;
        NOVA_DEFER(cmdBuffer, &CommandBuffer::Free);

        Ref<Fence> fence = device->CreateFence();
        if (!fence) return false;
        NOVA_DEFER(fence, &Fence::Destroy);

        cmdBuffer->Begin({CommandBufferUsageFlagBits::OneTimeSubmit});
        cmdBuffer->TextureBarrier(toTransferBarrier);
        cmdBuffer->CopyBufferToTexture(*stagingBuffer, *texture, 0, dataSize, arrayIndex, mipLevel);
        cmdBuffer->TextureBarrier(toInitialState);
        cmdBuffer->End();

        const Queue* queue = device->GetGraphicsQueue();
        queue->Submit(cmdBuffer, nullptr, nullptr, fence);
        fence->Wait(FENCE_WAIT_INFINITE);
        return true;
    }

    Ref<Texture> LoadTexture(Ref<RenderDevice>& device, const StringView filepath)
    {
        stbi_set_flip_vertically_on_load(true);
        int32_t width = 0, height = 0;
        stbi_uc* pixels = stbi_load(*filepath, &width, &height, nullptr, STBI_rgb_alpha);
        const size_t pixelsSize = width * height * 4 * sizeof(stbi_uc);
        if (!pixels) return nullptr;
        NOVA_DEFER_FUNC([&pixels]{ stbi_image_free(pixels); });

        const TextureCreateInfo createInfo = TextureCreateInfo::Texture2D(width, height, Format::R8G8B8A8_SRGB, 1, 1);
        Ref<Texture> texture = device->CreateTexture(createInfo);
        if (!texture) return nullptr;

        if (!UploadTextureData(device, texture, 0, 0, pixels, pixelsSize))
        {
            texture->Destroy();
            return nullptr;
        }
        return texture;
    }

    Ref<Texture> LoadTexture(Ref<RenderDevice>& device, const void* data, const size_t dataSize)
    {
        stbi_set_flip_vertically_on_load(true);
        int32_t width = 0, height = 0;
        stbi_uc* pixels = stbi_load_from_memory((const uint8_t*)data, (int)dataSize, &width, &height, nullptr, STBI_rgb_alpha);
        const size_t pixelsSize = width * height * 4 * sizeof(stbi_uc);
        if (!pixels) return nullptr;

        const TextureCreateInfo createInfo = TextureCreateInfo::Texture2D(width, height, Format::R8G8B8A8_SRGB, 1, 1);
        Ref<Texture> texture = device->CreateTexture(createInfo);
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
