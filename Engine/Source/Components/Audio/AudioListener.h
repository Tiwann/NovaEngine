#pragma once
#include "Game/Component.h"
#include "Math/Vector3.h"
#include <cstdint>

namespace Nova
{
    class AudioListener : public Component
    {
    public:
        AudioListener(Entity* owner);

        void OnInit() override;
        void OnDestroy() override;
        void OnStart() override;
        void OnUpdate(float deltaTime) override;
        void OnEnable() override;
        void OnDisable() override;
        // void OnInspectorGUI(const ImGuiIO& IO) override;
        void SetIndex(uint32_t index);
        uint32_t GetIndex() const;
    private:
        uint32_t m_Index = 0;
        Vector3 m_LastPosition = Vector3::Zero;
    };
}
