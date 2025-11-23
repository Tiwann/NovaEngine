#pragma once
#include "Containers/StringView.h"
#include "Rendering/Texture.h"
#include "Runtime/Ref.h"

namespace Nova
{
   Ref<Texture> LoadTexture(Ref<Device>& device, StringView filepath);
   Ref<Texture> LoadTexture(Ref<Device>& device, const void* data, size_t dataSize);
}
