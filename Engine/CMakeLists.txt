include(Vendor/Vendor.cmake)

set(NOVA_ENGINE_SOURCES
        Source/AssetImporters/ASTCImporter.cpp
        Source/AssetImporters/ASTCImporter.h
        Source/AssetImporters/ASTCImporter.cpp
        Source/AssetImporters/DDSImporter.cpp
        Source/AssetImporters/DDSImporter.h
        Source/AssetImporters/TextureImporter.cpp
        Source/AssetImporters/TextureImporter.h

        Source/Audio/AudioClip.cpp
        Source/Audio/AudioClip.h
        Source/Audio/AudioFormat.h
        Source/Audio/AudioLoader.h
        Source/Audio/AudioNode.cpp
        Source/Audio/AudioNode.h
        Source/Audio/AudioSystem.cpp
        Source/Audio/AudioSystem.h
        Source/Audio/FFTAudioNode.cpp
        Source/Audio/FFTAudioNode.h

        Source/Components/Rendering/AmbientLight.cpp
        Source/Components/Rendering/LightComponent.h
        Source/Components/Physics/PlaneComponent2D.h
        Source/Components/Rendering/PointLight.h
        Source/Components/Rendering/LightComponent.cpp
        Source/Components/Rendering/DirectionalLight.cpp
        Source/Components/Rendering/StaticMeshRenderer.cpp
        Source/Components/Rendering/SpriteRenderer.h
        Source/Components/Rendering/StaticMeshRenderer.h
        Source/Components/Physics/BoxComponent2D.h
        Source/Components/Physics/BoxComponent2D.cpp
        Source/Components/Physics/PhysicsComponent.h
        Source/Components/Rendering/PointLight.cpp
        Source/Components/Rendering/DirectionalLight.h
        Source/Components/Rendering/AmbientLight.h
        Source/Components/Audio/AudioListener.cpp
        Source/Components/Physics/PlaneComponent2D.cpp
        Source/Components/Physics/PhysicsComponent.cpp
        Source/Components/Rendering/SpriteRenderer.cpp
        Source/Components/Audio/AudioListener.h
        Source/Components/Audio/AudioSource.cpp
        Source/Components/Audio/AudioSource.h
        Source/Components/Camera.cpp
        Source/Components/Camera.h
        Source/Components/Transform.cpp
        Source/Components/Transform.h

        Source/Containers/Array.h
        Source/Containers/ArrayView.h
        Source/Containers/BufferView.h
        Source/Containers/BumpAllocator.h
        Source/Containers/Function.h
        Source/Containers/Lazy.h
        Source/Containers/Map.h
        Source/Containers/MulticastDelegate.h
        Source/Containers/Pair.h
        Source/Containers/Queue.h
        Source/Containers/StaticArray.h
        Source/Containers/Std140Buffer.cpp
        Source/Containers/Std140Buffer.h
        Source/Containers/String.h
        Source/Containers/StringConversion.h
        Source/Containers/StringFormat.cpp
        Source/Containers/StringFormat.h
        Source/Containers/StringView.h
        Source/Containers/TreeNode.h

        Source/Editor/Components/ComponentEditor.h
        Source/Editor/Components/PhysicsComponentEditor.cpp
        Source/Editor/Components/PhysicsComponentEditor.h
        Source/Editor/Editor.h
        Source/Editor/EditorWindow.h
        Source/Editor/HierarchyWindow.cpp
        Source/Editor/HierarchyWindow.h
        Source/Editor/InspectorWindow.cpp
        Source/Editor/InspectorWindow.h
        Source/Editor/Selection.h
        Source/Editor/ViewportWindow.cpp
        Source/Editor/ViewportWindow.h

        Source/External/arena.c
        Source/External/arena.h
        Source/External/ImGuiExtension.cpp
        Source/External/ImGuiExtension.h
        Source/External/stb_image.cpp
        Source/External/stb_image.h
        Source/External/stb_image_resize2.h
        Source/External/stb_image_write.h
        Source/External/stb_vorbis.c

        Source/Input/CursorMode.h
        Source/Input/GamepadButton.h
        Source/Input/GamepadThumbstick.h
        Source/Input/Input.cpp
        Source/Input/Input.h
        Source/Input/InputState.h
        Source/Input/KeyCode.h
        Source/Input/MouseButton.h

        Source/IO/ArrayStream.cpp
        Source/IO/ArrayStream.h
        Source/IO/AssetPack.cpp
        Source/IO/AssetPack.h
        Source/IO/AssetPack.hxx
        Source/IO/FileStream.cpp
        Source/IO/FileStream.h
        Source/IO/MemoryStream.cpp
        Source/IO/MemoryStream.h
        Source/IO/OpenMode.h
        Source/IO/Seek.h
        Source/IO/Stream.cpp
        Source/IO/Stream.h

        Source/Math/Functions.cpp
        Source/Math/Functions.h
        Source/Math/LinearAlgebra.h
        Source/Math/LineraAlgebra.cpp
        Source/Math/MathConversions.h
        Source/Math/Matrix2.cpp
        Source/Math/Matrix2.h
        Source/Math/Matrix3.cpp
        Source/Math/Matrix3.h
        Source/Math/Matrix3x4.cpp
        Source/Math/Matrix3x4.h
        Source/Math/Matrix4.cpp
        Source/Math/Matrix4.h
        Source/Math/Quaternion.cpp
        Source/Math/Quaternion.h
        Source/Math/Ray.h
        Source/Math/Rect.h
        Source/Math/Vector2.cpp
        Source/Math/Vector2.h
        Source/Math/Vector3.cpp
        Source/Math/Vector3.h
        Source/Math/Vector4.cpp
        Source/Math/Vector4.h

        Source/Physics/Box2DHelpers.h
        Source/Physics/BoxShape2D.cpp
        Source/Physics/BoxShape2D.h
        Source/Physics/CapsuleShape2D.cpp
        Source/Physics/CapsuleShape2D.h
        Source/Physics/CircleShape2D.cpp
        Source/Physics/CircleShape2D.h
        Source/Physics/PhysicsBody.h
        Source/Physics/PhysicsBody2D.cpp
        Source/Physics/PhysicsBody2D.h
        Source/Physics/PhysicsBodyDefinition.h
        Source/Physics/PhysicsBodyType.h
        Source/Physics/PhysicsConstraints.h
        Source/Physics/PhysicsContactInfo.h
        Source/Physics/PhysicsMaterial.cpp
        Source/Physics/PhysicsMaterial.h
        Source/Physics/PhysicsShape.h
        Source/Physics/PhysicsShape2D.cpp
        Source/Physics/PhysicsShape2D.h
        Source/Physics/PhysicsWorld.h
        Source/Physics/PhysicsWorld2D.cpp
        Source/Physics/PhysicsWorld2D.h
        Source/Physics/PlaneShape2D.cpp
        Source/Physics/PlaneShape2D.h

        Source/Rendering/BindingType.h
        Source/Rendering/BlendFactor.h
        Source/Rendering/BlendFunction.h
        Source/Rendering/BlendOperation.h
        Source/Rendering/BlitRegion.h
        Source/Rendering/Buffer.h
        Source/Rendering/ColorChannel.h
        Source/Rendering/ColorSpace.h
        Source/Rendering/CommandBuffer.h
        Source/Rendering/CommandPool.h
        Source/Rendering/CompareOperation.h
        Source/Rendering/ComputePipeline.h
        Source/Rendering/CullMode.h
        Source/Rendering/DebugRenderer.cpp
        Source/Rendering/DebugRenderer.h
        Source/Rendering/DescriptorPool.h
        Source/Rendering/Device.cpp
        Source/Rendering/Device.h
        Source/Rendering/Fence.h
        Source/Rendering/Filter.h
        Source/Rendering/FrontFace.h
        Source/Rendering/GraphicsPipeline.h
        Source/Rendering/ImGuiRenderer.cpp
        Source/Rendering/ImGuiRenderer.h
        Source/Rendering/LoadOperation.h
        Source/Rendering/Material.h
        Source/Rendering/PolygonMode.h
        Source/Rendering/PresentMode.h
        Source/Rendering/PrimitiveTopology.h
        Source/Rendering/Queue.h
        Source/Rendering/RenderPass.cpp
        Source/Rendering/RenderPass.h
        Source/Rendering/RenderTarget.cpp
        Source/Rendering/RenderTarget.h
        Source/Rendering/ResolveMode.h
        Source/Rendering/ResourceState.h
        Source/Rendering/Sampler.h
        Source/Rendering/SamplerAddressMode.h
        Source/Rendering/Scoped.h
        Source/Rendering/Semaphore.h
        Source/Rendering/Shader.h
        Source/Rendering/ShaderBindingSet.h
        Source/Rendering/ShaderBindingSetLayout.cpp
        Source/Rendering/ShaderBindingSetLayout.h
        Source/Rendering/ShaderEntryPoint.h
        Source/Rendering/ShaderModule.h
        Source/Rendering/ShaderModuleInfo.h
        Source/Rendering/ShaderPushConstantRange.h
        Source/Rendering/ShaderResourceManager.cpp
        Source/Rendering/ShaderResourceManager.h
        Source/Rendering/ShaderStage.h
        Source/Rendering/ShaderTarget.h
        Source/Rendering/ShadingLanguage.h
        Source/Rendering/SpriteRendererFlags.h
        Source/Rendering/StoreOperation.h
        Source/Rendering/Surface.cpp
        Source/Rendering/Surface.h
        Source/Rendering/Swapchain.cpp
        Source/Rendering/Swapchain.h
        Source/Rendering/SwpchainBuffering.h
        Source/Rendering/Texture.h
        Source/Rendering/TextureDimension.h
        Source/Rendering/Vertex.h
        Source/Rendering/VertexLayout.cpp
        Source/Rendering/VertexLayout.h
        Source/Rendering/Null/Device.cpp
        Source/Rendering/Null/Device.h
        Source/Rendering/ResourceBarrier.h
        Source/Rendering/ResourceBarrier.cpp
        Source/Rendering/SpriteBatchRenderer.cpp
        Source/Rendering/SpriteBatchRenderer.h
        Source/Rendering/SamplerManager.cpp
        Source/Rendering/SamplerManager.h

        Source/Runtime/Application.cpp
        Source/Runtime/Application.h
        Source/Runtime/ArgumentParser.cpp
        Source/Runtime/ArgumentParser.h
        Source/Runtime/Assertion.cpp
        Source/Runtime/Assertion.h
        Source/Runtime/Asset.h
        Source/Runtime/AssetDatabase.cpp
        Source/Runtime/AssetDatabase.h
        Source/Runtime/AssetType.h
        Source/Runtime/CmdLineArgs.h
        Source/Runtime/Color.cpp
        Source/Runtime/Color.h
        Source/Runtime/CommandLineOption.h
        Source/Runtime/Component.cpp
        Source/Runtime/Component.h
        Source/Runtime/DesktopWindow.cpp
        Source/Runtime/DesktopWindow.h
        Source/Runtime/DialogFilters.cpp
        Source/Runtime/DialogFilters.h
        Source/Runtime/Entity.cpp
        Source/Runtime/Entity.h
        Source/Runtime/EntityHandle.cpp
        Source/Runtime/EntityHandle.h
        Source/Runtime/EntryPoint.cpp
        Source/Runtime/EntryPoint.h
        Source/Runtime/Export.h
        Source/Runtime/FileUtils.cpp
        Source/Runtime/FileUtils.h
        Source/Runtime/Flags.h
        Source/Runtime/Format.h
        Source/Runtime/Iterator.h
        Source/Runtime/Memory.h
        Source/Runtime/Object.cpp
        Source/Runtime/Object.h
        Source/Runtime/Path.h
        Source/Runtime/Path.cpp
        Source/Runtime/Random.cpp
        Source/Runtime/Random.h
        Source/Runtime/Ref.h
        Source/Runtime/RefObject.h
        Source/Runtime/Scene.cpp
        Source/Runtime/Scene.h
        Source/Runtime/SceneManager.cpp
        Source/Runtime/SceneManager.h
        Source/Runtime/ScopedTimer.cpp
        Source/Runtime/ScopedTimer.h
        Source/Runtime/Sprite.h
        Source/Runtime/SpriteAnimation.cpp
        Source/Runtime/SpriteAnimation.h
        Source/Runtime/SpriteSheet.h
        Source/Runtime/StaticMesh.cpp
        Source/Runtime/StaticMesh.h
        Source/Runtime/Time.cpp
        Source/Runtime/Time.h
        Source/Runtime/Timer.cpp
        Source/Runtime/Timer.h
        Source/Runtime/TypeTraits.h
        Source/Runtime/Uuid.cpp
        Source/Runtime/Uuid.h
        Source/Runtime/Version.h
        Source/Runtime/Window.cpp
        Source/Runtime/Window.h
        Source/Runtime/Log.cpp
        Source/Runtime/Log.h
        Source/Runtime/LogCategory.h
        Source/Runtime/LogVerbosity.h
        Source/Runtime/Logger.cpp
        Source/Runtime/Logger.h
        Source/Runtime/Logger.h

        Source/Utils/BufferUtils.cpp
        Source/Utils/BufferUtils.h
        Source/Utils/ShaderUtils.cpp
        Source/Utils/ShaderUtils.h
        Source/Utils/TextureUtils.cpp
        Source/Utils/TextureUtils.h
)

