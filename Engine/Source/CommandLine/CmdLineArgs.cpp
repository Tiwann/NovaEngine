#include "CmdLineArgs.h"


namespace Nova
{
    CmdLineArgs::CmdLineArgs(const Array<String>& Arguments) : m_Arguments(Arguments)
    {

    }

    CmdLineArgs::CmdLineArgs(const Array<const char*>& Arguments)
    {
        for (const char* Arg : Arguments)
        {
            m_Arguments.Add(String(Arg));
        }
    }

    Iterator<String> CmdLineArgs::begin()
    {
        return m_Arguments.begin();
    }

    Iterator<String> CmdLineArgs::end()
    {
        return m_Arguments.end();
    }

    ConstIterator<String> CmdLineArgs::begin() const
    {
        return m_Arguments.begin();
    }

    ConstIterator<String> CmdLineArgs::end() const
    {
        return m_Arguments.end();
    }
}
