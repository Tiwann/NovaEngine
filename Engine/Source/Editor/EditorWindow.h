#pragma once
#include "Runtime/Object.h"
#include "Runtime/Ref.h"

namespace Nova
{
    class EditorWindow : public Object
    {
    public:
        explicit EditorWindow(const String& name) : Object(name) { }


        virtual void OnInit() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnGui() {}
        virtual void OnDestroy() {}
        virtual void OnEnable() {}
        virtual void OnDisable() {}

        void Show() { m_Show = true; }
        void Hide() { m_Show = false; }

        template<typename EditorWindowType, typename... Args>
        static Ref<EditorWindowType> CreateWindow(Args&&... args)
        {
            Ref<EditorWindowType> editorWindow = MakeRef<EditorWindowType>(std::forward<Args>(args)...);
            editorWindow->OnInit();
            return editorWindow;
        }

    protected:
        bool m_Show = true;
    };
}
