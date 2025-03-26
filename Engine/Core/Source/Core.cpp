#include "Core.h"
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>

bool Core::Initialize()
{
    Assimp::Importer importer;
    return glfwInit();
}

void Core::Shutdown()
{
    glfwTerminate();
}

bool Core::CreateMainWindow(const char* Title, int Width, int Height)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_MainWindow = glfwCreateWindow(Width, Height, Title, nullptr, nullptr);
    glfwMakeContextCurrent(m_MainWindow);
    glfwShowWindow(m_MainWindow);
    return m_MainWindow;
}

bool Core::MainWindowShouldClose()
{
    return glfwWindowShouldClose(m_MainWindow);
}

void Core::PollEvents()
{
    glfwPollEvents();
}

void Core::MainWindowPresent()
{
    glfwSwapBuffers(m_MainWindow);
}
