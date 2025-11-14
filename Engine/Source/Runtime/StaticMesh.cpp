#include "StaticMesh.h"
#include "Application.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Rendering/Device.h"
#include "Rendering/Buffer.h"
#include "Rendering/Vertex.h"
#include "Utils/BufferUtils.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Utils/TextureUtils.h"


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

    /*static bool HasFBXExtension(StringView filepath)
    {
        return filepath.EndsWith(".fbx") || filepath.EndsWith(".FBX");
    }

    static bool HasGLTFExtension(StringView filepath)
    {
        return filepath.EndsWith(".gltf") || filepath.EndsWith(".GLTF");
    }

    static bool HasGLBExtension(StringView filepath)
    {
        return filepath.EndsWith(".glb") || filepath.EndsWith(".GLB");
    }

    static bool HasOBJExtension(StringView filepath)
    {
        return filepath.EndsWith(".obj") || filepath.EndsWith(".OBJ");
    }

    static bool HasDAEExtension(StringView filepath)
    {
        return filepath.EndsWith(".dae") || filepath.EndsWith(".DAE");
    }*/

    /*static ModelFormat GetModelFormat(StringView filepath)
    {
        if (HasGLTFExtension(filepath)) return ModelFormat::GLTF;
        if (HasFBXExtension(filepath)) return ModelFormat::FBX;
        if (HasGLBExtension(filepath)) return ModelFormat::GLB;
        if (HasOBJExtension(filepath)) return ModelFormat::OBJ;
        if (HasDAEExtension(filepath)) return ModelFormat::DAE;
        return ModelFormat::None;
    }*/
    
    bool StaticMesh::LoadFromFile(const StringView filepath)
    {
        /*switch (GetModelFormat(filepath))
        {
        case ModelFormat::FBX:  return LoadFromFileAssimp(filepath);
        case ModelFormat::GLTF: return LoadFromFileAssimp(filepath);
        case ModelFormat::GLB:  return LoadFromFileAssimp(filepath);
        case ModelFormat::OBJ:  return LoadFromFileAssimp(filepath);
        case ModelFormat::DAE:  return LoadFromFileAssimp(filepath);
        case ModelFormat::None: return false;
        }
        return false;*/
        return LoadFromFileAssimp(filepath);
    }

    const Map<uint32_t, MaterialInfo>& StaticMesh::GetMaterialInfos() const
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

    bool StaticMesh::LoadFromFileAssimp(StringView filepath)
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
            MaterialInfo& materialInfo = m_MaterialInfos[materialIndex];

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


        if (loadedScene->HasMaterials())
        {
            for (unsigned int i = 0; i < loadedScene->mNumMaterials && i < m_MaterialInfos.Count(); ++i)
            {
                aiMaterial* material = loadedScene->mMaterials[i];
                aiString name;
                material->Get(AI_MATKEY_NAME, name);

                if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
                {
                    aiString path;
                    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
                    {
                        const aiTexture* texture = loadedScene->GetEmbeddedTexture(path.data);

                        if (texture->mHeight == 0)
                        {
                            m_MaterialInfos[i].texture = LoadTexture(device, texture->pcData, texture->mWidth);
                        }
                    }
                }
            }
        }


        m_VertexBuffer = CreateVertexBuffer(device, allVertices.Data(), allVertices.Size());
        m_IndexBuffer = CreateIndexBuffer(device, allIndices.Data(), allIndices.Size());
        importer.FreeScene();
        return true;
    }
}
