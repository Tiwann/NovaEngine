#include "PostProcessStack.h"
#include "Rendering/PostProcessEffect.h"
#include "Rendering/Pipeline.h"
#include "Rendering/Renderer.h"
#include "Runtime/Application.h"
#include "Runtime/Entity.h"
#include "Runtime/Scene.h"

namespace Nova
{
    void PostProcessStack::OnInit()
    {
        const Scene* Scene = m_Entity->GetOwner();
        const Application* App = Scene->GetOwner();
        Renderer* Renderer = App->GetRenderer();

    }

    void PostProcessStack::OnRender(Renderer* Renderer)
    {
        
        for (PostProcessEffect* Effect : m_Effects)
        {

        }
    }

    void PostProcessStack::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);
    }

    void PostProcessStack::AddEffect(PostProcessEffect* Effect)
    {
        m_Effects.Add(Effect);
    }

    void PostProcessStack::RemoveEffect(PostProcessEffect* Effect)
    {
        m_Effects.Remove(Effect);
    }

    void PostProcessStack::SetRenderTarget(RenderTarget* RenderTarget)
    {
        m_RenderTarget = RenderTarget;
    }

    RenderTarget* PostProcessStack::GetRenderTarget() const
    {
        return m_RenderTarget;
    }
}
