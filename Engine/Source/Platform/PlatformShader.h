#pragma once
#if defined(NOVA_PLATFORM_OPENGL)
#include "OpenGL/OpenGLShader.h"
#endif

#if defined(NOVA_PLATFORM_VULKAN)
#include "Vulkan/VulkanShader.h"
#endif

#if defined(NOVA_PLATFORM_DIRECTX)
#include "DirectX/DirectXShader.h"
#endif