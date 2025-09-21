#pragma once
#include "Asset.h"
#include "Containers/Array.h"
#include "Containers/StringView.h"
#include "Runtime/Ref.h"

namespace Nova
{
    namespace Rendering
    {
        class Buffer;
    }

    struct MaterialSlot
    {
        String name;
        size_t index;
    };

    struct MaterialSlotBinding
    {
        MaterialSlot* slot = nullptr;
        class Material* material = nullptr;
    };

    struct SubMeshInfo
    {
        String name = "Empty Mesh";
        size_t vertexBufferOffset = 0;
        size_t vertexBufferSize = 0;
        size_t indexBufferOffset = 0;
        size_t indexBufferSize = 0;
        Array<MaterialSlotBinding> materialSlotBindings;
    };


    class StaticMesh final : public Asset
    {
    public:
        StaticMesh() = default;
        explicit StaticMesh(const String& name);
        ~StaticMesh() override;

        String GetAssetType() const override;
        bool LoadFromFile(StringView filepath);

        const Array<SubMeshInfo>& GetSubMeshes() const;
        Ref<Rendering::Buffer> GetVertexBuffer() const;
        Ref<Rendering::Buffer> GetIndexBuffer() const;

        void BindMaterial(size_t subMeshIndex, size_t slotIndex, Material* material);
    private:
        bool LoadFromFileAssimp(StringView filepath);
    private:
        Array<SubMeshInfo> m_SubMeshes;
        Ref<Rendering::Buffer> m_VertexBuffer = nullptr;
        Ref<Rendering::Buffer> m_IndexBuffer = nullptr;
        Array<MaterialSlot> m_MaterialSlots;
    };
    
}
