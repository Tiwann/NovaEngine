#pragma once

#if defined(NOVA_PLATFORM_OPENGL)
#include "OpenGL/OpenGLImGuiRenderer.h"
#elif defined(NOVA_PLATFORM_VULKAN)
#include "Vulkan/VulkanImGuiRenderer.h"
#elif defined(NOVA_PLATFORM_DIRECTX)
#include "DirectX/DirectXImGuiRenderer.h"
#endif