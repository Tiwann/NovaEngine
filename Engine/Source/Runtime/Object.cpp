#include "Object.h"

namespace Nova
{
    Object::Object() : m_Name("New Object")
    {
        
    }

    Object::Object(String name) : m_Name(std::move(name))
    {
    }

    const String& Object::GetObjectName() const
    {
        return m_Name;
    }

    void Object::SetObjectName(const String& newName)
    {
        m_Name = newName;
    }
}
