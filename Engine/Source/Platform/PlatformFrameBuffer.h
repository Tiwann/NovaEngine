#pragma once

#if defined(NOVA_PLATFORM_OPENGL)
#include "OpenGL/OpenGLFrameBuffer.h"
#endif

#if defined(NOVA_PLATFORM_VULKAN)
#include "Vulkan/VulkanFrameBuffer.h"
#endif

#if defined(NOVA_PLATFORM_DIRECTX)
#include "DirectX/DirectXFrameBuffer.h"
#endif