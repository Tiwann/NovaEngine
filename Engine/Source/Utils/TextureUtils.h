#pragma once
#include "Containers/StringView.h"
#include "Rendering/ITexture.h"
#include "Runtime/Ref.h"

namespace Nova::TextureUtils
{
   Ref<ITexture> LoadTexture(Ref<RenderDevice>& device, StringView filepath);
   Ref<ITexture> LoadTexture(Ref<RenderDevice>& device, const void* data, size_t dataSize);
   bool UploadTextureData(Ref<RenderDevice>& device, Ref<ITexture>& texture, uint32_t arrayLayer, uint32_t mipLevel, const void* data, size_t dataSize);
}
