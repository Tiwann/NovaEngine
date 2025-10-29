#include "Time.h"
#include <chrono>

namespace Nova
{
    double Time::Get()
    {
        static const auto start = std::chrono::high_resolution_clock::now();
        const auto now = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start);
        return duration.count() / 1000000000.0;
    }
}
