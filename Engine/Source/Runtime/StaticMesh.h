#pragma once
#include "Asset.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Containers/StringView.h"
#include "Rendering/Texture.h"
#include "Runtime/Ref.h"

namespace Nova
{
    namespace Rendering
    {
        class Buffer;
    }

    struct SubMeshInfo
    {
        size_t vertexBufferOffset = 0;
        size_t vertexBufferSize = 0;
        size_t indexBufferOffset = 0;
        size_t indexBufferSize = 0;
    };

    struct MaterialInfo
    {
        Array<SubMeshInfo> subMeshes;
        Ref<Rendering::Texture> texture = nullptr;
    };

    class StaticMesh final : public Asset
    {
    public:
        StaticMesh() = default;
        explicit StaticMesh(const String& name);
        ~StaticMesh() override;

        AssetType GetAssetType() const override;
        bool LoadFromFile(StringView filepath);

        const Map<uint32_t, MaterialInfo>& GetMaterialInfos() const;
        Ref<Rendering::Buffer> GetVertexBuffer() const;
        Ref<Rendering::Buffer> GetIndexBuffer() const;
    private:
        bool LoadFromFileAssimp(StringView filepath);
    private:
        Map<uint32_t, MaterialInfo> m_MaterialInfos;
        Ref<Rendering::Buffer> m_VertexBuffer = nullptr;
        Ref<Rendering::Buffer> m_IndexBuffer = nullptr;
    };
    
}
