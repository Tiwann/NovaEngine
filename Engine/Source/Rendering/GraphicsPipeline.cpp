#include "GraphicsPipeline.h"

namespace Nova
{
    VertexInputState CreateInputStateFromVertexLayout(const VertexLayout& vertexLayout)
    {
        const Array<VertexAttribute>& vertexAttributes = vertexLayout.GetInputAttributes();
        const Map<uint32_t, VertexInputRate>& inputBindings = vertexLayout.GetInputBindings();

        Array<VertexInputBindingDesc> bindingDescs;
        Array<VertexInputAttributeDesc> attributeDescs;

        for (uint32_t i = 0; i < vertexLayout.GetBindingCount(); ++i)
        {
            const auto& inputBinding = inputBindings.GetAt(i);

            VertexInputBindingDesc bindingDesc;
            bindingDesc.binding = inputBinding.key;
            bindingDesc.inputRate = inputBinding.value;
            bindingDesc.stride = vertexLayout.GetStride(inputBinding.key);
            bindingDescs.Add(bindingDesc);
        }

        for (size_t i = 0; i < vertexLayout.GetAttributeCount(); i++)
        {
            const VertexAttribute& attribute = vertexAttributes[i];

            VertexInputAttributeDesc attributeDesc;
            attributeDesc.binding = attribute.binding;
            attributeDesc.location = i;
            attributeDesc.format = GetFormat(attribute.type);
            attributeDesc.offset = vertexLayout.GetAttributeOffset(i);
            attributeDescs.Add(attributeDesc);
        }

        return { bindingDescs, attributeDescs };
    }
}
