#pragma once
#include "Containers/StringView.h"
#include "Rendering/Texture.h"
#include "Runtime/Ref.h"

namespace Nova::TextureUtils
{
   Ref<Texture> LoadTexture(Ref<RenderDevice>& device, StringView filepath);
   Ref<Texture> LoadTexture(Ref<RenderDevice>& device, const void* data, size_t dataSize);
   bool UploadTextureData(Ref<RenderDevice>& device, Ref<Texture>& texture, uint32_t arrayIndex, uint32_t mipLevel, const void* data, size_t dataSize);
}
