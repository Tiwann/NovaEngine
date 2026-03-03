#pragma once
#include "Containers/Array.h"
#include "Containers/String.h"
#include "ShaderDataType.h"
#include "VertexInputRate.h"
#include "Containers/Map.h"

namespace Nova
{
    struct VertexAttribute
    {
        String name;
        ShaderDataType type;
        uint32_t binding;
        bool operator==(const VertexAttribute&) const;
    };

    class VertexLayout
    {
    public:
        VertexLayout() = default;

        void AddInputBinding(uint32_t binding, VertexInputRate inputRateBinding);
        void AddInputAttribute(const VertexAttribute& attribute);
        void AddInputAttribute(const String& name, ShaderDataType type, uint32_t binding);

        uint32_t GetStride(uint32_t binding) const;
        uint32_t GetAttributeCount() const;
        uint32_t GetBindingCount() const;
        uint32_t GetAttributeOffset(const VertexAttribute& attribute) const;
        uint32_t GetAttributeOffset(const String& name) const;
        uint32_t GetAttributeOffset(uint32_t index) const;

        const Array<VertexAttribute>& GetInputAttributes() const;
        const Map<uint32_t, VertexInputRate>& GetInputBindings() const;
    private:
        Map<uint32_t, VertexInputRate> m_InputBindings;
        Array<VertexAttribute> m_InputAttributes;
    };
}
