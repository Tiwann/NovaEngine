#include "Uuid.h"
#include "Random.h"

namespace Nova
{

    Uuid Uuid::Zero = Uuid(0, 0);
    
    Uuid::Uuid()
    {
        m_Values[0] = Random::Integer<uint64_t>();
        m_Values[1] = Random::Integer<uint64_t>();
    }

    Uuid::Uuid(const Uuid& uuid) : m_Values { uuid.m_Values[0], uuid.m_Values[1] }
    {
        
    }

    Uuid::Uuid(const uint64_t low, const uint64_t high) : m_Values{ low, high }
    {
    }

    const uint64_t* Uuid::GetValues() const
    {
        return m_Values;
    }

    /* From ChatGPT */
    String Uuid::GetString() const
    {
        const uint8_t* data = reinterpret_cast<const uint8_t*>(m_Values);
        char str[37] = { 0 };
        std::snprintf(str, 37,
        "%02X%02X%02X%02X-"
        "%02X%02X-"
        "%02X%02X-"
        "%02X%02X-"
        "%02X%02X%02X%02X%02X%02X",
        data[0], data[1], data[2], data[3],
        data[4], data[5],
        data[6], data[7],
        data[8], data[9],
        data[10], data[11], data[12], data[13], data[14], data[15]);
        return String(str, 37);
    }

    bool Uuid::operator==(const Uuid& Other) const
    {
        return m_Values[0] == Other.m_Values[0] && m_Values[1] == Other.m_Values[1];
    }
}
