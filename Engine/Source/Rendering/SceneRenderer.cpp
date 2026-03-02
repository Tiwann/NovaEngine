#include "SceneRenderer.h"
#include "CommandBuffer.h"
#include "Components/Camera.h"
#include "Runtime/StaticMesh.h"

namespace Nova
{
    void SceneRenderer::Begin(const Camera* camera)
    {
        NOVA_ASSERT(!m_Begin, "Begin/End mismatch!");
        m_Camera = camera;
        m_Begin = true;
    }

    void SceneRenderer::End()
    {
        NOVA_ASSERT(m_Begin, "Begin/End mismatch!");
        m_Begin = false;
    }

    void SceneRenderer::SubmitMesh(Ref<StaticMesh> staticMesh, const Matrix4& transform)
    {
        if (!staticMesh) return;
        Array<Matrix4>& transforms = m_StaticMeshMap[staticMesh];
        transforms.Add(transform);
    }

    void SceneRenderer::PrepareForRender(CommandBuffer& cmdBuffer)
    {
        // Upload
    }

    void SceneRenderer::RenderScene(CommandBuffer& cmdBuffer)
    {
        const auto width = m_Camera->GetWidth();
        const auto height = m_Camera->GetHeight();

        cmdBuffer.SetViewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
        cmdBuffer.SetScissor(0, 0, (int32_t)width, (int32_t)height);

        for (const auto& [staticMesh, transforms] : m_StaticMeshMap)
        {
            if (!staticMesh) continue;
            if (staticMesh->GetMaterialInfos().IsEmpty()) continue;

            const uint32_t instanceCount = transforms.Count();
            Ref<Buffer> vertexBuffer = staticMesh->GetVertexBuffer();
            Ref<Buffer> indexBuffer = staticMesh->GetIndexBuffer();

            const Array<MaterialInfo>& materialInfos = staticMesh->GetMaterialInfos();
            for (const MaterialInfo& materialInfo : materialInfos)
            {
                Ref<Material> material = staticMesh->GetMaterial(materialInfo.slot);
                if (!material) continue;
                cmdBuffer.BindMaterial(*material);

                for (const SubMeshInfo& subMesh : materialInfo.subMeshes)
                {
                    const uint32_t indexCount = subMesh.indexBufferSize / sizeof(uint32_t);

                    cmdBuffer.BindVertexBuffer(*vertexBuffer, subMesh.vertexBufferOffset);
                    cmdBuffer.BindIndexBuffer(*indexBuffer, subMesh.indexBufferOffset, Format::Uint32);
                    cmdBuffer.DrawIndexed(indexCount, instanceCount, 0, 0, 0);
                }
            }
        }
    }

    void SceneRenderer::SetScene(const Scene* scene)
    {
        m_Scene = scene;
    }

}
