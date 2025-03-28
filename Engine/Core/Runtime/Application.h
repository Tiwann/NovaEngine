#pragma once
#include "Types.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_API Application
    {
    public:
        Application(i32 Argc, char** Argv);
    protected:
        virtual void OnInit();
        virtual void OnUpdate(f64 Delta);
        virtual void OnExit();
    private:

    };
}
