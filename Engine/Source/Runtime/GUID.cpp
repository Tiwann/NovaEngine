#include "GUID.h"

#include <iomanip>
#include <random>
#include <sstream>

namespace Nova
{
    namespace
    {
        std::random_device RandomDevice;
        std::mt19937_64 RandomEngine(RandomDevice());
        std::uniform_int_distribution<u64> UniformDistribution;
    }

    GUID GUID::Zero = { 0ULL, 0ULL };
    
    GUID::GUID()
    {
        m_Values[0] = UniformDistribution(RandomEngine);
        m_Values[1] = UniformDistribution(RandomEngine);
    }

    GUID::GUID(const GUID& Guid) : m_Values { Guid.m_Values[0], Guid.m_Values[1] }
    {
        
    }

    GUID::GUID(const u64 Low, const u64 High) : m_Values{ Low, High }
    {
    }

    const u64* GUID::GetValues() const
    {
        return m_Values;
    }

    /* From ChatGPT */
    String GUID::GetString() const
    {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        ss << std::setw(8) << (m_Values[0] >> 32) << "-";
        ss << std::setw(4) << ((m_Values[0] >> 16) & 0xFFFF) << "-";
        ss << std::setw(4) << (m_Values[0] & 0xFFFF) << "-";
        ss << std::setw(4) << ((m_Values[1] >> 48) & 0xFFFF) << "-";
        ss << std::setw(12) << (m_Values[1] & 0xFFFFFFFFFFFF);
        std::string Result = ss.str();
        std::ranges::transform(Result, Result.begin(), [](const char c) -> char { return (char)std::toupper(c); });
        return Result.data();
    }

    bool GUID::operator==(const GUID& Other) const
    {
        return m_Values[0] == Other.m_Values[0] && m_Values[1] == Other.m_Values[1];
    }
}
