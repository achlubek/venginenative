#pragma once

namespace VEngine
{
    namespace Input
    {
        class Keyboard;
        class Mouse;
        class Joystick;
    }
    namespace FileSystem
    {
        class Media;
    }
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }

        class Object3dInfoFactory;
        class VulkanShaderFactory;
        class VulkanDescriptorSetLayoutFactory;
        class VulkanRenderStageFactory;
        class VulkanComputeStageFactory;
        class VulkanBufferFactory;
        class VulkanImageFactory;
        class VulkanSwapChainOutputFactory;

        class VulkanToolkit
        {
        public:
            VulkanToolkit(int width, int height, bool enableValidation, std::string windowName);
            VulkanToolkit(bool enableValidation);
            ~VulkanToolkit();
            bool shouldCloseWindow();
            void poolEvents();
            double getExecutionTime();
            size_t getTotalAllocatedMemory();
            void waitQueueIdle();
            void waitDeviceIdle();

            Object3dInfoFactory* getObject3dInfoFactory();
            VulkanShaderFactory* getVulkanShaderFactory();
            VulkanDescriptorSetLayoutFactory* getVulkanDescriptorSetLayoutFactory();
            VulkanRenderStageFactory* getVulkanRenderStageFactory();
            VulkanComputeStageFactory* getVulkanComputeStageFactory();
            VulkanBufferFactory* getVulkanBufferFactory();
            VulkanImageFactory* getVulkanImageFactory();
            VulkanSwapChainOutputFactory* getVulkanSwapChainOutputFactory();
            Input::Keyboard* getKeyboard();
            Input::Mouse* getMouse();
            Input::Joystick* getJoystick();
            FileSystem::Media* getMedia();

        private:
            Internal::VulkanDevice * device;
            FileSystem::Media* media;
            int windowWidth;
            int windowHeight;

            Object3dInfoFactory* object3dInfoFactory;
            VulkanShaderFactory* vulkanShaderFactory;
            VulkanDescriptorSetLayoutFactory* vulkanDescriptorSetLayoutFactory;
            VulkanRenderStageFactory* vulkanRenderStageFactory;
            VulkanComputeStageFactory* vulkanComputeStageFactory;
            VulkanBufferFactory* vulkanBufferFactory;
            VulkanImageFactory* vulkanImageFactory;
            VulkanSwapChainOutputFactory* vulkanSwapChainOutputFactory;
            Input::Keyboard* keyboard;
            Input::Mouse* mouse;
            Input::Joystick* joystick;
        };

    }
}