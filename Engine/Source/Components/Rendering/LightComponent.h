#pragma once
#include "Runtime/Color.h"
#include "Runtime/Component.h"
#include "Math/Vector3.h"

namespace Nova
{
    enum class LightType
    {
        Directional,
        Point,
        Spot,
        Ambient,
    };
    
    class LightComponent : public Component
    {
    public:
        LightComponent(Entity* Owner, const String& Name);
        void OnInspectorGUI(const ImGuiIO& IO) override;

        f32 GetIntensity() const;
        void SetIntensity(f32 intensity);
        
        const Color& GetColor() const;
        void SetColor(const Color& color);
        LightType GetType() const;

    protected:
        f32 m_Intensity = 1.0f;
        Color m_Color = Color::White;
        LightType m_Type = LightType::Directional;
    };
}
