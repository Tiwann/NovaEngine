#include "VertexLayout.h"
#include "VertexArray.h"

namespace Nova
{
    bool VertexAttribute::operator==(const VertexAttribute& Other) const
    {
        return Name == Other.Name && Format == Other.Format;
    }

    VertexLayout::VertexLayout(const Array<VertexAttribute>& Attributes) : m_Attributes(Attributes)
    {
        
    }

    void VertexLayout::AddAttribute(const VertexAttribute& Attribute)
    {
        m_Attributes.Add(Attribute);
    }

    size_t VertexLayout::Count() const
    {
        return m_Attributes.Count();
    }

    size_t VertexLayout::Stride() const
    {
        size_t Result = 0;
        for(const VertexAttribute& Attribute : m_Attributes)
        {
            Result += GetFormatSize(Attribute.Format);
        }
        return Result;
    }

    size_t VertexLayout::GetOffset(const VertexAttribute& Attribute) const
    {
        const Array<VertexAttribute>::SizeType Index = m_Attributes.Find(Attribute);
        NOVA_ASSERT(Index != -1ULL, "invalid Vertex Attribute!")
        ;
        size_t Result = 0;
        for(Array<VertexAttribute>::SizeType i = 0; i < Index; i++)
        {
            Result += GetFormatSize(m_Attributes[i].Format);
        }
        return Result;
    }

    const Array<VertexAttribute>& VertexLayout::GetAttributes() const
    {
        return m_Attributes;
    }

    VertexAttribute& VertexLayout::operator[](size_t Index)
    {
        return m_Attributes[Index];
    }

    const VertexAttribute& VertexLayout::operator[](size_t Index) const
    {
        return m_Attributes[Index];
    }
}
