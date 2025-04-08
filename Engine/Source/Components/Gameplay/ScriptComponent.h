#pragma once
#include "Runtime/Component.h"

namespace Nova
{
    struct Contact3D;
    struct Contact2D;

    class ScriptComponent : public Component
    {
    public:
        ScriptComponent(Entity* Owner, String Name) : Component(Owner, std::move(Name)) {}
    protected:
        void OnInit() override;
        void OnDestroy() override;

        virtual void OnContactBegin2D(const Contact2D& Contact) {}
        virtual void OnContactStay2D(const Contact2D& Contact) {}
        virtual void OnContactEnd2D(const Contact2D& Contact) {}

        virtual void OnTriggerBegin2D(const Contact2D& Contact) {}
        virtual void OnTriggerStay2D(const Contact2D& Contact) {}
        virtual void OnTriggerEnd2D(const Contact2D& Contact) {}
        
        virtual void OnContactBegin3D(const Contact3D& Contact) {}
        virtual void OnContactStay3D(const Contact3D& Contact) {}
        virtual void OnContactEnd3D(const Contact3D& Contact) {}

        virtual void OnTriggerBegin3D(const Contact3D& Contact) {}
        virtual void OnTriggerStay3D(const Contact3D& Contact) {}
        virtual void OnTriggerEnd3D(const Contact3D& Contact) {}
    };
}
