#pragma once
#include "Runtime/Component.h"

namespace FMOD
{
    class System;
}

namespace Nova
{
    class SoundListener : public Component
    {
    public:
        SoundListener(Entity* Owner);

        void OnInit() override;
        void OnDestroy() override;
        void OnStart() override;
        void OnUpdate(f32 Delta) override;
        void OnInspectorGUI(const ImGuiIO& IO) override;
        void SetIndex(u32 Index);
    private:
        u32 m_Index = 0;
    };
}
