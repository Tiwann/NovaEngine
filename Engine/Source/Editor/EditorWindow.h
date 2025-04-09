#pragma once
#include "Platform/PlatformImGui.h"
#include "Runtime/UUID.h"
#include "Containers/String.h"

namespace Nova
{
    class EditorWindow
    {
    public:
        EditorWindow(String Name);
        virtual ~EditorWindow() = default;
        virtual void OnInit(){}
        virtual void OnDestroy(){}
        virtual void OnUpdate(f32 Delta){}
        virtual void OnInspectorGUI(const ImGuiIO& IO){}

        bool* OpenedPtr();
        bool IsOpened() const;
    

    protected:
        String m_Name;
        bool m_Opened;
        UUID m_UUID;
    };
}
