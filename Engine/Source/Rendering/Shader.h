#pragma once
#include "Containers/StringView.h"
#include "ShaderModuleInfo.h"
#include "ShaderEntryPoint.h"
#include "ShaderTarget.h"

#include <slang/slang.h>
#include <slang/slang-com-ptr.h>

#include "Runtime/Asset.h"


namespace Nova::Rendering
{
    class Device;
    class Texture;
    class Sampler;
    class Buffer;

    struct ShaderCreateInfo
    {
        Device* device = nullptr;
        Slang::ComPtr<slang::IGlobalSession> slang = nullptr;
        ShaderModuleInfo moduleInfo;
        ShaderTarget target = ShaderTarget::SPIRV;
        Array<StringView> includes;
        Array<ShaderEntryPoint> entryPoints;
    };

    class Shader : public Asset
    {
    public:
        Shader() = default;
        ~Shader() override = default;

        String GetAssetType() const override { return "Shader"; }

        virtual bool Initialize(const ShaderCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        virtual bool BindTexture(uint32_t set, uint32_t binding, const Texture& texture) = 0;
        virtual bool BindSampler(uint32_t set, uint32_t binding, const Sampler& sampler) = 0;
        virtual bool BindCombinedSamplerTexture(uint32_t set, uint32_t binding, const Sampler& sampler, const Texture& texture) = 0;
        virtual bool BindBuffer(uint32_t set, uint32_t binding, const Buffer& buffer, size_t offset, size_t size) = 0;
    };
}
