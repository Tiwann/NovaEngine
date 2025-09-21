#pragma once
#include "Runtime/Color.h"
#include "Runtime/Component.h"

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
        LightComponent(Entity* owner, const String& name);
        //void OnInspectorGUI(const ImGuiIO& IO) override;

        float GetIntensity() const;
        void SetIntensity(float intensity);
        
        const Color& GetColor() const;
        void SetColor(const Color& color);

        LightType GetType() const;
    protected:
        float m_Intensity = 1.0f;
        Color m_Color = Color::White;
        LightType m_Type = LightType::Directional;
    };
}
