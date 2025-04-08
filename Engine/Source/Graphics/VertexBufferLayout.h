#pragma once
#include "Containers/Array.h"
#include "Containers/String.h"

namespace Nova
{
    class VertexArray;
}

namespace Nova
{
    enum class VertexAttributeType
    {
        Float,
        Vector2,
        Vector3,
        Vector4,
        Matrix4
    };
    
    struct VertexAttribute
    {
        String Name;
        VertexAttributeType Type;
        bool operator==(const VertexAttribute&) const;
    };

    u32 GetNumComponents(VertexAttributeType Type);
    
    class VertexBufferLayout
    {
    public:
        VertexBufferLayout(const Array<VertexAttribute>& Attributes);
        
        void AddAttribute(const VertexAttribute& Attribute);
        void Apply(const Ref<VertexArray>& Vao) const;
        
        size_t Count() const;
        size_t Stride() const;
        size_t GetOffset(const VertexAttribute& Attribute) const;
        

        VertexAttribute& operator[](size_t Index);
        const VertexAttribute& operator[](size_t Index) const;
        
        Array<VertexAttribute>::Iterator begin() { return m_Attributes.begin(); }
        Array<VertexAttribute>::Iterator end() { return m_Attributes.end(); }

        static VertexBufferLayout Default;
    private:
        Array<VertexAttribute> m_Attributes;
    };
}
