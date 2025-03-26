#pragma once
#include "CoreExport.h"

class CORE_API Core
{
public:
    bool Initialize();
    void Shutdown();
    bool CreateMainWindow(const char* Title, int Width, int Height);

    bool MainWindowShouldClose();

    void PollEvents();
    void MainWindowPresent();
private:
    struct GLFWwindow* m_MainWindow = nullptr;
};
