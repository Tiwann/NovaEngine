#include "Model.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/VertexBufferLayout.h"
#include "ObjLoader.h"

#include <tinygltf/tinygltf.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Nova
{
    Mesh::~Mesh()
    {
        if (VertexArray)
        {
            delete VertexArray;
            VertexArray = nullptr;
        }

        if (VertexBuffer)
        {
            delete VertexBuffer;
            VertexBuffer = nullptr;
        }
    }

    Model::Model(const String& Name) : Asset(Name)
    {
    }

    Model::~Model()
    {
        for (Mesh* Mesh : m_Meshes)
        {
            delete Mesh;
            Mesh = nullptr;
        }
    }

    String Model::GetAssetType() const
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
    
    bool Model::LoadFromFile(const Path& Filepath)
    {
        switch (GetModelFormat(Filepath))
        {
        case ModelFormat::FBX:  return LoadFromFileAssimp(Filepath);
        case ModelFormat::GLTF: return LoadGLTFFromFile(Filepath);
        case ModelFormat::GLB:  return LoadGLBFromFile(Filepath);
        case ModelFormat::OBJ:  return LoadFromFileAssimp(Filepath);
        case ModelFormat::DAE:  return LoadFromFileAssimp(Filepath);
        case ModelFormat::None: return false;
        }
        return false;
    }

    Mesh* Model::GetMesh(Array<Mesh>::SizeType Index)
    {
        return m_Meshes.GetAt(Index);
    }

    
    bool Model::LoadFromFileAssimp(const Path& Filepath)
    {
        std::string filepath = Filepath.string();
        Assimp::Importer Importer;
        constexpr u32 Flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices;
        const aiScene* Scene = Importer.ReadFile(Filepath.string().c_str(), Flags);
        if (!Scene) return false;
        
        for (size_t MeshIndex = 0; MeshIndex < Scene->mNumMeshes; ++MeshIndex)
        {
            const aiMesh* LoadedMesh = Scene->mMeshes[MeshIndex];
            Mesh* CombinedMesh = new Mesh();
            CombinedMesh->Name = String(const_cast<char*>(LoadedMesh->mName.data), LoadedMesh->mName.length);
            
            Array<u32> Indices;
            for (u32 FaceIndex = 0; FaceIndex < LoadedMesh->mNumFaces; ++FaceIndex)
            {
                const aiFace& Face = LoadedMesh->mFaces[FaceIndex];
                for (u32 i = 0; i < Face.mNumIndices; ++i)
                    Indices.Add(Face.mIndices[i]);
            }
            
            for (u32 index = 0; index < Indices.Count(); ++index)
            {
                const aiVector3D& Position = LoadedMesh->HasPositions() ? LoadedMesh->mVertices[Indices[index]] : aiVector3D(0, 0, 0);
                const aiVector3D& TexCoord = LoadedMesh->HasTextureCoords(0) ? LoadedMesh->mTextureCoords[0][Indices[index]] : aiVector3D(0, 0, 0);
                const aiVector3D& Normal = LoadedMesh->HasNormals() ? LoadedMesh->mNormals[Indices[index]] : aiVector3D(0, 0, 0);
                const aiColor4D& Color = LoadedMesh->HasVertexColors(0) ? LoadedMesh->mColors[0][Indices[index]] : aiColor4D(0, 0, 0, 0);
                const aiVector3D& Tangent = LoadedMesh->HasTangentsAndBitangents() ? LoadedMesh->mTangents[Indices[index]] : aiVector3D(0, 0, 0);
                const aiVector3D& Bitangent = LoadedMesh->HasTangentsAndBitangents() ? LoadedMesh->mBitangents[Indices[index]] : aiVector3D(0, 0, 0);

                const auto ToVector3 = [](const aiVector3D& In) { return Vector3(In.x, In.y, In.z); };
                const auto ToVector2 = [](const aiVector3D& In) { return Vector2(In.x, In.y); };
                const auto ToVector4 = [](const aiColor4D& In) { return Vector4(In.r, In.g, In.b, In.a); };

                
                const Vertex Vertex {
                    .Position = ToVector3(Position),
                    .TextureCoordinate = ToVector2(TexCoord),
                    .Normal = ToVector3(Normal),
                    .Color = ToVector4(Color)
                };
                CombinedMesh->Vertices.Add(Vertex);
            }
            
            CombinedMesh->VertexArray = VertexArray::Create(TODO);
            CombinedMesh->VertexArray->Bind();
            
            CombinedMesh->VertexBuffer = VertexBuffer::Create(TODO, TODO);
            CombinedMesh->VertexBuffer->SendData(CombinedMesh->Vertices.Data(), CombinedMesh->Vertices.Count());
            CombinedMesh->VertexArray->SetBufferLayout(VertexBufferLayout::Default);
            m_Meshes.Add(CombinedMesh);
        }

        Importer.FreeScene();
        return true;
    }

    void Model::ConstructModel(const tinygltf::Model& Model)
    {
        for (const tinygltf::Mesh& LoadedMesh : Model.meshes)
        {
            Mesh* CombinedMesh = new Mesh;
            CombinedMesh->Name = String(LoadedMesh.name.data());
            for (const tinygltf::Primitive& Primitive : LoadedMesh.primitives)
            {
                const tinygltf::Accessor& IndexAccessor = Model.accessors[Primitive.indices];
                const tinygltf::BufferView& IndexBufferView = Model.bufferViews[IndexAccessor.bufferView];
                const tinygltf::Buffer& IndexBuffer = Model.buffers[IndexBufferView.buffer];
                const unsigned char* IndexData = IndexBuffer.data.data() + IndexBufferView.byteOffset + IndexAccessor.
                    byteOffset;

                // Access vertices
                const tinygltf::Accessor& PositionAccessor = Model.accessors[Primitive.attributes.find("POSITION")->
                    second];
                const tinygltf::BufferView& PositionBufferView = Model.bufferViews[PositionAccessor.bufferView];
                const tinygltf::Buffer& PositionBuffer = Model.buffers[PositionBufferView.buffer];
                const unsigned char* PositionData = PositionBuffer.data.data() + PositionBufferView.byteOffset +
                    PositionAccessor.byteOffset;

                const tinygltf::Accessor& TexCoordAccessor = Model.accessors[Primitive.attributes.find("TEXCOORD_0")->
                    second];
                const tinygltf::BufferView& TexCoordBufferView = Model.bufferViews[TexCoordAccessor.bufferView];
                const tinygltf::Buffer& TexCoordBuffer = Model.buffers[TexCoordBufferView.buffer];
                const unsigned char* TexCoordData = TexCoordBuffer.data.data() + TexCoordBufferView.byteOffset +
                    TexCoordAccessor.byteOffset;

                const tinygltf::Accessor& NormalsAccessor = Model.accessors[Primitive.attributes.find("NORMAL")->
                    second];
                const tinygltf::BufferView& NormalsBufferView = Model.bufferViews[NormalsAccessor.bufferView];
                const tinygltf::Buffer& NormalsBuffer = Model.buffers[NormalsBufferView.buffer];
                const unsigned char* NormalsData = NormalsBuffer.data.data() + NormalsBufferView.byteOffset +
                    NormalsAccessor.byteOffset;

                bool HasColorAttribute = Primitive.attributes.contains("COLOR_0");
                const tinygltf::Accessor& ColorAccessor = HasColorAttribute ? Model.accessors[Primitive.attributes.find("COLOR_0")->second] : tinygltf::Accessor();
                const tinygltf::BufferView& ColorBufferView = HasColorAttribute ? Model.bufferViews[ColorAccessor.bufferView] : tinygltf::BufferView();
                const tinygltf::Buffer& ColorBuffer = HasColorAttribute ? Model.buffers[ColorBufferView.buffer] : tinygltf::Buffer();
                const unsigned char* ColorData = HasColorAttribute ? ColorBuffer.data.data() + ColorBufferView.byteOffset + ColorAccessor.
                    byteOffset : nullptr;
                
                for (size_t i = 0; i < IndexAccessor.count; ++i)
                {
                    size_t Index = 0;
                    switch (IndexAccessor.componentType)
                    {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        Index = reinterpret_cast<const uint8_t*>(IndexData)[i];
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        Index = reinterpret_cast<const uint16_t*>(IndexData)[i];
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        Index = reinterpret_cast<const uint32_t*>(IndexData)[i];
                        break;
                    }

                    const Vector3& Position = *(const Vector3*)(PositionData + Index * PositionAccessor.ByteStride(PositionBufferView));
                    const Vector2& TexCoord = *(const Vector2*)(TexCoordData + Index * TexCoordAccessor.ByteStride(TexCoordBufferView));
                    const Vector3& Normals = *(const Vector3*)(NormalsData + Index * NormalsAccessor.ByteStride(NormalsBufferView));

                    Vertex Vertex;
                    Vertex.Position = Position;
                    Vertex.Normal = Normals;
                    Vertex.TextureCoordinate = TexCoord;
                    if (HasColorAttribute)
                    {
                        if (ColorAccessor.type == TINYGLTF_TYPE_VEC3)
                        {
                            const Vector3& Colors = *(const Vector3*)(ColorData + static_cast<size_t>(Index * ColorAccessor.ByteStride(ColorBufferView)));
                            const Vector4 ColorsVec4 = Vector4(Colors, 1.0f);
                            Vertex.Color = ColorsVec4;
                        }

                        if (ColorAccessor.type == TINYGLTF_TYPE_VEC4)
                        {
                            const Vector4& Colors = *(const Vector4*)(ColorData + static_cast<size_t>(Index * ColorAccessor.ByteStride(ColorBufferView)));
                            Vertex.Color = Colors;
                        }
                    }
                    else
                    {
                        Vertex.Color = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
                    }
                    
                    CombinedMesh->Vertices.Add(Vertex);
                }

                CombinedMesh->VertexArray = VertexArray::Create(TODO);
                CombinedMesh->VertexArray->Bind();

                CombinedMesh->VertexBuffer = VertexBuffer::Create(TODO, TODO);
                CombinedMesh->VertexBuffer->SendData(CombinedMesh->Vertices.Data(), CombinedMesh->Vertices.Count());
                CombinedMesh->VertexArray->SetBufferLayout(VertexBufferLayout::Default);
            }
            m_Meshes.Add(CombinedMesh);
        }
    }
    
    bool Model::LoadGLTFFromFile(const Path& Filepath)
    {
        tinygltf::TinyGLTF Context;
        tinygltf::Model LoadedModel;
        if (!Context.LoadASCIIFromFile(&LoadedModel, nullptr, nullptr, Filepath.string())) return false;
        ConstructModel(LoadedModel);
        return true;
    }

    bool Model::LoadGLBFromFile(const Path& Filepath)
    {
        tinygltf::TinyGLTF Context;
        tinygltf::Model LoadedModel;
        if (!Context.LoadBinaryFromFile(&LoadedModel, nullptr, nullptr, Filepath.string())) return false;
        ConstructModel(LoadedModel);
        return true;
    }

    
    bool Model::LoadOBJFromFile(const Path& Filepath)
    {
        ObjLoader Loader;
        if (!Loader.LoadFromFile(Filepath)) return false;

        for (const ObjLoader::Mesh& LoadedMesh : Loader.GetMeshes())
        {
            Nova::Mesh* CombinedMesh = new Nova::Mesh;
            CombinedMesh->Name = LoadedMesh.Name;

            for (size_t Index = 0; Index < LoadedMesh.Faces.Count(); Index++)
            {
                const u32 PositionIndex = LoadedMesh.Faces[Index][0] - 1;
                const u32 TexCoordIndex = LoadedMesh.Faces[Index][1] - 1;
                const u32 NormalIndex = LoadedMesh.Faces[Index][2] - 1;
                const u32 ColorIndex = LoadedMesh.Faces[Index][0] - 1; // Indexing at vertex position
                
                const Vector3 Position = LoadedMesh.Positions.IsEmpty() ? Vector3::Zero : LoadedMesh.Positions[PositionIndex];
                const Vector2 TextureCoordinate = LoadedMesh.TextureCoordinates.IsEmpty() ? Vector2::Zero :  LoadedMesh.TextureCoordinates[TexCoordIndex];
                const Vector3 Normal = LoadedMesh.Normals.IsEmpty() ? Vector3::Zero : LoadedMesh.Normals[NormalIndex];
                const Color Color = LoadedMesh.Colors.IsEmpty() ? Color::Black : LoadedMesh.Colors[ColorIndex]; 
                CombinedMesh->Vertices.Add(Vertex {
                    .Position = Position,
                    .TextureCoordinate = TextureCoordinate,
                    .Normal = Normal,
                    .Color = Color
                });
            }

            CombinedMesh->VertexArray = VertexArray::Create(TODO);
            CombinedMesh->VertexArray->Bind();

            CombinedMesh->VertexBuffer = VertexBuffer::Create(TODO, TODO);
            CombinedMesh->VertexBuffer->SendData(CombinedMesh->Vertices.Data(), CombinedMesh->Vertices.Count());
            CombinedMesh->VertexArray->SetBufferLayout(VertexBufferLayout::Default);
            m_Meshes.Add(CombinedMesh);
        }
        return true;
    }
}
