#include "Runtime/EntryPoint.h"
#include "Runtime/DesktopWindow.h"
#include "Audio/AudioSystem.h"
#include "Rendering/Vulkan/Device.h"
#include "Rendering/Vulkan/RenderTarget.h"

#include <cstdlib>


static bool g_Running = true;

namespace Nova
{
    int GuardedMain(int, char**)
    {
        WindowCreateInfo windowCreateInfo;
        windowCreateInfo.title = "Hello Triangle";
        windowCreateInfo.width = 1600;
        windowCreateInfo.height = 900;
        windowCreateInfo.show = true;

        DesktopWindow window;
        if (!window.Initialize(windowCreateInfo))
            return EXIT_FAILURE;

        window.closeEvent.Bind([] { g_Running = false; });


        Rendering::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.applicationName = "Hello Triangle";
        deviceCreateInfo.versionMajor = 1;
        deviceCreateInfo.versionMinor = 0;
        deviceCreateInfo.window = &window;

        Vulkan::Device device;
        if (!device.Initialize(deviceCreateInfo))
            return EXIT_FAILURE;

        Rendering::RenderTargetCreateInfo renderTargetCreateInfo;
        renderTargetCreateInfo.device = &device;
        renderTargetCreateInfo.width = window.GetWidth();
        renderTargetCreateInfo.height = window.GetHeight();
        renderTargetCreateInfo.depth = 1;
        renderTargetCreateInfo.colorFormat = Format::R8G8B8A8_UNORM;
        renderTargetCreateInfo.depthFormat = Format::D32_FLOAT_S8_UINT;
        renderTargetCreateInfo.sampleCount = 1;

        Vulkan::RenderTarget renderTarget;
        if (!renderTarget.Initialize(renderTargetCreateInfo))
            return EXIT_FAILURE;

        window.maximizeEvent.Bind([&device]
        {
            Vulkan::Swapchain* swapchain = device.GetSwapchain();
            swapchain->Invalidate();
        });

        window.resizeEvent.Bind([&device](int32_t, int32_t)
        {
            Vulkan::Swapchain* swapchain = device.GetSwapchain();
            swapchain->Invalidate();
        });

        while (g_Running)
        {
            window.Update();

            if (device.BeginFrame())
            {
                Vulkan::CommandBuffer& commandBuffer = device.GetCurrentCommandBuffer();
                renderTarget.BeginRendering(commandBuffer);
                renderTarget.Clear(Color::Red);
                renderTarget.EndRendering();

                device.BlitRenderTarget(renderTarget, Filter::Nearest);
                device.EndFrame();
                device.Present();
            }
        }

        renderTarget.Destroy();
        device.Destroy();
        window.Destroy();
        return 0;
    }
}
