#pragma once

namespace Nova
{
    template<typename Handle>
    class PhysicsShape
    {
    public:
        using HandleType = Handle;
        PhysicsShape() = default;
        PhysicsShape(const PhysicsShape& Other) : m_Shape(Other.m_Shape){}
        PhysicsShape(PhysicsShape&& Other) noexcept : m_Shape(Other.m_Shape){ Other.m_Shape = nullptr; }
        
        PhysicsShape& operator=(const PhysicsShape& Other)
        {
            if(this == &Other)
                return *this;
            m_Shape = Other.m_Shape;
            return *this;
        }
        PhysicsShape& operator=(PhysicsShape&& Other) noexcept
        {
            if(this == &Other)
                return *this;
            m_Shape = Other.m_Shape;
            Other.m_Shape = nullptr;
            return *this;
        }
        
        virtual ~PhysicsShape()
        {
            m_Shape = nullptr;
        }
        
        HandleType& GetHandle() { return *m_Shape; }
        const HandleType& GetHandle() const { return *m_Shape; }
    protected:
        HandleType* m_Shape = nullptr;
    };
}
