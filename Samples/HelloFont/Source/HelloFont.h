#pragma once
#include "Runtime/Application.h"
#include "Runtime/LogCategory.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(HelloFont, "HELLO FONT")

class HelloFont : public Nova::Application
{
public:
    explicit HelloFont(const Nova::Array<const char*>& Arguments);
    Nova::ApplicationConfiguration CreateConfiguration() const override;
    void OnInit() override;
};