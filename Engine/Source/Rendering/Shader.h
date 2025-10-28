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

        ShaderCreateInfo& withDevice(Device* device) { this->device = device; return *this; }
        ShaderCreateInfo& withModuleInfo(const ShaderModuleInfo& moduleInfo) { this->moduleInfo = moduleInfo; return *this; }
        ShaderCreateInfo& withTarget(const ShaderTarget& target) { this->target = target; return *this; }
        ShaderCreateInfo& withIncludes(const Array<StringView>& includes) { this->includes = includes; return *this; }
        ShaderCreateInfo& withIncludesAdded(const Array<StringView>& includes) { this->includes.AddRange(includes); return *this; }
        ShaderCreateInfo& withEntryPoints(const Array<ShaderEntryPoint>& entryPoints) { this->entryPoints = entryPoints; return *this; }
        ShaderCreateInfo& withEntryPointsAdded(const Array<ShaderEntryPoint>& entryPoints) { this->entryPoints.AddRange(entryPoints); return *this; }
    };

    class Shader : public Asset
    {
    public:
        Shader() = default;
        ~Shader() override = default;

        String GetAssetType() const override { return "Shader"; }

        virtual bool Initialize(const ShaderCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        virtual Ref<ShaderBindingSet> CreateBindingSet(size_t setIndex = 0) const = 0;
        virtual Array<Ref<ShaderBindingSet>> CreateBindingSets() const = 0;
    };
}
