#pragma once
#include "UUID.h"
#include "Object.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_API Asset : public Object
    {
    public:
        Asset() = default;
        Asset(const String& Name);
        ~Asset() override = default;

        virtual String GetAssetType() const = 0;
        UUID GetGuid() const { return m_GUID; }
        void SetGUID(const UUID& Value) { m_GUID = Value; }
    private:
        UUID m_GUID;
    };
}