set(NOVA_ENGINE_WINDOWS_SOURCES
        Source/Platform/Windows/FileDialog.cpp
        Source/Platform/Windows/Filesystem.cpp
        Source/Platform/Windows/StringConversions.cpp
)

set(NOVA_ENGINE_LINUX_SOURCES
)

set(NOVA_ENGINE_MACOS_SOURCES
)

set(NOVA_ENGINE_D3D12_SOURCES
        Source/Rendering/D3D12/Buffer.cpp
        Source/Rendering/D3D12/Buffer.h
        Source/Rendering/D3D12/CommandBuffer.cpp
        Source/Rendering/D3D12/CommandBuffer.h
        Source/Rendering/D3D12/CommandPool.cpp
        Source/Rendering/D3D12/CommandPool.h
        Source/Rendering/D3D12/Conversions.h
        Source/Rendering/D3D12/Device.cpp
        Source/Rendering/D3D12/Device.h
        Source/Rendering/D3D12/Fence.cpp
        Source/Rendering/D3D12/Fence.h
        Source/Rendering/D3D12/ImGuiRenderer.cpp
        Source/Rendering/D3D12/ImGuiRenderer.h
        Source/Rendering/D3D12/Queue.cpp
        Source/Rendering/D3D12/Queue.h
        Source/Rendering/D3D12/RenderTarget.cpp
        Source/Rendering/D3D12/RenderTarget.h
        Source/Rendering/D3D12/Surface.cpp
        Source/Rendering/D3D12/Surface.h
        Source/Rendering/D3D12/Swapchain.cpp
        Source/Rendering/D3D12/Swapchain.h
        Source/Rendering/D3D12/Texture.cpp
        Source/Rendering/D3D12/Texture.h
)

