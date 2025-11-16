#include "StaticMesh.h"
#include "Application.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Rendering/Device.h"
#include "Rendering/Buffer.h"
#include "Rendering/Vertex.h"
#include "Utils/BufferUtils.h"
#include "Utils/TextureUtils.h"
#include "Rendering/Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace Nova
{
    StaticMesh::StaticMesh(const String& name) : Asset(name)
    {
    }

    StaticMesh::~StaticMesh()
    {
        if (m_VertexBuffer)
        {
            m_VertexBuffer->Destroy();
        }

        if (m_IndexBuffer)
        {
            m_IndexBuffer->Destroy();
        }
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
    
    bool StaticMesh::LoadFromFile(const StringView filepath, bool loadRessources)
    {
        Assimp::Importer importer;
        constexpr auto flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_EmbedTextures;
        const aiScene* loadedScene = importer.ReadFile(*filepath, flags);
        if (!loadedScene) return false;
        if (!loadedScene->HasMeshes()) return false;

        Ref<Rendering::Device> device = Application::GetCurrentApplication().GetDevice();

        Array<Vertex> allVertices;
        Array<uint32_t> allIndices;
        size_t vertexOffset = 0;
        size_t indexOffset = 0;

        for (size_t meshIndex = 0; meshIndex < loadedScene->mNumMeshes; meshIndex++)
        {
            const aiMesh* loadedMesh = loadedScene->mMeshes[meshIndex];
            uint32_t materialIndex = loadedMesh->mMaterialIndex;
            const aiString materialSlotName = loadedScene->mMaterials[materialIndex]->GetName();
            MaterialInfo& materialInfo = MaterialSlotExists(materialIndex) ? m_MaterialInfos[materialIndex] : CreateMaterialSlot(String{materialSlotName.C_Str()}, materialIndex);

            Array<uint32_t> indices;
            for (uint32_t faceIndex = 0; faceIndex < loadedMesh->mNumFaces; ++faceIndex)
            {
                const aiFace& face = loadedMesh->mFaces[faceIndex];
                for (uint32_t i = 0; i < face.mNumIndices; ++i)
                    indices.Add(face.mIndices[i]);
            }


            const auto toVector3 = [](const aiVector3D& in) { return Vector3(in.x, in.y, in.z); };
            const auto toVector2 = [](const aiVector3D& in) { return Vector2(in.x, in.y); };
            const auto toVector4 = [](const aiColor4D& in) { return Vector4(in.r, in.g, in.b, in.a); };

            Array<Vertex> vertices;
            for (uint32_t vertexIndex = 0; vertexIndex < loadedMesh->mNumVertices; ++vertexIndex)
            {
                const aiVector3D& position = loadedMesh->HasPositions() ? loadedMesh->mVertices[vertexIndex] : aiVector3D(0, 0, 0);
                const aiVector3D& texCoord = loadedMesh->HasTextureCoords(0) ? loadedMesh->mTextureCoords[0][vertexIndex] : aiVector3D(0, 0, 0);
                const aiVector3D& normal = loadedMesh->HasNormals() ? loadedMesh->mNormals[vertexIndex] : aiVector3D(0, 0, 0);
                const aiVector3D& tangent = loadedMesh->HasTangentsAndBitangents() ? loadedMesh->mTangents[vertexIndex] : aiVector3D(0, 0, 0);
                const aiColor4D& color = loadedMesh->HasVertexColors(0) ? loadedMesh->mColors[0][vertexIndex] : aiColor4D(0, 0, 0, 0);

                const Vertex vertex
                {
                    .position = toVector3(position),
                    .texCoords = toVector2(texCoord),
                    .normal = toVector3(normal),
                    .tangent = toVector3(tangent),
                    .color = toVector4(color)
                };
                vertices.Add(vertex);
            }

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

        if (loadRessources && !m_MaterialInfos.IsEmpty())
        {
            const AssetDatabase& assetDatabase = Application::GetCurrentApplication().GetAssetDatabase();
            const Ref<Rendering::Shader> blinnPhongShader = assetDatabase.Get<Rendering::Shader>("BlinnPhongShader");

            for (uint32_t i = 0; i < m_MaterialInfos.Count(); ++i)
            {
                const aiMaterial* loadedMaterial = loadedScene->mMaterials[i];

                if (loadedMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
                {
                    aiString path;
                    if (loadedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
                    {
                        const aiTexture* loadedTexture = loadedScene->GetEmbeddedTexture(path.data);

                        Ref<Rendering::Texture> texture = LoadTexture(device, loadedTexture->pcData, loadedTexture->mWidth);
                        m_MaterialInfos[i].textures.Add(texture);

                        Ref<Rendering::Material> material = device->CreateMaterial({device, blinnPhongShader});
                        material->SetTexture("albedoTex", texture);
                        m_MaterialInfos[i].material = material;
                    }
                }
            }
        }

        if (m_VertexBuffer) m_VertexBuffer->Destroy();
        m_VertexBuffer = CreateVertexBuffer(device, allVertices.Data(), allVertices.Size());
        if (m_IndexBuffer) m_IndexBuffer->Destroy();
        m_IndexBuffer = CreateIndexBuffer(device, allIndices.Data(), allIndices.Size());
        importer.FreeScene();
        return true;
    }

    void StaticMesh::SetMaterial(const uint32_t slot, Ref<Rendering::Material> material)
    {
        MaterialInfo* info = m_MaterialInfos.Single([&slot](const MaterialInfo& info)
        {
            return info.slot == slot;
        });

        if (info)
        {
            info->material = material;
        }
    }

    Ref<Rendering::Material> StaticMesh::GetMaterial(const uint32_t slot)
    {
        MaterialInfo* info = m_MaterialInfos.Single([&slot](const MaterialInfo& info)
        {
            return info.slot == slot;
        });
        return info ? info->material : nullptr;
    }

    const Array<MaterialInfo>& StaticMesh::GetMaterialInfos() const
    {
        return m_MaterialInfos;
    }

    Ref<Rendering::Buffer> StaticMesh::GetVertexBuffer() const
    {
        return m_VertexBuffer;
    }

    Ref<Rendering::Buffer> StaticMesh::GetIndexBuffer() const
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
