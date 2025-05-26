#pragma once
#include "Runtime/Component.h"
#include "Containers/Array.h"


namespace Nova
{
    class PostProcessEffect;
    class RenderTarget;

    class PostProcessStack : public Component
    {
    public:
        explicit PostProcessStack(Entity* Owner) : Component(Owner, "Post Process Stack") {}

        void OnInit() override;
        void OnRender(Renderer* Renderer) override;
        void OnInspectorGUI(const ImGuiIO& IO) override;

        void AddEffect(PostProcessEffect* Effect);
        void RemoveEffect(PostProcessEffect* Effect);

        void SetRenderTarget(RenderTarget* RenderTarget);
        RenderTarget* GetRenderTarget() const;
    private:
        Array<PostProcessEffect*> m_Effects;
        RenderTarget* m_RenderTarget = nullptr;
    };


}
