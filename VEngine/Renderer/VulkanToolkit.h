#pragma once
#include "../Interface/Renderer/ToolkitInterface.h"

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
        namespace Internal
        {
            class VulkanDevice;
        }

        class Object3dInfoFactoryInterface;
        class ShaderFactoryInterface;
        class DescriptorSetLayoutFactoryInterface;
        class RenderStageFactoryInterface;
        class ComputeStageFactoryInterface;
        class BufferFactoryInterface;
        class ImageFactoryInterface;
        class SwapChainOutputFactoryInterface;

        class VulkanToolkit : public ToolkitInterface
        {
        public:
            VulkanToolkit(int width, int height, bool enableValidation, std::string windowName);
            VulkanToolkit(bool enableValidation);
            ~VulkanToolkit();
            virtual bool shouldCloseWindow() override;
            virtual void poolEvents() override;
            virtual double getExecutionTime() override;
            virtual size_t getTotalAllocatedMemory() override;
            virtual void waitQueueIdle() override;
            virtual void waitDeviceIdle() override;

            virtual Object3dInfoFactoryInterface* getObject3dInfoFactory() override;
            virtual ShaderFactoryInterface* getShaderFactory() override;
            virtual DescriptorSetLayoutFactoryInterface* getDescriptorSetLayoutFactory() override;
            virtual RenderStageFactoryInterface* getRenderStageFactory() override;
            virtual ComputeStageFactoryInterface* getComputeStageFactory() override;
            virtual BufferFactoryInterface* getBufferFactory() override;
            virtual ImageFactoryInterface* getImageFactory() override;
            virtual SwapChainOutputFactoryInterface* getSwapChainOutputFactory() override;
            virtual SemaphoreFactoryInterface* getSemaphoreFactory() override;
            virtual Input::KeyboardInterface* getKeyboard() override;
            virtual Input::MouseInterface* getMouse() override;
            virtual Input::JoystickInterface* getJoystick() override;
            virtual FileSystem::MediaInterface* getMedia() override;

        private:
            Internal::VulkanDevice * device;
            FileSystem::MediaInterface* media;
            int windowWidth;
            int windowHeight;

            SemaphoreFactoryInterface* vulkanSemaphoreFactory;
            Object3dInfoFactoryInterface* object3dInfoFactory;
            ShaderFactoryInterface* vulkanShaderFactory;
            DescriptorSetLayoutFactoryInterface* vulkanDescriptorSetLayoutFactory;
            RenderStageFactoryInterface* vulkanRenderStageFactory;
            ComputeStageFactoryInterface* vulkanComputeStageFactory;
            BufferFactoryInterface* vulkanBufferFactory;
            ImageFactoryInterface* vulkanImageFactory;
            SwapChainOutputFactoryInterface* vulkanSwapChainOutputFactory;
            Input::KeyboardInterface* keyboard;
            Input::MouseInterface* mouse;
            Input::JoystickInterface* joystick;
        };

    }
}