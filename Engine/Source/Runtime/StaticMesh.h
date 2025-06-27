#pragma once
#include "Asset.h"
#include "Filesystem.h"
#include "Containers/Array.h"


namespace tinygltf
{
    class Model;
}
namespace Nova
{
    class VertexArray;
    class VertexBuffer;
    class IndexBuffer;

    struct MaterialSlot
    {
        String Name;
        size_t Index;
    };

    struct MaterialSlotBinding
    {
        MaterialSlot* Slot = nullptr;
        class Material* Material = nullptr;
    };

    struct SubMesh
    {
        String Name = "Empty Mesh";
        size_t VertexBufferOffset = 0;
        size_t VertexBufferSize = 0;
        size_t IndexBufferOffset = 0;
        size_t IndexBufferSize = 0;
        Array<MaterialSlotBinding> MaterialSlotBindings;
    };


    class StaticMesh : public Asset
    {
    public:
        StaticMesh() = default;
        explicit StaticMesh(const String& Name);
        ~StaticMesh() override;

        String GetAssetType() const override;
        bool LoadFromFile(const Path& Filepath);

        const Array<SubMesh>& GetSubMeshes() const;
        VertexBuffer* GetVertexBuffer() const;
        IndexBuffer* GetIndexBuffer() const;

        void BindMaterial(size_t SubMeshIndex, size_t SlotIndex, Material* Material);
    private:
        bool LoadFromFileAssimp(const Path& Filepath);
    private:
        Array<SubMesh> m_SubMeshes;
        VertexBuffer* m_VertexBuffer = nullptr;
        IndexBuffer* m_IndexBuffer = nullptr;
        Array<MaterialSlot> m_MaterialSlots;
    };
    
}
