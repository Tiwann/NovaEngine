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


    String StaticMesh::GetAssetType() const
    {
        return "Model";
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

    const Array<SubMeshInfo>& StaticMesh::GetSubMeshes() const
    {
        return m_SubMeshes;
    }

    Ref<Rendering::Buffer> StaticMesh::GetVertexBuffer() const
    {
        return m_VertexBuffer;
    }

    Ref<Rendering::Buffer> StaticMesh::GetIndexBuffer() const
    {
        return m_IndexBuffer;
    }

    void StaticMesh::BindMaterial(size_t subMeshIndex, size_t slotIndex, Material* material)
    {
        //m_SubMeshes[SubMeshIndex].MaterialSlotBindings.Single([](const MaterialSlotBinding& Binding))
    }

    bool StaticMesh::LoadFromFileAssimp(StringView filepath)
    {
        Assimp::Importer importer;
        constexpr auto flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices;
        const aiScene* loadedScene = importer.ReadFile(*filepath, flags);
        if (!loadedScene) return false;

        Array<Vertex> allVerts;
        Array<uint32_t> allIndices;

        /*for (size_t materialSlotIndex = 0; materialSlotIndex < loadedScene->mNumMaterials; materialSlotIndex++)
        {
            const aiMaterial* material = loadedScene->mMaterials[materialSlotIndex];
            MaterialSlot slot;
            slot.name = material->GetName().C_Str();
            uint32_t NumAllocated = material->mNumAllocated;
            uint32_t NumProperties = material->mNumProperties;
        }*/

        for (size_t meshIndex = 0; meshIndex < loadedScene->mNumMeshes; meshIndex++)
        {
            const aiMesh* loadedMesh = loadedScene->mMeshes[meshIndex];

            Array<uint32_t> indices;
            for (uint32_t faceIndex = 0; faceIndex < loadedMesh->mNumFaces; ++faceIndex)
            {
                const aiFace& face = loadedMesh->mFaces[faceIndex];
                for (uint32_t i = 0; i < face.mNumIndices; ++i)
                    indices.Add(face.mIndices[i]);
            }

            Array<Vertex> vertices;
            for (uint32_t vertexIndex = 0; vertexIndex < loadedMesh->mNumVertices; ++vertexIndex)
            {
                const aiVector3D& position = loadedMesh->HasPositions() ? loadedMesh->mVertices[vertexIndex] : aiVector3D(0, 0, 0);
                const aiVector3D& texCoord = loadedMesh->HasTextureCoords(0) ? loadedMesh->mTextureCoords[0][vertexIndex] : aiVector3D(0, 0, 0);
                const aiVector3D& normal = loadedMesh->HasNormals() ? loadedMesh->mNormals[vertexIndex] : aiVector3D(0, 0, 0);
                const aiColor4D& color = loadedMesh->HasVertexColors(0) ? loadedMesh->mColors[0][vertexIndex] : aiColor4D(0, 0, 0, 0);

                Vector3(* const toVector3)(const aiVector3D&) = [](const aiVector3D& in) { return Vector3(in.x, in.y, in.z); };
                Vector2(* const toVector2)(const aiVector3D&) = [](const aiVector3D& in) { return Vector2(in.x, in.y); };
                Vector4(* const toVector4)(const aiColor4D&) = [](const aiColor4D& in) { return Vector4(in.r, in.g, in.b, in.a); };


                const Vertex vertex
                {
                    .position = toVector3(position),
                    .texCoords = toVector2(texCoord),
                    .normal = toVector3(normal),
                    .color = toVector4(color)
                };
                vertices.Add(vertex);
            }

            allVerts.AddRange(vertices);
            allIndices.AddRange(indices);

            SubMeshInfo subMeshInfo { };
            subMeshInfo.name = String(loadedMesh->mName.C_Str());
            subMeshInfo.vertexBufferSize = vertices.Size();
            subMeshInfo.vertexBufferOffset = meshIndex == 0 ? 0 : m_SubMeshes[meshIndex - 1].vertexBufferOffset + m_SubMeshes[meshIndex - 1].vertexBufferSize;
            subMeshInfo.indexBufferSize = indices.Count() * sizeof(uint32_t);
            subMeshInfo.indexBufferOffset = meshIndex == 0 ? 0 : m_SubMeshes[meshIndex - 1].indexBufferOffset + m_SubMeshes[meshIndex - 1].indexBufferSize;
            m_SubMeshes.Add(subMeshInfo);
        }


        Application& application = Application::GetCurrentApplication();
        Ref<Rendering::Device>& device = application.GetDevice();
        m_VertexBuffer = CreateVertexBuffer(device, allVerts.Data(), allVerts.Size());
        m_IndexBuffer = CreateIndexBuffer(device, allIndices.Data(), allIndices.Size());
        importer.FreeScene();
        return true;
    }
}
