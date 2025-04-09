#include "VertexBufferLayout.h"
#include "VertexArray.h"

namespace Nova
{
    VertexBufferLayout VertexBufferLayout::Default = {
        {
           { "Position", VertexAttributeType::Vector3 },
           { "TexCoord", VertexAttributeType::Vector2 },
           { "Normal",   VertexAttributeType::Vector3 },
           { "Color",    VertexAttributeType::Vector4 }
        }
    };

    bool VertexAttribute::operator==(const VertexAttribute& Other) const
    {
        return Name == Other.Name && Type == Other.Type;
    }

    u32 GetNumComponents(VertexAttributeType Type)
    {
        switch (Type)
        {
        case VertexAttributeType::Float: return 1;
        case VertexAttributeType::Vector2: return 2;
        case VertexAttributeType::Vector3: return 3;
        case VertexAttributeType::Vector4: return 4;
        case VertexAttributeType::Matrix4: return 4 * 4;
        }
        return 0;
    }

    VertexBufferLayout::VertexBufferLayout(const Array<VertexAttribute>& Attributes) : m_Attributes(Attributes)
    {
        
    }

    void VertexBufferLayout::AddAttribute(const VertexAttribute& Attribute)
    {
        m_Attributes.Add(Attribute);
    }

    void VertexBufferLayout::Apply(VertexArray* Vao) const
    {
        Vao->SetBufferLayout(*this);
    }

    size_t VertexBufferLayout::Count() const
    {
        return m_Attributes.Count();
    }

    size_t VertexBufferLayout::Stride() const
    {
        size_t Result = 0;
        for(auto& Attribute : m_Attributes)
        {
            switch(Attribute.Type)
            {
            case VertexAttributeType::Float:
                Result += sizeof(f32);
                break;
            case VertexAttributeType::Vector2:
                Result += 2 * sizeof(f32);
                break;
            case VertexAttributeType::Vector3:
                Result += 3 * sizeof(f32);
                break;
            case VertexAttributeType::Vector4:
                Result += 4 * sizeof(f32);
                break;
            case VertexAttributeType::Matrix4:
                Result += 4 * 4 * sizeof(f32);
                break;
            }
        }
        return Result;
    }

    size_t VertexBufferLayout::GetOffset(const VertexAttribute& Attribute) const
    {
        const Array<VertexAttribute>::SizeType Index = m_Attributes.Find(Attribute);
        size_t Result = 0;
        for(Array<VertexAttribute>::SizeType i = 0; i < Index; i++)
        {
            const VertexAttribute& Att = m_Attributes[i];
            Result += GetNumComponents(Att.Type) * sizeof(f32);
        }
        return Result;
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
