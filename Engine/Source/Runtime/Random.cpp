#include "Random.h"

#include <random>

namespace Nova
{
    std::random_device Random::s_RandomDevice;
    
    Color Random::Color()
    {
        const float Red = Float(0.0f, 1.0f);
        const float Green = Float(0.0f, 1.0f);
        const float Blue = Float(0.0f, 1.0f);
        return { Red, Green, Blue, 1.0f };
    }

    float Random::Float(const float min, const float max)
    {
        std::uniform_real_distribution Distribution(min, max);
        return Distribution(s_RandomDevice);
    }

    int32_t Random::Integer(const int32_t min, const int32_t max)
    {
        std::uniform_int_distribution Distribution(min, max - 1);
        return Distribution(s_RandomDevice);
    }

    Vector2 Random::Vector2(const float xMin, const float yMin, const float xMax, const float yMax)
    {
        return { Float(xMin, yMin), Float(xMax, yMax) };
    }
}
