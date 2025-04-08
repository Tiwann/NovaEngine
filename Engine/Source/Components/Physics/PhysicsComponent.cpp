#include "PhysicsComponent.h"

namespace Nova
{
    void PhysicsComponent::OnInspectorGUI(const ImGuiIO& IO)
    {
        ImGui::Checkbox("Draw Debug", &m_ShowCollisions);
    }

    bool PhysicsComponent::GetShowCollisions() const
    {
        return m_ShowCollisions;
    }

    void PhysicsComponent::SetShowCollisions(bool Enabled)
    {
        m_ShowCollisions = Enabled;
    }
}
