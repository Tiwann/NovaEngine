#pragma once
#include "Core/NumericTypes.h"
#include "Containers/String.h"

namespace Nova
{
    class GUID
    {
    public:
        GUID();
        GUID(const GUID& Guid);
        GUID(u64 Low, u64 High);
        const u64* GetValues() const;
        String GetString() const;

        bool operator==(const GUID&) const;

        static GUID Zero;
    private:
        u64 m_Values[2] = {};
    };
}
