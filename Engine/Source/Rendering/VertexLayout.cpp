#include "VertexLayout.h"

namespace Nova
{
    bool VertexAttribute::operator==(const VertexAttribute& Other) const
    {
        return name == Other.name && format == Other.format;
    }

    VertexLayout::VertexLayout(const Array<VertexAttribute>& attributes) : m_Attributes(attributes)
    {
        
    }

    void VertexLayout::AddAttribute(const VertexAttribute& attribute)
    {
        m_Attributes.Add(attribute);
    }

    size_t VertexLayout::Count() const
    {
        return m_Attributes.Count();
    }

    size_t VertexLayout::Stride() const
    {
        size_t Result = 0;
        for(const VertexAttribute& Attribute : m_Attributes)
            Result += GetFormatSize(Attribute.format);
        return Result;
    }

    size_t VertexLayout::GetOffset(const VertexAttribute& attribute) const
    {
        const Array<VertexAttribute>::SizeType Index = m_Attributes.Find(attribute);
        NOVA_ASSERT(Index != -1, "Invalid vertex attribute!");
        size_t Result = 0;
        for(Array<VertexAttribute>::SizeType i = 0; i < Index; i++)
        {
            Result += GetFormatSize(m_Attributes[i].format);
        }
        return Result;
    }

    const Array<VertexAttribute>& VertexLayout::GetAttributes() const
    {
        return m_Attributes;
    }

    VertexAttribute& VertexLayout::operator[](size_t index)
    {
        return m_Attributes[index];
    }

    const VertexAttribute& VertexLayout::operator[](size_t index) const
    {
        return m_Attributes[index];
    }
}