set(NOVA_ENGINE_VULKAN_SOURCES
        Source/External/vk_mem_alloc.cpp
        Source/Rendering/Vulkan/Buffer.cpp
        Source/Rendering/Vulkan/Buffer.h
        Source/Rendering/Vulkan/CommandBuffer.cpp
        Source/Rendering/Vulkan/CommandBuffer.h
        Source/Rendering/Vulkan/CommandPool.cpp
        Source/Rendering/Vulkan/CommandPool.h
        Source/Rendering/Vulkan/ComputePipeline.cpp
        Source/Rendering/Vulkan/ComputePipeline.h
        Source/Rendering/Vulkan/Conversions.h
        Source/Rendering/Vulkan/DescriptorPool.cpp
        Source/Rendering/Vulkan/DescriptorPool.h
        Source/Rendering/Vulkan/Device.cpp
        Source/Rendering/Vulkan/Device.h
        Source/Rendering/Vulkan/Fence.cpp
        Source/Rendering/Vulkan/Fence.h
        Source/Rendering/Vulkan/GraphicsPipeline.cpp
        Source/Rendering/Vulkan/GraphicsPipeline.h
        Source/Rendering/Vulkan/ImGuiRenderer.cpp
        Source/Rendering/Vulkan/ImGuiRenderer.h
        Source/Rendering/Vulkan/Material.cpp
        Source/Rendering/Vulkan/Material.h
        Source/Rendering/Vulkan/Queue.cpp
        Source/Rendering/Vulkan/Queue.h
        Source/Rendering/Vulkan/RenderTarget.cpp
        Source/Rendering/Vulkan/RenderTarget.h
        Source/Rendering/Vulkan/Sampler.cpp
        Source/Rendering/Vulkan/Sampler.h
        Source/Rendering/Vulkan/Semaphore.cpp
        Source/Rendering/Vulkan/Semaphore.h
        Source/Rendering/Vulkan/Shader.cpp
        Source/Rendering/Vulkan/Shader.h
        Source/Rendering/Vulkan/ShaderBindingSet.cpp
        Source/Rendering/Vulkan/ShaderBindingSet.h
        Source/Rendering/Vulkan/ShaderBindingSetLayout.cpp
        Source/Rendering/Vulkan/ShaderBindingSetLayout.h
        Source/Rendering/Vulkan/ShaderModule.cpp
        Source/Rendering/Vulkan/ShaderModule.h
        Source/Rendering/Vulkan/ShaderResourceManager.cpp
        Source/Rendering/Vulkan/ShaderResourceManager.h
        Source/Rendering/Vulkan/Surface.cpp
        Source/Rendering/Vulkan/Surface.h
        Source/Rendering/Vulkan/Swapchain.cpp
        Source/Rendering/Vulkan/Swapchain.h
        Source/Rendering/Vulkan/Texture.cpp
        Source/Rendering/Vulkan/Texture.h
        Source/Rendering/Vulkan/VulkanExtensions.cpp
        Source/Rendering/Vulkan/VulkanExtensions.h
        Source/Rendering/Vulkan/VulkanUtils.cpp
        Source/Rendering/Vulkan/VulkanUtils.h
        Source/Utils/VulkanUtils.h
)

