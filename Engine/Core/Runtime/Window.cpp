#include "Window.h"
#include "Image.h"
#include "Filesystem.h"
#include "Math/Vector2.h"
#include "ApplicationConfiguration.h"

#include <GLFW/glfw3.h>


namespace Nova
{
    Window::Window(const ApplicationConfiguration& Config)
    : m_Name(Config.AppName), m_Width(Config.WindowWidth), m_Height(Config.WindowHeight), m_Resizable(Config.WindowResizable)
    {
        glfwWindowHint(GLFW_RESIZABLE, m_Resizable);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#if defined(HYDRO_PLATFORM_OPENGL)
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_SAMPLES, 16);
#if defined(HYDRO_DEBUG)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
#else
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); 
#endif
        m_Handle = glfwCreateWindow((int)m_Width, (int)m_Height, *m_Name, nullptr, nullptr);
    }

    Window::~Window()
    {
        if(m_Handle)
        {
            glfwDestroyWindow(m_Handle);
            m_Handle = nullptr;
        }
    }
    
    void Window::Destroy()
    {
        if(m_Handle)
        {
            glfwDestroyWindow(m_Handle);
            m_Handle = nullptr;
        }
    }

    void Window::Show() const
    {
        glfwShowWindow(m_Handle);
    }

    void Window::Hide() const
    {
        glfwHideWindow(m_Handle);
    }


    const GLFWwindow* Window::GetNativeWindow() const
    {
        return m_Handle;
    }
    
    GLFWwindow* Window::GetNativeWindow()
    {
        return m_Handle;
    }
    
    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_Handle);
    }

    bool Window::IsValid() const
    {
        return m_Handle;
    }


    Vector2 Window::GetSize() const
    {
        return { GetWidth<f32>(), GetHeight<f32>() };
    }

    Vector2 Window::GetCenter() const
    {
        return { (f32)m_Width / 2, (f32)m_Height / 2 };
    }

    const String& Window::GetName() const
    {
        return m_Name;
    }

    void Window::SetIcon(const Path& Filepath) const
    {
        ScopedBuffer ImageData = File::ReadToBuffer(Filepath);
        const Image IconImage(ImageData.AsBuffer(), Format::R32G32B32A32_FLOAT);
        const GLFWimage GLFWImage{ (int)IconImage.GetWidth(), (int)IconImage.GetHeight(), (u8*)IconImage.GetData() };
        glfwSetWindowIcon(m_Handle, 1, &GLFWImage);
    }
    
    void Window::SetIcon(const Image* Image) const
    {
        const GLFWimage IconImage{ (int)Image->GetWidth(), (int)Image->GetHeight(), (u8*)Image->GetData() };
        glfwSetWindowIcon(m_Handle, 1, &IconImage);
    }
    

    bool Window::IsResizable() const
    {
        return m_Resizable;
    }
    
    void Window::SetName(String&& Name)
    {
        m_Name = std::move(Name);
        glfwSetWindowTitle(m_Handle, *m_Name);
    }

    void Window::ResetName() const
    {
        glfwSetWindowTitle(m_Handle, *m_Name);
    }
}
