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
        explicit Asset(const String& Name);
        ~Asset() override = default;

        virtual String GetAssetType() const = 0;
        UUID GetUUID() const { return m_GUID; }
        void SetUUID(const UUID& Value) { m_GUID = Value; }

        virtual bool Serialize(class MemoryStream& Stream) { return false; }
        virtual bool Deserialize(class MemoryStream& Stream) { return false; }
    private:
        UUID m_GUID;
    };
}
