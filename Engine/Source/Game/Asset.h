#pragma once
#include "Runtime/Uuid.h"
#include "Runtime/Object.h"

namespace Nova
{
    class Asset : public Object
    {
    public:
        Asset() = default;
        ~Asset() override = default;
        explicit Asset(const String& name);

        virtual String GetAssetType() const = 0;
        Uuid GetUuid() const { return m_Uuid; }
    private:
        Uuid m_Uuid;
    };
}
