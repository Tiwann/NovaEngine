#pragma once
#include "Color.h"
#include "Math/Vector2.h"
#include <random>
#include <limits>
#include <type_traits>

namespace Nova
{
    class Random
    {
    public:
        static Color Color();
        static float Float(float min, float max);

        template<typename T> requires std::is_integral<T>::value
        static T Integer(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
        {
            std::uniform_int_distribution<T> distribution(min, max);
            return distribution(s_RandomDevice);
        }

        static Vector2 Vector2(float xMin, float yMin, float xMax, float yMax);
    private:
        static std::random_device s_RandomDevice;
        static std::mt19937 s_RandomEngine;
    };
}
