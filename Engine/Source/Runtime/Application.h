#pragma once

namespace Nova
{
    class Window;

    class Application
    {
    public:
        Application() = default;
        virtual ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        virtual void Initialize() = 0;
        virtual void Update() = 0;
        virtual void Destroy() = 0;
    protected:
        Window* m_Window = nullptr;
    };
}
