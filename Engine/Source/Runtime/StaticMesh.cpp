#include "StaticMesh.h"
#include "Application.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Rendering/RenderDevice.h"
#include "Rendering/Buffer.h"
#include "Rendering/Vertex.h"
#include "Utils/BufferUtils.h"
#include "Utils/TextureUtils.h"
#include "Rendering/Shader.h"
#include "Rendering/Material.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/GltfMaterial.h>

#include "Common.h"


namespace Nova
{
    StaticMesh::StaticMesh(const String& name) : Asset(name)
    {
    }

    StaticMesh::~StaticMesh()
    {
        if (m_VertexBuffer) m_VertexBuffer->Destroy();
        if (m_IndexBuffer) m_IndexBuffer->Destroy();
    }

    AssetType StaticMesh::GetAssetType() const
    {
        return AssetType::StaticMesh;
    }

    enum class ModelFormat
    {
        None = -1,
        FBX,
        GLTF,
        GLB,
        OBJ,
        DAE
    };

    static Array<uint32_t> GetIndicesFromFaces(const BufferView<aiFace>& faces)
    {
        Array<uint32_t> result;
        for (uint32_t faceIndex = 0; faceIndex < faces.Count(); ++faceIndex)
        {
            const aiFace& face = faces[faceIndex];
            result.AddRange(face.mIndices, face.mNumIndices);
        }
        return result;
    }

    static Array<Vertex> GetVerticesFromMesh(const aiMesh& mesh)
    {
        Array<Vertex> result;

        const auto toVector3 = [](const aiVector3D& in) { return Vector3(in.x, in.y, in.z); };
        const auto toVector2 = [](const aiVector3D& in) { return Vector2(in.x, in.y); };
        const auto toVector4 = [](const aiColor4D& in) { return Vector4(in.r, in.g, in.b, in.a); };

        for (uint32_t vertexIndex = 0; vertexIndex < mesh.mNumVertices; ++vertexIndex)
        {
            const aiVector3D& position = mesh.HasPositions() ? mesh.mVertices[vertexIndex] : aiVector3D(0, 0, 0);
            const aiVector3D& texCoord = mesh.HasTextureCoords(0) ? mesh.mTextureCoords[0][vertexIndex] : aiVector3D(0, 0, 0);
            const aiVector3D& normal = mesh.HasNormals() ? mesh.mNormals[vertexIndex] : aiVector3D(0, 0, 0);
            const aiVector3D& tangent = mesh.HasTangentsAndBitangents() ? mesh.mTangents[vertexIndex] : aiVector3D(0, 0, 0);
            const aiColor4D& color = mesh.HasVertexColors(0) ? mesh.mColors[0][vertexIndex] : aiColor4D(0, 0, 0, 0);

            const Vertex vertex
            {
                .position = toVector3(position),
                .texCoords = toVector2(texCoord),
                .normal = toVector3(normal),
                .tangent = toVector3(tangent),
                .color = toVector4(color)
            };

            result.Add(vertex);
        }

        return result;
    }
    
    bool StaticMesh::LoadFromFile(const StringView filepath, bool loadResources)
    {
        Assimp::Importer importer;
        NOVA_DEFER(importer, &Assimp::Importer::FreeScene);
        constexpr auto flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals
        | aiProcess_JoinIdenticalVertices | aiProcess_EmbedTextures | aiProcess_PreTransformVertices;

        const aiScene* loadedScene = importer.ReadFile(*filepath, flags);
        if (!loadedScene) return false;
        if (!loadedScene->HasMeshes()) return false;

        Ref<RenderDevice>& device = Application::GetCurrentApplication().GetRenderDevice();

        Array<Vertex> allVertices;
        Array<uint32_t> allIndices;
        size_t vertexOffset = 0;
        size_t indexOffset = 0;

        for (size_t meshIndex = 0; meshIndex < loadedScene->mNumMeshes; meshIndex++)
        {
            const aiMesh* loadedMesh = loadedScene->mMeshes[meshIndex];
            const uint32_t materialIndex = loadedMesh->mMaterialIndex;
            const aiString materialSlotName = loadedScene->mMaterials[materialIndex]->GetName();
            MaterialInfo& materialInfo = MaterialSlotExists(materialIndex) ? m_MaterialInfos[materialIndex] : CreateMaterialSlot(String{materialSlotName.C_Str()}, materialIndex);

            Array<uint32_t> indices = GetIndicesFromFaces(BufferView(loadedMesh->mFaces, loadedMesh->mNumFaces));
            Array<Vertex> vertices = GetVerticesFromMesh(*loadedMesh);

            allVertices.AddRange(vertices);
            allIndices.AddRange(indices);

            SubMeshInfo subMeshInfo { };
            subMeshInfo.vertexBufferSize = vertices.Size();
            subMeshInfo.vertexBufferOffset = vertexOffset;
            subMeshInfo.indexBufferSize = indices.Size();
            subMeshInfo.indexBufferOffset = indexOffset;
            materialInfo.subMeshes.Add(subMeshInfo);

            vertexOffset += subMeshInfo.vertexBufferSize;
            indexOffset += subMeshInfo.indexBufferSize;
        }


        if (m_VertexBuffer) m_VertexBuffer->Destroy();
        m_VertexBuffer = BufferUtils::CreateVertexBuffer(device, allVertices.Data(), allVertices.Size());
        if (m_IndexBuffer) m_IndexBuffer->Destroy();
        m_IndexBuffer = BufferUtils::CreateIndexBuffer(device, allIndices.Data(), allIndices.Size());
        return true;
    }

    void StaticMesh::SetMaterial(const uint32_t slot, Ref<Material> material)
    {
        const auto predicate = [&slot](const MaterialInfo& info) { return info.slot == slot; };
        MaterialInfo* info = m_MaterialInfos.Single(predicate);
        if (info)
            info->material = material;
    }

    Ref<Material> StaticMesh::GetMaterial(const uint32_t slot)
    {
        const auto predicate = [&slot](const MaterialInfo& info) { return info.slot == slot; };
        MaterialInfo* info = m_MaterialInfos.Single(predicate);
        return info ? info->material : nullptr;
    }

    const Array<MaterialInfo>& StaticMesh::GetMaterialInfos() const
    {
        return m_MaterialInfos;
    }

    Ref<Buffer> StaticMesh::GetVertexBuffer() const
    {
        return m_VertexBuffer;
    }

    Ref<Buffer> StaticMesh::GetIndexBuffer() const
    {
        return m_IndexBuffer;
    }

    bool StaticMesh::MaterialSlotExists(uint32_t slot) const
    {
        const auto* info = m_MaterialInfos.Single([&slot](const MaterialInfo& info) { return info.slot == slot;});
        return info;
    }

    MaterialInfo& StaticMesh::CreateMaterialSlot(const String& name, uint32_t slot)
    {
        m_MaterialInfos.Emplace({ name, slot });
        return m_MaterialInfos.Last();
    }
}
