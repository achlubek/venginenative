#pragma once
#include <cstdio>

namespace VEngine
{
    namespace Input
    {
        class KeyboardInterface;
        class MouseInterface;
        class JoystickInterface;
    }
    namespace FileSystem
    {
        class MediaInterface;
    }
    namespace Renderer
    {

        class Object3dInfoFactoryInterface;
        class ShaderFactoryInterface;
        class DescriptorSetLayoutFactoryInterface;
        class RenderStageFactoryInterface;
        class ComputeStageFactoryInterface;
        class BufferFactoryInterface;
        class ImageFactoryInterface;
        class SwapChainOutputFactoryInterface;

        class ToolkitInterface
        {
        public:
            bool shouldCloseWindow();
            void poolEvents();
            double getExecutionTime();
            size_t getTotalAllocatedMemory();
            void waitQueueIdle();
            void waitDeviceIdle();

            Object3dInfoFactoryInterface* getObject3dInfoFactory();
            ShaderFactoryInterface* getVulkanShaderFactory();
            DescriptorSetLayoutFactoryInterface* getVulkanDescriptorSetLayoutFactory();
            RenderStageFactoryInterface* getVulkanRenderStageFactory();
            ComputeStageFactoryInterface* getVulkanComputeStageFactory();
            BufferFactoryInterface* getVulkanBufferFactory();
            ImageFactoryInterface* getVulkanImageFactory();
            SwapChainOutputFactoryInterface* getVulkanSwapChainOutputFactory();
            Input::KeyboardInterface* getKeyboard();
            Input::MouseInterface* getMouse();
            Input::JoystickInterface* getJoystick();
            FileSystem::MediaInterface* getMedia();
        };

    }
}