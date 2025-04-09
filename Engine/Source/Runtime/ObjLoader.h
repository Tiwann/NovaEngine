#pragma once
#include "Color.h"
#include "Containers/StaticArray.h"
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Filesystem.h"
#include "Flags.h"

namespace Nova
{
    enum class ObjFlagBits
    {
        None = 0,
        NOVA_FLAG_BIT(GenerateVertexNormals, 0),
        NOVA_FLAG_BIT(GenerateFaceNormals, 1),
    };

    NOVA_DECLARE_FLAGS(ObjFlagBits, ObjFlags)
    class ObjLoader
    {
        using Face = StaticArray<u32, 3>;
    public:
        struct Mesh
        {
            String Name;
            Array<Vector3> Positions;
            Array<Vector2> TextureCoordinates;
            Array<Vector3> Normals;
            Array<Color> Colors;
            Array<Face> Faces;
        };
        
        bool LoadFromFile(const Path& Filepath, ObjFlags Flags = ObjFlagBits::None);
        const Array<Mesh>& GetMeshes() const;
        const Path& GetMaterialLibFilepath() const;
    private:
        void ValidateMesh(Mesh& Mesh, ObjFlags Flags);
        static void CalculateMeshNormals(Mesh& Mesh);
        Array<Mesh> m_Meshes;
        Path m_MaterialLibPath;
    };
}