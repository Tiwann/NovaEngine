#pragma once
#include "Containers/StringView.h"
#include "ShaderModuleInfo.h"
#include "ShaderEntryPoint.h"
#include "ShaderTarget.h"
#include "Runtime/Asset.h"
#include "Runtime/Ref.h"

namespace slang { class IGlobalSession; }

namespace Nova
{
    class RenderDevice;
    class Texture;
    class Sampler;
    class Buffer;
    class ShaderBindingSet;

    struct ShaderCreateInfo
    {
        RenderDevice* device = nullptr;
        ShaderModuleInfo moduleInfo;
        ShaderTarget target = ShaderTarget::SPIRV;

        Array<StringView> includes;
        Array<ShaderEntryPoint> entryPoints;

        ShaderCreateInfo& WithDevice(RenderDevice* inDevice) { this->device = inDevice; return *this; }
        ShaderCreateInfo& WithModuleInfo(const ShaderModuleInfo& inModuleInfo) { this->moduleInfo = inModuleInfo; return *this; }
        ShaderCreateInfo& WithTarget(const ShaderTarget& target) { this->target = target; return *this; }
        ShaderCreateInfo& WithIncludes(const Array<StringView>& includes) { this->includes = includes; return *this; }
        ShaderCreateInfo& WithIncludesAdded(const Array<StringView>& includes) { this->includes.AddRange(includes); return *this; }
        ShaderCreateInfo& WithEntryPoints(const Array<ShaderEntryPoint>& entryPoints) { this->entryPoints = entryPoints; return *this; }
        ShaderCreateInfo& WithEntryPointsAdded(const Array<ShaderEntryPoint>& entryPoints) { this->entryPoints.AddRange(entryPoints); return *this; }
        ShaderCreateInfo& AddEntryPoint(const ShaderEntryPoint& entryPoint) { this->entryPoints.Add(entryPoint); return *this; }
        ShaderCreateInfo& AddEntryPoint(const String& name, ShaderStageFlagBits stage) { this->entryPoints.Add({name, stage}); return *this; }
    };

    class Shader : public Asset
    {
    public:
        Shader() = default;
        ~Shader() override = default;

        AssetType GetAssetType() const final { return AssetType::Shader; }

        virtual bool Initialize(const ShaderCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        virtual Ref<ShaderBindingSet> CreateBindingSet(size_t setIndex = 0) const = 0;
        virtual Array<Ref<ShaderBindingSet>> CreateBindingSets() const = 0;
    };
}
