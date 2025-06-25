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
    
    struct SubMesh
    {
        String Name = "Empty Mesh";
        size_t VertexBufferOffset = 0;
        size_t VertexBufferSize = 0;
        size_t IndexBufferOffset = 0;
        size_t IndexBufferSize = 0;
    };

    struct MaterialSlot
    {
        String Name;
        u32 Index;
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
    private:
        bool LoadFromFileAssimp(const Path& Filepath);
    private:
        Array<SubMesh> m_SubMeshes;
        VertexBuffer* m_VertexBuffer = nullptr;
        IndexBuffer* m_IndexBuffer = nullptr;
        Array<MaterialSlot> m_MaterialSlots;
    };
    
}
