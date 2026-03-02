#pragma once
#include "Math/Matrix4.h"
#include "Runtime/Object.h"
#include "Runtime/Ref.h"
#include "Containers/Map.h"

namespace Nova
{
    class CommandBuffer;
    class Camera;
    class Scene;
    class StaticMesh;
    class Texture;

    class SceneRenderer final : public Nova::Object
    {
    public:
        SceneRenderer() : Object("Scene Renderer"){}
        ~SceneRenderer() override = default;

        void SetScene(const Scene* scene);

        void Begin(const Camera* camera);
        void End();

        void SubmitMesh(Ref<StaticMesh> staticMesh, const Matrix4& transform);
        void PrepareForRender(CommandBuffer& cmdBuffer);
        void RenderScene(CommandBuffer& cmdBuffer);
    private:
        const Camera* m_Camera = nullptr;
        const Scene* m_Scene = nullptr;
        bool m_Begin = false;

        Map<StaticMesh*, Array<Matrix4>> m_StaticMeshMap;
    };
}
