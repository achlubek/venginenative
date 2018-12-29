#include "stdafx.h"
#include "VulkanToolkit.h"
#include "Internal/VulkanDevice.h"

#include "Object3dInfoFactory.h"
#include "VulkanShaderFactory.h"
#include "VulkanDescriptorSetLayoutFactory.h"
#include "VulkanRenderStageFactory.h"
#include "VulkanComputeStageFactory.h"
#include "VulkanBufferFactory.h"
#include "VulkanImageFactory.h"
#include "VulkanSemaphoreFactory.h"
#include "VulkanSwapChainOutputFactory.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"
#include "../Input/Joystick.h"
#include "../FileSystem/Media.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;
        using namespace VEngine::Input;
        using namespace VEngine::FileSystem;

        VulkanToolkit::VulkanToolkit(int width, int height, bool enableValidation, std::string windowName)
            : device(new VulkanDevice(width, height, enableValidation, windowName)),
            windowWidth(width), windowHeight(height),
            media(new Media()),
            object3dInfoFactory(new Object3dInfoFactory(device, media)),
            vulkanSemaphoreFactory(new VulkanSemaphoreFactory(device)),
            vulkanShaderFactory(new VulkanShaderFactory(device, media)),
            vulkanDescriptorSetLayoutFactory(new VulkanDescriptorSetLayoutFactory(device)),
            vulkanRenderStageFactory(new VulkanRenderStageFactory(device, vulkanSemaphoreFactory)),
            vulkanComputeStageFactory(new VulkanComputeStageFactory(device, vulkanSemaphoreFactory)),
            vulkanBufferFactory(new VulkanBufferFactory(device)),
            vulkanImageFactory(new VulkanImageFactory(device, media)),
            vulkanSwapChainOutputFactory(new VulkanSwapChainOutputFactory(device, vulkanSemaphoreFactory)),
            keyboard(new Keyboard(device->getWindow())),
            mouse(new Mouse(device->getWindow())),
            joystick(new Joystick(device->getWindow()))
        {
        }

        VulkanToolkit::VulkanToolkit(bool enableValidation)
            : device(new VulkanDevice(0, 0, enableValidation, "")),
            windowWidth(0), windowHeight(0),
            media(new Media()),
            object3dInfoFactory(new Object3dInfoFactory(device, media)),
            vulkanSemaphoreFactory(new VulkanSemaphoreFactory(device)),
            vulkanShaderFactory(new VulkanShaderFactory(device, media)),
            vulkanDescriptorSetLayoutFactory(new VulkanDescriptorSetLayoutFactory(device)),
            vulkanRenderStageFactory(new VulkanRenderStageFactory(device, vulkanSemaphoreFactory)),
            vulkanComputeStageFactory(new VulkanComputeStageFactory(device, vulkanSemaphoreFactory)),
            vulkanBufferFactory(new VulkanBufferFactory(device)),
            vulkanImageFactory(new VulkanImageFactory(device, media)),
            vulkanSwapChainOutputFactory(new VulkanSwapChainOutputFactory(device, vulkanSemaphoreFactory)),
            keyboard(nullptr),
            mouse(nullptr),
            joystick(nullptr)
        {
        }

        VulkanToolkit::~VulkanToolkit()
        {
            safedelete(object3dInfoFactory);
            safedelete(vulkanShaderFactory);
            safedelete(vulkanDescriptorSetLayoutFactory);
            safedelete(vulkanRenderStageFactory);
            safedelete(vulkanComputeStageFactory);
            safedelete(vulkanBufferFactory);
            safedelete(vulkanImageFactory);
            safedelete(vulkanSemaphoreFactory);
            safedelete(vulkanSwapChainOutputFactory);
            safedelete(keyboard);
            safedelete(mouse);
            safedelete(joystick);
            safedelete(media);
            safedelete(device);
        }

        bool VulkanToolkit::shouldCloseWindow()
        {
            return device->shouldCloseWindow();
        }

        void VulkanToolkit::poolEvents()
        {
            device->poolEvents();
        }

        double VulkanToolkit::getExecutionTime()
        {
            return device->getExecutionTime();
        }

        size_t VulkanToolkit::getTotalAllocatedMemory()
        {
            return device->getMemoryManager()->getTotalAllocatedMemory();
        }

        void VulkanToolkit::waitQueueIdle()
        {
            vkQueueWaitIdle(device->getMainQueue());
        }

        void VulkanToolkit::waitDeviceIdle()
        {
            vkDeviceWaitIdle(device->getDevice());
        }

        Object3dInfoFactoryInterface * VulkanToolkit::getObject3dInfoFactory()
        {
            return object3dInfoFactory;
        }

        ShaderFactoryInterface * VulkanToolkit::getShaderFactory()
        {
            return vulkanShaderFactory;
        }

        DescriptorSetLayoutFactoryInterface * VulkanToolkit::getDescriptorSetLayoutFactory()
        {
            return vulkanDescriptorSetLayoutFactory;
        }

        RenderStageFactoryInterface * VulkanToolkit::getRenderStageFactory()
        {
            return vulkanRenderStageFactory;
        }

        ComputeStageFactoryInterface * VulkanToolkit::getComputeStageFactory()
        {
            return vulkanComputeStageFactory;
        }

        BufferFactoryInterface * VulkanToolkit::getBufferFactory()
        {
            return vulkanBufferFactory;
        }

        ImageFactoryInterface * VulkanToolkit::getImageFactory()
        {
            return vulkanImageFactory;
        }

        SwapChainOutputFactoryInterface * VulkanToolkit::getSwapChainOutputFactory()
        {
            return vulkanSwapChainOutputFactory;
        }

        SemaphoreFactoryInterface * VulkanToolkit::getSemaphoreFactory()
        {
            return vulkanSemaphoreFactory;
        }

        Input::KeyboardInterface * VulkanToolkit::getKeyboard()
        {
            return keyboard;
        }

        Input::MouseInterface * VulkanToolkit::getMouse()
        {
            return mouse;
        }

        Input::JoystickInterface * VulkanToolkit::getJoystick()
        {
            return joystick;
        }

        FileSystem::MediaInterface * VulkanToolkit::getMedia()
        {
            return media;
        }
    }
}