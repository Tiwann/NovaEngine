#include "VertexLayout.h"

namespace Nova
{
    bool VertexAttribute::operator==(const VertexAttribute& other) const
    {
        return name == other.name && type == other.type && binding == other.binding;
    }

    void VertexLayout::AddInputBinding(const uint32_t binding, const VertexInputRate inputRateBinding)
    {
        if (!m_InputBindings.FindKey(binding))
            m_InputBindings[binding] = inputRateBinding;
    }

    void VertexLayout::AddInputAttribute(const VertexAttribute& attribute)
    {
        m_InputAttributes.Add(attribute);
    }

    void VertexLayout::AddInputAttribute(const String& name, const ShaderDataType type, const uint32_t binding)
    {
        m_InputAttributes.Add(VertexAttribute(name, type, binding));
    }

    uint32_t VertexLayout::GetStride(const uint32_t binding) const
    {
        uint32_t result = 0;
        for(const VertexAttribute& attribute : m_InputAttributes)
        {
            if (attribute.binding == binding)
                result += GetDataTypeSize(attribute.type);
        }
        return result;
    }

    uint32_t VertexLayout::GetAttributeCount() const
    {
        return m_InputAttributes.Count();
    }

    uint32_t VertexLayout::GetBindingCount() const
    {
        return m_InputBindings.Count();
    }

    uint32_t VertexLayout::GetAttributeOffset(const VertexAttribute& attribute) const
    {
        const size_t index = m_InputAttributes.Find(attribute);
        NOVA_ASSERT(index != -1, "Invalid vertex attribute!");
        uint32_t result = 0;
        for(size_t i = 0; i < index; i++)
            result += GetDataTypeSize(m_InputAttributes[i].type);
        return result;
    }

    uint32_t VertexLayout::GetAttributeOffset(const String& name) const
    {
        const auto predicate = [&name](const VertexAttribute& attribute) { return attribute.name == name; };
        const VertexAttribute* attribute = m_InputAttributes.Single(predicate);
        NOVA_ASSERT(attribute, "Invalid vertex attribute!");
        const auto index = m_InputAttributes.Find(*attribute);
        uint32_t result = 0;
        for(size_t i = 0; i < index; i++)
            result += GetDataTypeSize(m_InputAttributes[i].type);
        return result;
    }

    uint32_t VertexLayout::GetAttributeOffset(const uint32_t index) const
    {
        NOVA_ASSERT(index < m_InputAttributes.Count(), "Invalid vertex attribute!");
        uint32_t result = 0;
        for(size_t i = 0; i < index; i++)
            result += GetDataTypeSize(m_InputAttributes[i].type);
        return result;
    }

    const Array<VertexAttribute>& VertexLayout::GetInputAttributes() const
    {
        return m_InputAttributes;
    }

    const Map<uint32_t, VertexInputRate>& VertexLayout::GetInputBindings() const
    {
        return m_InputBindings;
    }
}
