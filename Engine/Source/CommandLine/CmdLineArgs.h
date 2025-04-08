#pragma once
#include "Containers/Array.h"
#include "Containers/String.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_API CmdLineArgs : public Iterable<String>
    {
    public:
        explicit CmdLineArgs(const Array<String>& Arguments);
        explicit CmdLineArgs(const Array<const char*>& Arguments);
        Iterator<String> begin() override;
        Iterator<String> end() override;
        ConstIterator<String> begin() const override;
        ConstIterator<String> end() const override;
    private:
        Array<String> m_Arguments;
    };
}
