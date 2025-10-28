#pragma once
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Runtime/Format.h"

namespace Nova
{
    struct VertexAttribute
    {
        String name;
        Format format;
        bool operator==(const VertexAttribute&) const;
    };

    class VertexLayout
    {
    public:
        VertexLayout() = default;
        VertexLayout(const Array<VertexAttribute>& attributes);
        
        void AddAttribute(const VertexAttribute& attribute);
        
        size_t Count() const;
        size_t Stride() const;
        size_t GetOffset(const VertexAttribute& attribute) const;

        const Array<VertexAttribute>& GetAttributes() const;

        VertexAttribute& operator[](size_t index);
        const VertexAttribute& operator[](size_t index) const;
        
        Array<VertexAttribute>::Iterator begin() { return m_Attributes.begin(); }
        Array<VertexAttribute>::Iterator end() { return m_Attributes.end(); }
    private:
        Array<VertexAttribute> m_Attributes;
    };
}
