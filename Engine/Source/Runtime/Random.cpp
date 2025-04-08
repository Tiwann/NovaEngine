#include "Random.h"

#include <random>

namespace Nova
{
    std::random_device Random::s_RandomDevice;
    
    Color Random::Color()
    {
        const f32 Red = Float(0.0f, 1.0f);
        const f32 Green = Float(0.0f, 1.0f);
        const f32 Blue = Float(0.0f, 1.0f);
        return {Red, Green, Blue, 1.0f};
    }

    f32 Random::Float(f32 Min, f32 Max)
    {
        std::uniform_real_distribution Distribution(Min, Max);
        return Distribution(s_RandomDevice);
    }

    i32 Random::Integer(i32 Min, i32 Max)
    {
        std::uniform_int_distribution Distribution(Min, Max - 1);
        return Distribution(s_RandomDevice);
    }

    Vector2 Random::Vector2(f32 MinX, f32 MaxX, f32 MinY, f32 MaxY)
    {
        return { Float(MinX, MaxX), Float(MinY, MaxY) };
    }
}
