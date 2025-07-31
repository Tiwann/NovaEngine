#include "Runtime/EntryPoint.h"
#include "Runtime/DesktopWindow.h"
#include "Rendering/Vulkan/Device.h"
#include "Rendering/Vulkan/RenderTarget.h"
#include "Containers/StringView.h"
#include "Containers/StringFormat.h"
#include "Rendering/Vulkan/Conversions.h"
#include "Runtime/ScopedTimer.h"
#include <cstdlib>

#include "Assets/Sprite.h"
#include "Math/Vector3.h"
#include "Rendering/Renderer2D.h"

static constexpr uint32_t SAMPLE_COUNT = 1;
static bool g_Running = true;

namespace Nova
{
    static String GetAssetPath(const StringView filepath)
    {
        return StringFormat("{}/{}", APPLICATION_DIR, filepath);
    }

    int GuardedMain(int, char**)
    {
        WindowCreateInfo windowCreateInfo;
        windowCreateInfo.title = "Hello Texture";
        windowCreateInfo.width = 600;
        windowCreateInfo.height = 400;
        windowCreateInfo.show = true;

        DesktopWindow window;
        window.Initialize(windowCreateInfo);

        Rendering::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.applicationName = "Hello Texture";
        deviceCreateInfo.versionMajor = 1;
        deviceCreateInfo.versionMinor = 0;
        deviceCreateInfo.window = &window;

        window.closeEvent.Bind([] { g_Running = false; });

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
        renderTargetCreateInfo.sampleCount = SAMPLE_COUNT;

        Vulkan::RenderTarget renderTarget;
        if (!renderTarget.Initialize(renderTargetCreateInfo))
            return EXIT_FAILURE;

        window.maximizeEvent.Bind([&device]
        {
            Vulkan::Swapchain* swapchain = device.GetSwapchain();
            swapchain->Invalidate();
        });

        window.resizeEvent.Bind([&device, &renderTarget](const int32_t newX, const int32_t newY)
        {
            Vulkan::Swapchain* swapchain = device.GetSwapchain();
            swapchain->Invalidate();
            renderTarget.Resize(newX, newY);
        });

        const auto onTimer = [](const double deltaTime)
        {
            static double timer = 0.0f;
            timer += deltaTime;
            if (timer > 1.0)
            {
                const uint32_t fps = 1.0 / deltaTime;
                std::println(std::cout, "Frame Time: {:.2f}ms | FPS: {}", deltaTime * 1000.0, fps);
                timer = 0.0f;
            }
        };

        Rendering::Renderer2D renderer;

        while (g_Running)
        {
            const ScopedTimer timer(onTimer);

            window.PollEvents();

            if (device.BeginFrame())
            {
                if (renderer.BeginScene(nullptr))
                {
                    renderer.DrawSprite(Vector3::Zero, Sprite());
                    renderer.Initialize(device, renderTarget);
                    renderer.EndScene();
                    renderer.Render();
                }

                if constexpr (SAMPLE_COUNT > 1)
                {
                    device.ResolveToSwapchain(renderTarget);
                } else {
                    device.BlitToSwapchain(renderTarget, Filter::Nearest);
                }
                device.EndFrame();
                device.Present();
            }
        }

        renderer.Destroy();
        device.WaitIdle();
        renderTarget.Destroy();
        device.Destroy();
        window.Destroy();
        return 0;
    }
}
