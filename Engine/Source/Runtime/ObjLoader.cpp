#include "ObjLoader.h"
#include "Serialization/MemoryStream.h"
#include "Math/LinearAlgebra.h"
#include "Color.h"
namespace Nova
{
    bool ObjLoader::LoadFromFile(const Path& Filepath, ObjFlags Flags)
    {
        String Content = File::ReadToString(Filepath);
        if (Content.IsEmpty()) return false;
        const BufferView<u8> ContentView((u8*)Content.Data(), Content.Count());
        MemoryStream MemStream(ContentView);
        
        String Line;
        Vector3 TempVec3;
        Vector2 TempVec2;
        Color TempColor = 0x000000FF;
        StaticArray<Face, 3> TempFaces;
        Mesh TempMesh;
        u32 MeshCount = 0;
        
        while (MemStream.ReadLine(Line) != Stream::EndOfFile)
        {
            if (Line.StartsWith("#"))
                continue;

            
            if (Line.StartsWith("o") || Line.StartsWith("g"))
            {
                if(MeshCount == m_Meshes.Count())
                {
                    TempMesh.Name = Line.Substring(2, Line.Count());
                    MeshCount++;
                    continue;
                }
                
                ValidateMesh(TempMesh, Flags);
                TempMesh.Name = Line.Substring(2, Line.Count());
                MeshCount++;
                continue;
            }

            if (Line.StartsWith("v "))
            {
                Line = Line.Substring(2, Line.Count());
                const u32 Scanned = Line.ScanFormat("%f %f %f %f %f %f", &TempVec3.x, &TempVec3.y, &TempVec3.z, &TempColor.r, &TempColor.g, &TempColor.b);
                if (Scanned >= 3) TempMesh.Positions.Add(TempVec3);
                if (Scanned == 6) TempMesh.Colors.Add(TempColor);
                continue;
            }

            if (Line.StartsWith("vt"))
            {
                Line = Line.Substring(2, Line.Count());
                Line.ScanFormat("%f %f", &TempVec2.x, &TempVec2.y);
                TempMesh.TextureCoordinates.Add(TempVec2);
                continue;
            }
            
            if (Line.StartsWith("vn"))
            {
                Line = Line.Substring(3, Line.Count());
                Line.ScanFormat("%f %f %f", &TempVec3.x, &TempVec3.y, &TempVec3.z);
                TempMesh.Normals.Add(TempVec3);
                continue;
            }

            
            if (Line.StartsWith("f"))
            {
                // v only
                if (Line.ScanFormat("f %d %d %d", &TempFaces[0][0], &TempFaces[1][0], &TempFaces[2][0]) == 3)
                {
                    TempMesh.Faces.Add(TempFaces[0]);
                    TempMesh.Faces.Add(TempFaces[1]);
                    TempMesh.Faces.Add(TempFaces[2]);
                    continue;
                }

                // v/vt
                if (Line.ScanFormat("f %d/%d %d/%d %d/%d",
                    &TempFaces[0][0], &TempFaces[0][1],
                    &TempFaces[1][0], &TempFaces[1][1],
                    &TempFaces[2][0], &TempFaces[2][1]) == 6)
                {
                    TempMesh.Faces.Add(TempFaces[0]);
                    TempMesh.Faces.Add(TempFaces[1]);
                    TempMesh.Faces.Add(TempFaces[2]);
                    continue;
                }

                // v//vn
                if (Line.ScanFormat("f %d//%d %d//%d %d//%d",
                    &TempFaces[0][0], &TempFaces[0][2],
                    &TempFaces[1][0], &TempFaces[1][2],
                    &TempFaces[2][0], &TempFaces[2][2]) == 6)
                {
                    TempMesh.Faces.Add(TempFaces[0]);
                    TempMesh.Faces.Add(TempFaces[1]);
                    TempMesh.Faces.Add(TempFaces[2]);
                    continue;
                }

                // v/vt/vn
                if (Line.ScanFormat("f %d/%d/%d %d/%d/%d %d/%d/%d",
                    &TempFaces[0][0], &TempFaces[0][1], &TempFaces[0][2],
                    &TempFaces[1][0], &TempFaces[1][1], &TempFaces[1][2],
                    &TempFaces[2][0], &TempFaces[2][1], &TempFaces[2][2]) == 9)

                TempMesh.Faces.Add(TempFaces[0]);
                TempMesh.Faces.Add(TempFaces[1]);
                TempMesh.Faces.Add(TempFaces[2]);
                continue;
            }

            if (Line.StartsWith("mtllib"))
            {
                String IncludeStr(Line.Substring(7));
                const Path Directory = File::GetDirectory(Filepath);
                m_MaterialLibPath = weakly_canonical(weakly_canonical(Directory) / Path(*IncludeStr));
                continue;
            }
        }
        MemStream.Close();
        ValidateMesh(TempMesh, Flags);
        return true;
    }

    const Array<ObjLoader::Mesh>& ObjLoader::GetMeshes() const
    {
        return m_Meshes;
    }

    const Path& ObjLoader::GetMaterialLibFilepath() const
    {
        return m_MaterialLibPath;
    }

    void ObjLoader::ValidateMesh(Mesh& Mesh, ObjFlags Flags)
    {
        if (Flags.Contains(ObjFlagBits::GenerateVertexNormals))
            CalculateMeshNormals(Mesh);
        m_Meshes.Add(Mesh);
    }

    void ObjLoader::CalculateMeshNormals(Mesh& Mesh)
    {
        for (size_t FaceIndex = 0; FaceIndex < Mesh.Faces.Count(); FaceIndex += 3)
        {
            Face& Face0 = Mesh.Faces[FaceIndex + 0];
            Face& Face1 = Mesh.Faces[FaceIndex + 1];
            Face& Face2 = Mesh.Faces[FaceIndex + 2];
            const Vector3& Position0 = Mesh.Positions[Face0[0]];
            const Vector3& Position1 = Mesh.Positions[Face1[0]];
            const Vector3& Position2 = Mesh.Positions[Face2[0]];

            const Vector3 Edge1 = Position1 - Position0;
            const Vector3 Edge2 = Position2 - Position0;
            const Vector3 FaceNormal = Math::Normalize<Vector3>(Edge1.Cross(Edge2));
                
            Mesh.Normals.Add(FaceNormal);
            Face0[2] = (u32)FaceIndex + 1;
            Face1[2] = (u32)FaceIndex + 1;
            Face2[2] = (u32)FaceIndex + 1;
        }
    }
}
