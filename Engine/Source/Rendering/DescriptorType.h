#pragma once

namespace Nova
{
    enum class DescriptorType
    {
        Sampler,
        CombinedImageSampler,
        SampledImage,
        StorageImage,
        UniformBuffer,
        StorageBuffer,
        InputAttachment,
        InlineUniformBlock,
        AccelerationStructure,
        StorageTexelBuffer,
        UniformTexelBuffer,
    };
}