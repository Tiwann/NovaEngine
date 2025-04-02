#pragma once

#if defined(NOVA_PLATFORM_OPENGL)
#include "OpenGL/OpenGLVertexArray.h"
#endif

#if defined(NOVA_PLATFORM_VULKAN)
#include "Vulkan/VulkanVertexArray.h"
#endif

#if defined(NOVA_PLATFORM_DIRECTX)
#include "DirectX/DirectXVertexArray.h"
#endif