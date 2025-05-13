#include "StaticMesh.h"
#include "Application.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Rendering/Renderer.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace Nova
{
    StaticMesh::StaticMesh(const String& Name) : Asset(Name)
    {
    }

    StaticMesh::~StaticMesh()
    {
        delete m_VertexBuffer;
        delete m_IndexBuffer;
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

    static bool HasFBXExtension(const Path& Filepath)
    {
        return Filepath.string().ends_with(".fbx") || Filepath.string().ends_with(".FBX");
    }

    static bool HasGLTFExtension(const Path& Filepath)
    {
        return Filepath.string().ends_with(".gltf") || Filepath.string().ends_with(".GLTF");
    }

    static bool HasGLBExtension(const Path& Filepath)
    {
        return Filepath.string().ends_with(".glb") || Filepath.string().ends_with(".GLB");
    }

    static bool HasOBJExtension(const Path& Filepath)
    {
        return Filepath.string().ends_with(".obj") || Filepath.string().ends_with(".OBJ");
    }

    static bool HasDAEExtension(const Path& Filepath)
    {
        return Filepath.string().ends_with(".dae") || Filepath.string().ends_with(".DAE");
    }

    static ModelFormat GetModelFormat(const Path& Filepath)
    {
        if (HasGLTFExtension(Filepath)) return ModelFormat::GLTF;
        if (HasFBXExtension(Filepath)) return ModelFormat::FBX;
        if (HasGLBExtension(Filepath)) return ModelFormat::GLB;
        if (HasOBJExtension(Filepath)) return ModelFormat::OBJ;
        if (HasDAEExtension(Filepath)) return ModelFormat::DAE;
        return ModelFormat::None;
    }
    
    bool StaticMesh::LoadFromFile(const Path& Filepath)
    {
        switch (GetModelFormat(Filepath))
        {
        case ModelFormat::FBX:  return LoadFromFileAssimp(Filepath);
        case ModelFormat::GLTF: return LoadFromFileAssimp(Filepath);
        case ModelFormat::GLB:  return LoadFromFileAssimp(Filepath);
        case ModelFormat::OBJ:  return LoadFromFileAssimp(Filepath);
        case ModelFormat::DAE:  return LoadFromFileAssimp(Filepath);
        case ModelFormat::None: return false;
        }
        return false;
    }

    const Array<SubMesh>& StaticMesh::GetSubMeshes() const
    {
        return m_SubMeshes;
    }

    VertexBuffer* StaticMesh::GetVertexBuffer() const
    {
        return m_VertexBuffer;
    }

    IndexBuffer* StaticMesh::GetIndexBuffer() const
    {
        return m_IndexBuffer;
    }

    bool StaticMesh::LoadFromFileAssimp(const Path& Filepath)
    {
        const std::string filepath = Filepath.string();
        Assimp::Importer Importer;
        constexpr u32 Flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices;
        const aiScene* LoadedScene = Importer.ReadFile(Filepath.string().c_str(), Flags);
        if (!LoadedScene) return false;
        
        Array<Vertex> AllVertices;
        Array<u32> AllIndices;

        for (size_t MeshIndex = 0; MeshIndex < LoadedScene->mNumMeshes; MeshIndex++)
        {
            const aiMesh* LoadedMesh = LoadedScene->mMeshes[MeshIndex];

            Array<u32> Indices;
            for (u32 FaceIndex = 0; FaceIndex < LoadedMesh->mNumFaces; ++FaceIndex)
            {
                const aiFace& Face = LoadedMesh->mFaces[FaceIndex];
                for (u32 i = 0; i < Face.mNumIndices; ++i)
                    Indices.Add(Face.mIndices[i]);
            }

            Array<Vertex> Vertices;
            for (u32 VertexIndex = 0; VertexIndex < LoadedMesh->mNumVertices; ++VertexIndex)
            {
                const aiVector3D& Position = LoadedMesh->HasPositions() ? LoadedMesh->mVertices[VertexIndex] : aiVector3D(0, 0, 0);
                const aiVector3D& TexCoord = LoadedMesh->HasTextureCoords(0) ? LoadedMesh->mTextureCoords[0][VertexIndex] : aiVector3D(0, 0, 0);
                const aiVector3D& Normal = LoadedMesh->HasNormals() ? LoadedMesh->mNormals[VertexIndex] : aiVector3D(0, 0, 0);
                const aiColor4D& Color = LoadedMesh->HasVertexColors(0) ? LoadedMesh->mColors[0][VertexIndex] : aiColor4D(0, 0, 0, 0);

                Vector3(* const ToVector3)(const aiVector3D&) = [](const aiVector3D& In) { return Vector3(In.x, In.y, In.z); };
                Vector2(* const ToVector2)(const aiVector3D&) = [](const aiVector3D& In) { return Vector2(In.x, In.y); };
                Vector4(* const ToVector4)(const aiColor4D&) = [](const aiColor4D& In) { return Vector4(In.r, In.g, In.b, In.a); };


                const Vertex Vertex
                {
                    .Position = ToVector3(Position),
                    .TextureCoordinate = ToVector2(TexCoord),
                    .Normal = ToVector3(Normal),
                    .Color = ToVector4(Color)
                };
                Vertices.Add(Vertex);
            }

            AllVertices.AddRange(Vertices);
            AllIndices.AddRange(Indices);

            SubMesh SubMesh { };
            SubMesh.Name = LoadedMesh->mName.C_Str();
            SubMesh.VertexBufferSize = Vertices.Count() * sizeof(Vertex);
            SubMesh.VertexBufferOffset = MeshIndex == 0 ? 0 : m_SubMeshes[MeshIndex - 1].VertexBufferOffset + m_SubMeshes[MeshIndex - 1].VertexBufferSize;
            SubMesh.IndexBufferSize = Indices.Count() * sizeof(u32);
            SubMesh.IndexBufferOffset = MeshIndex == 0 ? 0 : m_SubMeshes[MeshIndex - 1].IndexBufferOffset + m_SubMeshes[MeshIndex - 1].IndexBufferSize;
            m_SubMeshes.Add(SubMesh);
        }

        Renderer* Renderer = g_Application->GetRenderer();
        m_VertexBuffer = Renderer->CreateVertexBuffer(BufferView(AllVertices.Data(), AllVertices.Count()));
        m_IndexBuffer = Renderer->CreateIndexBuffer(BufferView(AllIndices.Data(), AllIndices.Count()));

        Importer.FreeScene();
        return true;
    }
}
