﻿#pragma once
#include "Object.h"


namespace Nova::Rendering { class CommandBuffer; }

namespace Nova
{
    class Application;
    class Scene;

    class SceneManager : public Object
    {
    public:
        SceneManager() = default;

        void OnUpdate(float deltaTime);
        void OnPreRender(Rendering::CommandBuffer& cmdBuffer);
        void OnRender(Rendering::CommandBuffer& cmdBuffer);
        void OnDrawDebug();
        void Destroy();

        void LoadScene(Scene* newScene);
        void UnloadActiveScene();
        Scene* GetActiveScene() const;
        Scene* CreateScene(Application* owner, const String& name) const;
        Scene* CreateSceneAndSetActive(Application* owner, const String& name);
    private:
        Scene* m_ActiveScene = nullptr;
        Scene* m_PendingScene = nullptr;
    };
}
