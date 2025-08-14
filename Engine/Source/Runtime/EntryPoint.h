#pragma once

namespace Nova
{
#ifdef NOVA_CUSTOM_ENTRY_POINT
    extern "C" int GuardedMain(int argc, char** argv);
#else
    extern "C" class Application* CreateApplication();
#endif
}

#define NOVA_DEFINE_APPLICATION_CLASS(userClass) \
    namespace Nova \
    {\
        extern "C" ::Nova::Application* CreateApplication()\
        {\
            return new ::userClass();\
        }\
    }