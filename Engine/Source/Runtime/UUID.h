#pragma once
#include "Types.h"
#include "Containers/String.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_HIDDEN UUID
    {
    public:
        UUID();
        UUID(const UUID& Guid);
        UUID(u64 Low, u64 High);
        const u64* GetValues() const;
        String GetString() const;

        bool operator==(const UUID&) const;

        static UUID Zero;
    private:
        u64 m_Values[2] = {};
    };
}
