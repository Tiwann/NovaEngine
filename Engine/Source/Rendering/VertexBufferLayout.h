#pragma once
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Runtime/Format.h"

namespace Nova
{
    class VertexArray;
}

namespace Nova
{
    struct VertexAttribute
    {
        String Name;
        Format Format;
        bool operator==(const VertexAttribute&) const;
    };

    class VertexBufferLayout
    {
    public:
        VertexBufferLayout() = default;
        explicit VertexBufferLayout(const Array<VertexAttribute>& Attributes);
        
        void AddAttribute(const VertexAttribute& Attribute);
        
        size_t Count() const;
        size_t Stride() const;
        size_t GetOffset(const VertexAttribute& Attribute) const;

        const Array<VertexAttribute>& GetAttributes() const;

        VertexAttribute& operator[](size_t Index);
        const VertexAttribute& operator[](size_t Index) const;
        
        Array<VertexAttribute>::Iterator begin() { return m_Attributes.begin(); }
        Array<VertexAttribute>::Iterator end() { return m_Attributes.end(); }
    private:
        Array<VertexAttribute> m_Attributes;
    };
}
