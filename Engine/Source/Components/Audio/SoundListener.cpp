#include "SoundListener.h"
#include "Runtime/Application.h"
#include "Audio/AudioSystem.h"
#include "Components/Transform.h"
#include "Components/Physics/PhysicsComponent.h"
#include "Editor/EditorGUI.h"
#include <fmod/fmod.hpp>


static constexpr char ComponentName[15] = "Sound Listener";

namespace Nova
{
    SoundListener::SoundListener(Entity* Owner): Component(Owner, ComponentName)
    {
        
    }

    void SoundListener::OnInit()
    {
        Component::OnInit();
    }

    void SoundListener::OnDestroy()
    {
        Component::OnDestroy();
    }

    void SoundListener::OnStart()
    {
        Component::OnStart();
    }

    void SoundListener::OnUpdate(f32 Delta)
    {
        Component::OnUpdate(Delta);

        const AudioSystem* AudioSystem = g_Application->GetAudioSystem();
        FMOD::System* AudioSystemHandle = AudioSystem->GetHandle();

        const PhysicsComponent* Physics = m_Entity->GetComponent<PhysicsComponent>();
        const Vector3 Position = GetTransform()->GetPosition();
        const Vector3 Forward = GetTransform()->GetForwardVector();
        const Vector3 Up = GetTransform()->GetUpVector();
        const Vector3 Velocity = Physics ? Physics->GetLinearVelocity() : Vector3::Zero;
        
        AudioSystemHandle->set3DListenerAttributes((int)m_Index,
            (const FMOD_VECTOR*)&Position,
            (const FMOD_VECTOR*)&Velocity,
            (const FMOD_VECTOR*)&Forward,
            (const FMOD_VECTOR*)&Up);

        
    }

    void SoundListener::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);
        UI::SliderValue<u32>("Index", m_Index, 0, FMOD_MAX_LISTENERS, "%d", SliderFlagBits::AlwaysClamp);
    }

    void SoundListener::SetIndex(u32 Index)
    {
        m_Index = Index;
    }
    
}