set(NOVA_ENGINE_OPENGL_SOURCES
        Source/External/glad/src/glad.c
        Source/External/glad/include/glad/glad.h
        Source/External/glad/include/khr/khrplatform.h
        Source/Rendering/OpenGL/Buffer.cpp
        Source/Rendering/OpenGL/Buffer.h
        Source/Rendering/OpenGL/Device.cpp
        Source/Rendering/OpenGL/Device.h
        Source/Rendering/OpenGL/Texture.cpp
        Source/Rendering/OpenGL/Texture.h
)


add_library(NovaEngine STATIC)
set_target_properties(NovaEngine PROPERTIES CXX_STANDARD 23)
target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_SOURCES})
target_link_options(NovaEngine PRIVATE /NATVIS:NovaEngine.natvis)
target_include_directories(NovaEngine PUBLIC Source PRIVATE Source/External)
target_link_libraries(NovaEngine PUBLIC miniaudio imgui Vulkan::slang vorbisfile vorbis vorbisenc PRIVATE  box2d glfw assimp::assimp spirv-reflect-static kissfft::kissfft)
target_compile_definitions(NovaEngine PRIVATE NOVA_CORE INTERFACE NOVA_CLIENT PUBLIC NOVA_ENGINE_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

if(WIN32)
    target_link_libraries(NovaEngine PRIVATE kernel32)
    target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_WINDOWS_SOURCES})
