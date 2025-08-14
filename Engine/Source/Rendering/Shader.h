#pragma once
#include "Containers/StringView.h"
#include "ShaderModuleInfo.h"
#include "ShaderEntryPoint.h"
#include "ShaderTarget.h"
#include "Runtime/Asset.h"
#include "Runtime/Ref.h"

#include <slang/slang.h>
#include <slang/slang-com-ptr.h>


namespace Nova::Rendering
{
    class Device;
    class Texture;
    class Sampler;
    class Buffer;
    class ShaderBindingSet;

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

        virtual Ref<ShaderBindingSet> CreateBindingSet(size_t setIndex) const = 0;
    };
}
