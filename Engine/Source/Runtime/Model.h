#pragma once
#include "Asset.h"
#include "Filesystem.h"
#include "Rendering/Vertex.h"
#include "Containers/Array.h"


namespace tinygltf
{
    class Model;
}
namespace Nova
{
    class VertexArray;
    class VertexBuffer;
    
    struct Mesh
    {
        String Name = "Empty Mesh";
        Array<Vertex> Vertices;
        VertexArray* VertexArray = nullptr;
        VertexBuffer* VertexBuffer = nullptr;

        ~Mesh();
    };

    class Model : public Asset
    {
    public:
        Model() = default;
        Model(const String& Name);
        ~Model() override;

        String GetAssetType() const override;

        
        bool LoadFromFile(const Path& Filepath);

        Mesh* GetMesh(Array<Mesh>::SizeType Index);

        Array<Mesh*>& GetMeshes() { return m_Meshes; }

    private:
        void ConstructModel(const tinygltf::Model& Model);
        bool LoadFromFileAssimp(const Path& Filepath);
        bool LoadGLTFFromFile(const Path& Filepath);
        bool LoadGLBFromFile(const Path& Filepath);
        bool LoadOBJFromFile(const Path& path);
    private:
        Array<Mesh*> m_Meshes;
    };
    
}
