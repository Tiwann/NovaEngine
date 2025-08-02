#pragma once


namespace Nova
{
    extern "C" int GuardedMain(int argc, char** argv);
    extern "C" class Application* CreateApplication();
    inline bool g_ApplicationRunning = false;
}