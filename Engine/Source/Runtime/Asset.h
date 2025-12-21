#pragma once
#include "UUID.h"
#include "Object.h"
#include "AssetType.h"

namespace Nova
{
    class Asset : public Object
    {
    public:
        Asset() : m_Uuid(UUID::Generate()) {}
        ~Asset() override = default;

        explicit Asset(const String& name) : Object(name), m_Uuid(UUID::Generate()) {}

        virtual AssetType GetAssetType() const = 0;

        void SetUuid(const UUID& uuid) { m_Uuid = uuid; }
        UUID GetUuid() const { return m_Uuid; }

    private:
        UUID m_Uuid;
    };
}
