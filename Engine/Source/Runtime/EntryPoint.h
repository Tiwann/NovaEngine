#pragma once

namespace Nova
{
#ifdef NOVA_CUSTOM_ENTRY_POINT
    extern "C" int GuardedMain(int argc, char** argv);
#else
    extern "C" class Application* CreateApplication();
#endif
}