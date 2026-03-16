#pragma once
#include "Resource.h"
#include "BufferUsage.h"
#include "ResourceState.h"
#include "Runtime/Ref.h"

namespace Nova
{
    class RenderDevice;

    struct BufferCreateInfo
    {
        RenderDevice* device;
        BufferUsage usage = BufferUsage::None;
        size_t size = 0;

        BufferCreateInfo& WithDevice(RenderDevice* inDevice) { device = inDevice; return *this; }
        BufferCreateInfo& WithUsage(const BufferUsage inUsage) { usage = inUsage; return *this; }
        BufferCreateInfo& WithSize(const size_t inSize) { size = inSize; return *this; }
    };

    class Buffer : public Resource
    {
    public:
        Buffer() : Resource() {}
        ~Buffer() override = default;

        virtual bool Initialize(const BufferCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        virtual void* Map() = 0;
        virtual void Unmap(const void* ptr) = 0;

        ResourceType GetResourceType() final { return ResourceType::Buffer; }
        ResourceState GetState() const { return m_State; }
        void SetState(const ResourceState state) { m_State = state; }
        BufferUsage GetUsage() const { return m_Usage; }
        size_t GetSize() const { return m_Size; }
    protected:
        size_t m_Size = 0;
        BufferUsage m_Usage = BufferUsage::None;
        ResourceState m_State = ResourceState::Undefined;
    };

    using BufferHandle = Ref<Buffer>;
}
