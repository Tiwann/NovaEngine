#pragma once
#include "Uuid.h"
#include "Object.h"

namespace Nova
{
    class Asset : public Object
    {
    public:
        Asset() : m_Uuid(Uuid::Generate()) {}
        ~Asset() override = default;

        explicit Asset(const String& name) : Object(name), m_Uuid(Uuid::Generate()) {}

        virtual String GetAssetType() const = 0;

        void SetUuid(const Uuid& uuid) { m_Uuid = uuid; }
        Uuid GetUuid() const { return m_Uuid; }

    private:
        Uuid m_Uuid;
    };
}
