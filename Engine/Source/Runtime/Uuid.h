#pragma once
#include "Containers/String.h"
#include <cstdint>

namespace Nova
{
    class Uuid
    {
    public:
        Uuid() = default;
        Uuid(const Uuid& uuid);
        explicit Uuid(uint64_t low, uint64_t high);

        static Uuid Generate();
        const uint64_t* GetValues() const;
        String GetString() const;

        bool operator==(const Uuid&) const;

        static Uuid Zero;
    private:
        uint64_t m_Values[2] = { 0, 0 };
    };
}
