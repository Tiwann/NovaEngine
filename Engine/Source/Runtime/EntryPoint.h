#pragma once


namespace Nova
{
    extern "C" int GuardedMain(int argc, char** argv);
    inline bool g_ApplicationRunning = false;
}