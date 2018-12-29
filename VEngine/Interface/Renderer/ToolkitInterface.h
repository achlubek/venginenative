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
        class SemaphoreFactoryInterface;

        class ToolkitInterface
        {
        public:
            virtual bool shouldCloseWindow() = 0;
            virtual void poolEvents() = 0;
            virtual double getExecutionTime() = 0;
            virtual size_t getTotalAllocatedMemory() = 0;
            virtual void waitQueueIdle() = 0;
            virtual void waitDeviceIdle() = 0;

            virtual Object3dInfoFactoryInterface* getObject3dInfoFactory() = 0;
            virtual ShaderFactoryInterface* getShaderFactory() = 0;
            virtual DescriptorSetLayoutFactoryInterface* getDescriptorSetLayoutFactory() = 0;
            virtual RenderStageFactoryInterface* getRenderStageFactory() = 0;
            virtual ComputeStageFactoryInterface* getComputeStageFactory() = 0;
            virtual BufferFactoryInterface* getBufferFactory() = 0;
            virtual ImageFactoryInterface* getImageFactory() = 0;
            virtual SwapChainOutputFactoryInterface* getSwapChainOutputFactory() = 0;
            virtual SemaphoreFactoryInterface* getSemaphoreFactory() = 0;
            virtual Input::KeyboardInterface* getKeyboard() = 0;
            virtual Input::MouseInterface* getMouse() = 0;
            virtual Input::JoystickInterface* getJoystick() = 0;
            virtual FileSystem::MediaInterface* getMedia() = 0;
        };

    }
}