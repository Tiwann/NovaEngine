#pragma once
#include "Containers/StringView.h"

#define NOVA_DECLARE_CLASS(className) \
public: \
static inline constexpr const Nova::RTTI::Class* StaticClass() \
{ \
return &m_StaticClass_Generated; \
} \
private: \
static constexpr Nova::RTTI::Class m_StaticClass_Generated{#className, nullptr}; \



#define NOVA_DECLARE_CLASS_WITH_PARENT(className, parentClass) \
public: \
static inline constexpr const Nova::RTTI::Class* StaticClass() \
{ \
return &m_StaticClass_Generated; \
} \
private: \
static constexpr Nova::RTTI::Class m_StaticClass_Generated{#className, parentClass::StaticClass()}; \


namespace Nova::RTTI
{
    class Class
    {
    public:
        Class() = delete;
        constexpr Class(StringView className, const Class* parentClass)
            : m_ClassName(className), m_ParentClass(parentClass){}

        Class(const Class&) = delete;
        Class(Class&&) = delete;
        Class& operator=(const Class&) = delete;
        Class& operator=(Class&&) = delete;
        virtual ~Class() = default;

        constexpr bool operator==(const Class& other) const
        {
            return this == &other;
        }

        constexpr bool IsA(const Class* other) const
        {
            const Class* current = this;
            while (current)
            {
                if (current == other)
                    return true;
                current = current->m_ParentClass;
            }
            return false;
        }

        template<typename T>
        constexpr bool IsA() const
        {
            return IsA(T::StaticClass());
        }

        constexpr const Class* Cast(const Class* other) const
        {
            return IsA(other) ? other : nullptr;
        }

        template<typename T>
        constexpr const Class* Cast() const
        {
            return Cast(T::StaticClass());
        }

        constexpr StringView GetName() const { return m_ClassName; }
        constexpr const Class* GetParent() const { return m_ParentClass; }


    private:
        StringView m_ClassName;
        const Class* m_ParentClass;
    };

    struct BaseTestClass
    {
        NOVA_DECLARE_CLASS(BaseTestClass)
    };

    struct DerivedTestClass : BaseTestClass
    {
        NOVA_DECLARE_CLASS_WITH_PARENT(DerivedTestClass, BaseTestClass)
    };

    constexpr bool RttiTestCheck()
    {
        const Class* derivedClass = DerivedTestClass::StaticClass();
        const Class* baseClass = BaseTestClass::StaticClass();
        return derivedClass->IsA(baseClass);
    };

    static_assert(RttiTestCheck(), "DerivedTestClass::StaticClass()");
}
