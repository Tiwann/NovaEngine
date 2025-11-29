#pragma once
#include "Runtime/Object.h"
#include "Containers/MulticastDelegate.h"
#include <miniaudio.h>


namespace Nova
{
    class AudioSystem;

    class AudioNode : public Object
    {
    public:
        virtual bool Initialize(AudioSystem* system);
        virtual void Destroy();

        virtual uint32_t GetNodeFlags() const { return 0;}
        virtual uint32_t GetInputBusCount() const = 0;
        virtual uint32_t GetOutputBusCount() const = 0;

        virtual void OnProcess(const float** inFrames, uint32_t inFrameCount, float** ourFrames, uint32_t outFrameCount) = 0;
        virtual void OnAttachToOutputBus(){}
        virtual void OnAttachToInputBus(){}
        virtual void OnDetachFromInputBus(){}
        virtual void OnDetachFromOutputBus(){}

        ma_node_base* GetHandle();
        const ma_node_base* GetHandle() const;

    private:
        struct ma_node_base_custom
        {
            ma_node_base base;
            void* pUserData;
        };
        ma_node_base_custom m_NodeBase{};
        ma_node_vtable m_Vtable{};
    };
}
