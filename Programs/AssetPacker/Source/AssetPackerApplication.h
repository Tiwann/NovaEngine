#pragma once
#include "Runtime/Application.h"

namespace Nova
{
    class AssetPackerApplication final : public Application
    {
    public:
        AssetPackerApplication(const int32_t argc, char** argv) : Application(argc, argv){}

        ApplicationConfiguration GetConfiguration() const override;
        void OnInit() override;
        void OnDestroy() override;
        DeviceType GetRenderDeviceType() const override;
    };
}
