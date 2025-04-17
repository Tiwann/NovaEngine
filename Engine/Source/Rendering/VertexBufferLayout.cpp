#include "VertexBufferLayout.h"
#include "VertexArray.h"

namespace Nova
{
    bool VertexAttribute::operator==(const VertexAttribute& Other) const
    {
        return Name == Other.Name && Format == Other.Format;
    }

    VertexBufferLayout::VertexBufferLayout(const Array<VertexAttribute>& Attributes) : m_Attributes(Attributes)
    {
        
    }

    void VertexBufferLayout::AddAttribute(const VertexAttribute& Attribute)
    {
        m_Attributes.Add(Attribute);
    }

    size_t VertexBufferLayout::Count() const
    {
        return m_Attributes.Count();
    }

    size_t VertexBufferLayout::Stride() const
    {
        size_t Result = 0;
        for(const VertexAttribute& Attribute : m_Attributes)
        {
            Result += GetFormatSize(Attribute.Format);
        }
        return Result;
    }

    size_t VertexBufferLayout::GetOffset(const VertexAttribute& Attribute) const
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

    const Array<VertexAttribute>& VertexBufferLayout::GetAttributes() const
    {
        return m_Attributes;
    }

    VertexAttribute& VertexBufferLayout::operator[](size_t Index)
    {
        return m_Attributes[Index];
    }

    const VertexAttribute& VertexBufferLayout::operator[](size_t Index) const
    {
        return m_Attributes[Index];
    }
}
