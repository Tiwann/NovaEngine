#pragma once

namespace Nova
{
    enum class ResourceBindingType
    {
        Sampler,
        CombinedTextureSampler,
        SampledTexture,
        StorageTexture,
        UniformBuffer,
        StorageBuffer,
        InputAttachment,
        InlineUniformBlock,
        AccelerationStructure,
        StorageTexelBuffer,
        UniformTexelBuffer,
    };
}