#pragma once

#if defined(NOVA_PLATFORM_OPENGL)
#include "OpenGL/OpenGLIndexBuffer.h"
#endif

#if defined(NOVA_PLATFORM_VULKAN)
#include "Vulkan/VulkanIndexBuffer.h"
#endif

#if defined(NOVA_PLATFORM_DIRECTX)
#include "DirectX/DirectXIndexBuffer.h"
#endif