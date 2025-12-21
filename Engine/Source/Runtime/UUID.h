#pragma once
#include "Containers/String.h"
#include <cstdint>

namespace Nova
{
    class UUID
    {
    public:
        UUID() = default;
        UUID(const UUID& uuid);
        explicit UUID(uint64_t low, uint64_t high);

        static UUID Generate();
        const uint64_t* GetValues() const;
        String GetString() const;

        bool operator==(const UUID&) const;

        static UUID Zero;
    private:
        uint64_t m_Values[2] = { 0, 0 };
    };
}
