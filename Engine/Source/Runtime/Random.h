#pragma once
#include <random>
#include "Color.h"
#include "TypeTraits.h"
#include "Math/Vector2.h"

namespace Nova
{
    class Random
    {
    public:
        static Color Color();
        static float Float(float min, float max);
        static int32_t Integer(int32_t min, int32_t max);

        template<typename T> requires IsIntegerValue<T>
        static T Int(T min, T max)
        {
            std::uniform_int_distribution<T> Distribution(min, max - 1);
            return Distribution(s_RandomDevice);
        }
        
        static Vector2 Vector2(float xMin, float yMin, float xMax, float yMax);
    private:
        static std::random_device s_RandomDevice;
    };
}
