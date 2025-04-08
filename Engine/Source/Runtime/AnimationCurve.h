#pragma once
#include "Asset.h"

namespace Nova
{
   
    
    template<typename T> requires IsInterpolatable<T>
    class AnimationCurve : public Asset
    {
    public:
        using ValueType = T;
        using SizeType = typename Array<T>::SizeType;
        
        enum class WrapMode
        {
            Clamp,
            Repeat,
            Mirror,
        
        };
        
        struct KeyFrame
        {
            float Time;
            T Value;
            WrapMode PreWrapMode;
            WrapMode PostWrapMode;
        };
        
        AnimationCurve(const String& Name) : Asset(Name) {}

        void AddKeyFrame(const KeyFrame& InKeyFrame)
        {
            m_KeyFrames.Add(InKeyFrame);
        }

        T Evaluate(float Time)
        {
            for (KeyFrame& KeyFrame : m_KeyFrames)
            {
                
            }
        }
        
        SizeType Count() const { return m_KeyFrames.Count(); }
    private:
        Array<KeyFrame> m_KeyFrames;
    };
}
