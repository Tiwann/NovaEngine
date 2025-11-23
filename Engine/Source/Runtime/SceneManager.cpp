#include "SceneManager.h"
#include "Scene.h"
#include <print>

#include "Editor/Selection.h"

namespace Nova
{
    void SceneManager::OnUpdate(const float deltaTime)
    {
        if (m_PendingScene)
        {
            if (m_ActiveScene)
            {
                m_ActiveScene->OnDestroy();
                delete m_ActiveScene;
            }

            m_ActiveScene = m_PendingScene;
            m_PendingScene = nullptr;
            Selection::SelectedScene = m_ActiveScene;
        }

        if (m_ActiveScene)
            m_ActiveScene->OnUpdate(deltaTime);
    }

    void SceneManager::OnPreRender(CommandBuffer& cmdBuffer)
    {
        if (m_ActiveScene)
            m_ActiveScene->OnPreRender(cmdBuffer);
    }

    void SceneManager::OnRender(CommandBuffer& cmdBuffer)
    {
        if (m_ActiveScene)
            m_ActiveScene->OnRender(cmdBuffer);
    }

    void SceneManager::OnDrawDebug()
    {
        if (m_ActiveScene)
            m_ActiveScene->OnDrawDebug();
    }

    void SceneManager::Destroy()
    {
        UnloadActiveScene();
    }

    void SceneManager::LoadScene(Scene* newScene)
    {
        if (m_PendingScene)
        {
            std::println(std::cerr, "A scene is already pending to be loaded as active scene!");
            return;
        }

        if (newScene != m_PendingScene)
            m_PendingScene = newScene;
    }

    void SceneManager::UnloadActiveScene()
    {
        if (m_ActiveScene)
        {
            m_ActiveScene->OnDestroy();
            delete m_ActiveScene;
            m_ActiveScene = nullptr;
        }
    }

    Scene* SceneManager::GetActiveScene() const
    {
        return m_ActiveScene;
    }

    Scene* SceneManager::CreateScene(Application* owner, const String& name) const
    {
        Scene* newScene = new Scene(owner, name);
        return newScene;
    }

    Scene* SceneManager::CreateSceneAndSetActive(Application* owner, const String& name)
    {
        Scene* newScene = new Scene(owner, name);
        newScene->OnInit();
        LoadScene(newScene);
        return newScene;
    }
}