endif ()

if(NOVA_ENGINE_BUILD_D3D12)
    target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_D3D12_SOURCES})
    target_compile_definitions(NovaEngine PUBLIC NOVA_HAS_D3D12)
    target_link_libraries(NovaEngine PRIVATE Microsoft::DirectX-Headers d3d12 dxgi PUBLIC D3D12MemoryAllocator)
endif ()

if(NOVA_ENGINE_BUILD_VULKAN)
    target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_VULKAN_SOURCES})
    target_compile_definitions(NovaEngine PUBLIC NOVA_HAS_VULKAN)
    target_link_libraries(NovaEngine PUBLIC Vulkan::Vulkan)
endif ()

if(NOVA_ENGINE_BUILD_OPENGL)
    target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_OPENGL_SOURCES})
    target_compile_definitions(NovaEngine PUBLIC NOVA_HAS_OPENGL)
    target_include_directories(NovaEngine PUBLIC Source/External/glad/include)
    target_link_libraries(NovaEngine PRIVATE )
endif ()

if(NOVA_ENGINE_CUSTOM_ENTRY_POINT)
    target_compile_definitions(NovaEngine PUBLIC NOVA_CUSTOM_ENTRY_POINT)
endif ()

if(NOVA_ENGINE_INCLUDE_AUDIO)
    target_compile_definitions(NovaEngine PUBLIC NOVA_HAS_AUDIO)
endif ()

if(NOVA_ENGINE_INCLUDE_PHYSICS)
    target_compile_definitions(NovaEngine PUBLIC NOVA_HAS_PHYSICS)
endif ()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(NovaEngine PUBLIC NOVA_DEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    target_compile_definitions(NovaEngine PUBLIC NOVA_DEV)
elseif (CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    target_compile_definitions(NovaEngine PUBLIC NOVA_RELEASE)
endif ()


if (WIN32)
    target_compile_definitions(NovaEngine PUBLIC NOVA_PLATFORM_WINDOWS UNICODE _UNICODE _CRT_SECURE_NO_WARNINGS)
elseif (LINUX)
    target_compile_definitions(NovaEngine PUBLIC NOVA_PLATFORM_LINUX)
elseif (MACOS)
    target_compile_definitions(NovaEngine PUBLIC NOVA_PLATFORM_MACOS)
endif ()