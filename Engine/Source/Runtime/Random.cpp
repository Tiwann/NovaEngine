#include "Random.h"

namespace Nova
{
    std::random_device Random::s_RandomDevice;
    std::mt19937 Random::s_RandomEngine = std::mt19937(s_RandomDevice());
    
    Color Random::Color()
    {
        const float red = Float(0.0f, 1.0f);
        const float green = Float(0.0f, 1.0f);
        const float blue = Float(0.0f, 1.0f);
        return { red, green, blue, 1.0f };
    }

    float Random::Float(const float min, const float max)
    {
        std::uniform_real_distribution distribution(min, max);
        return distribution(s_RandomDevice);
    }


    Vector2 Random::Vector2(const float xMin, const float yMin, const float xMax, const float yMax)
    {
        return { Float(xMin, yMin), Float(xMax, yMax) };
    }
}
