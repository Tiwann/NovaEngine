#include "Runtime/EntryPoint.h"
#include "Runtime/DesktopWindow.h"
#include "Containers/StringView.h"
#include "Containers/StringFormat.h"
#include "Audio/AudioClip.h"
#include "Audio/AudioSystem.h"

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
        windowCreateInfo.title = "Hello Audio";
        windowCreateInfo.width = 600;
        windowCreateInfo.height = 400;
        windowCreateInfo.show = true;

        DesktopWindow window;
        window.Initialize(windowCreateInfo);
        window.closeEvent.Bind([] { g_Running = false; });

        AudioSystem audioSystem;
        if (!audioSystem.Initialize(AudioSystemCreateInfo()))
            return EXIT_FAILURE;

        AudioClip* audioClip = audioSystem.CreateClipFromFile(GetAssetPath("Assets/dolce-prod-by-bryzn.mp3"));
        audioClip->SetObjectName("Dolce");
        audioSystem.PlayAudioClip(audioClip);

        while (g_Running)
        {
            window.PollEvents();
        }

        audioSystem.Destroy();
        window.Destroy();
        return 0;
    }
}
