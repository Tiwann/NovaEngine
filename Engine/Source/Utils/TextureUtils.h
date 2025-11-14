#pragma once
#include "Containers/StringView.h"
#include "Rendering/Texture.h"
#include "Runtime/Ref.h"

namespace Nova
{
   Ref<Rendering::Texture> LoadTexture(Ref<Rendering::Device>& device, StringView filepath);
   Ref<Rendering::Texture> LoadTexture(Ref<Rendering::Device>& device, const void* data, size_t dataSize);
}
