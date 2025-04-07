#pragma once


#if defined(NOVA_PLATFORM_OPENGL)
#include "OpenGL/OpenGLRenderer.h"
#endif

#if defined(NOVA_PLATFORM_VULKAN)
#include "Vulkan/VulkanRenderer.h"
#endif

#if defined(NOVA_PLATFORM_DIRECTX)
#include "DirectX/DirectXRenderer.h"
#endif
