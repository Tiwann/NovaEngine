#pragma once
#include "Rendering/Texture.h"

struct ID3D12Resource;
namespace D3D12MA { class Allocation; }
typedef D3D12MA::Allocation ID3D12Allocation;
typedef ID3D12Resource ID3D12Image;
typedef void ID3D12ImageView;

#define D3D12_TO_HANDLE(ptr) { reinterpret_cast<SIZE_T>((ptr)) }
#define D3D12_TO_PTR(handle) reinterpret_cast<void*>((handle))

namespace Nova::D3D12
{
    class Device;
    class RenderTarget;
    class Swapchain;

    class Texture final : public Nova::Texture
    {
    public:
        bool Initialize(const TextureCreateInfo& createInfo) override;
        void Destroy() override;
        bool IsValid() override;
        Array<uint8_t> GetPixels() override;

        const ID3D12Image* GetImage() const;
        ID3D12Image* GetImage();
        ID3D12ImageView* GetImageView() const;
    private:
        friend Swapchain;
        friend RenderTarget;

        Device* m_Device = nullptr;
        ID3D12Image* m_Image = nullptr;
        ID3D12ImageView* m_ImageView = nullptr;
        ID3D12Allocation* m_Allocation = nullptr;
    };
}
