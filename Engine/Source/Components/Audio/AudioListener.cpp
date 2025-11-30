#include "AudioListener.h"
#include "Audio/AudioSystem.h"
#include "Components/Transform.h"
#include <miniaudio.h>
#include <imgui.h>

namespace Nova
{
    AudioListener::AudioListener(Entity* owner): Component(owner, "Audio Listener")
    {

    }

    void AudioListener::OnInit()
    {
        Component::OnInit();
    }

    void AudioListener::OnDestroy()
    {
        Component::OnDestroy();
    }

    void AudioListener::OnStart()
    {
        Component::OnStart();
    }

    void AudioListener::OnUpdate(const float deltaTime)
    {
        Component::OnUpdate(deltaTime);

        AudioSystem* audioSystem = AudioSystem::GetInstance();

        const Transform* transform = GetTransform();
        const Vector3& position = transform->GetPosition();
        const Vector3 velocity = (position - m_LastPosition) / deltaTime;
        m_LastPosition = position;
        const Vector3 forward = transform->GetForwardVector();

        ma_engine_listener_set_direction(audioSystem->GetHandle(), m_Index, forward.x, forward.y, forward.z);
        ma_engine_listener_set_position(audioSystem->GetHandle(), m_Index, position.x, position.y, position.z);
        ma_engine_listener_set_velocity(audioSystem->GetHandle(), m_Index, velocity.x, velocity.y, velocity.z);
        //ma_engine_listener_set_cone(audioSystem->GetHandle(), m_Index, Math::Tau, Math::Tau, 1.0f);
    }

    void AudioListener::OnEnable()
    {
        Component::OnEnable();
        AudioSystem* audioSystem = AudioSystem::GetInstance();
        ma_engine_listener_set_enabled(audioSystem->GetHandle(), m_Index, true);
    }

    void AudioListener::OnDisable()
    {
        Component::OnDisable();
        AudioSystem* audioSystem = AudioSystem::GetInstance();
        ma_engine_listener_set_enabled(audioSystem->GetHandle(), m_Index, false);
    }

    void AudioListener::OnGui()
    {
        Component::OnGui();
        ImGui::DragInt("Lister Index", (int*)&m_Index, 1, 0, MA_ENGINE_MAX_LISTENERS, "%d", ImGuiSliderFlags_AlwaysClamp);
    }

    void AudioListener::SetIndex(const uint32_t index)
    {
        m_Index = index;
    }

    uint32_t AudioListener::GetIndex() const
    {
        return m_Index;
    }
}
