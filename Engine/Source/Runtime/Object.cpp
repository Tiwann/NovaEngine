#include "Object.h"

namespace Nova
{
    Object::Object() : m_Name("NewObject")
    {
        
    }

    Object::Object(String Name) : m_Name(std::move(Name))
    {
    }

    const String& Object::GetObjectName() const
    {
        return m_Name;
    }

    void Object::SetObjectName(const String& NewName)
    {
        m_Name = NewName;
    }
}
