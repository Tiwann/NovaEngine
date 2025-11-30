#pragma once
#include "Runtime/Object.h"
#include "Containers/StringFormat.h"

#include <type_traits>

namespace Nova
{
    template<class ObjectType> requires std::is_base_of_v<Object, ObjectType>
    class Editor : public Object
    {
    public:
        explicit Editor(ObjectType* target)
        : Object(StringFormat("{} Editor", target->GetObjectName())), m_Target(target){}

        ~Editor() override = default;

        virtual void OnInit() {}
        virtual void OnGui() {}
        virtual void OnDestroy() {}
    protected:
        ObjectType* m_Target = nullptr;
    };
}
