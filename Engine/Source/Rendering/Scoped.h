#pragma once

namespace Nova::Rendering
{
    template<class T>
    class Scoped
    {
    public:
        template<typename CreateInfo>
        Scoped(const CreateInfo& createInfo)
        {
            m_Object.Initialize(createInfo);
        }

        ~Scoped()
        {
            m_Object.Destroy();
        }

        T& operator*() { return m_Object; }
        const T& operator*() const { return m_Object; }
        T* operator->() { return &m_Object; }
        T* operator&() { return &m_Object; }
        const T* operator&() const { return &m_Object; }
    private:
        T m_Object;
    };
}