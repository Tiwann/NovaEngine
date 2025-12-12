#include "AudioNode.h"
#include "AudioSystem.h"

namespace Nova
{
    bool AudioNode::Initialize(AudioSystem* system)
    {
        if (!system) return false;

        ma_engine* engine = system->GetHandle();
        if (!engine) return false;

        ma_node_graph* graph = &engine->nodeGraph;
        if (!graph) return false;

        m_Vtable.onProcess = [](ma_node* pNode, const float** inFrames, uint32_t* inFrameCount, float** outFrames, uint32_t* outFrameCount)
        {
            if (!inFrames || !inFrameCount || !outFrames || !outFrameCount) return;
            if (!*inFrames) return;
            if (!*outFrames) return;

            ma_node_base_custom* nodeCustom = (ma_node_base_custom*)pNode;
            AudioNode* node = (AudioNode*)nodeCustom->pUserData;
            if (!node) return;

            node->OnProcess(inFrames, *inFrameCount, outFrames, *outFrameCount);
        };

        m_Vtable.inputBusCount = GetInputBusCount();
        m_Vtable.outputBusCount = GetOutputBusCount();
        m_Vtable.flags = GetNodeFlags();

        ma_node_config nodeConfig = ma_node_config_init();
        const uint32_t channels = system->GetOutputChannelCount();
        nodeConfig.vtable = &m_Vtable;
        nodeConfig.initialState = ma_node_state_started;
        nodeConfig.pInputChannels = &channels;
        nodeConfig.pOutputChannels = &channels;

        ma_node_detach_all_output_buses(&m_NodeBase);
        ma_node_uninit(&m_NodeBase, nullptr);
        const ma_result result = ma_node_init(graph, &nodeConfig, nullptr, &m_NodeBase);
        if (result != MA_SUCCESS) return false;
        m_NodeBase.pUserData = this;
        return true;
    }

    void AudioNode::Destroy()
    {
        ma_node_detach_all_output_buses(&m_NodeBase);
        ma_node_uninit(&m_NodeBase, nullptr);
    }

    ma_node_base* AudioNode::GetHandle()
    {
        return (ma_node_base*)&m_NodeBase;
    }

    const ma_node_base* AudioNode::GetHandle() const
    {
        return (ma_node_base*)&m_NodeBase;
    }
}
