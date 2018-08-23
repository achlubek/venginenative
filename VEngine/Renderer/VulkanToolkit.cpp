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
            vulkanShaderFactory(new VulkanShaderFactory(device, media)),
            vulkanDescriptorSetLayoutFactory(new VulkanDescriptorSetLayoutFactory(device)),
            vulkanRenderStageFactory(new VulkanRenderStageFactory(device)),
            vulkanComputeStageFactory(new VulkanComputeStageFactory(device)),
            vulkanBufferFactory(new VulkanBufferFactory(device)),
            vulkanImageFactory(new VulkanImageFactory(device, media)),
            vulkanSwapChainOutputFactory(new VulkanSwapChainOutputFactory(device)),
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
            vulkanShaderFactory(new VulkanShaderFactory(device, media)),
            vulkanDescriptorSetLayoutFactory(new VulkanDescriptorSetLayoutFactory(device)),
            vulkanRenderStageFactory(new VulkanRenderStageFactory(device)),
            vulkanComputeStageFactory(new VulkanComputeStageFactory(device)),
            vulkanBufferFactory(new VulkanBufferFactory(device)),
            vulkanImageFactory(new VulkanImageFactory(device, media)),
            vulkanSwapChainOutputFactory(new VulkanSwapChainOutputFactory(device)),
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

        void VulkanToolkit::waitQueueIdle()
        {
            vkQueueWaitIdle(device->getMainQueue());
        }

        void VulkanToolkit::waitDeviceIdle()
        {
            vkDeviceWaitIdle(device->getDevice());
        }

        Object3dInfoFactory * VulkanToolkit::getObject3dInfoFactory()
        {
            return object3dInfoFactory;
        }

        VulkanShaderFactory * VulkanToolkit::getVulkanShaderFactory()
        {
            return vulkanShaderFactory;
        }

        VulkanDescriptorSetLayoutFactory * VulkanToolkit::getVulkanDescriptorSetLayoutFactory()
        {
            return vulkanDescriptorSetLayoutFactory;
        }

        VulkanRenderStageFactory * VulkanToolkit::getVulkanRenderStageFactory()
        {
            return vulkanRenderStageFactory;
        }

        VulkanComputeStageFactory * VulkanToolkit::getVulkanComputeStageFactory()
        {
            return vulkanComputeStageFactory;
        }

        VulkanBufferFactory * VulkanToolkit::getVulkanBufferFactory()
        {
            return vulkanBufferFactory;
        }

        VulkanImageFactory * VulkanToolkit::getVulkanImageFactory()
        {
            return vulkanImageFactory;
        }

        VulkanSwapChainOutputFactory * VulkanToolkit::getVulkanSwapChainOutputFactory()
        {
            return vulkanSwapChainOutputFactory;
        }

        Input::Keyboard * VulkanToolkit::getKeyboard()
        {
            return keyboard;
        }

        Input::Mouse * VulkanToolkit::getMouse()
        {
            return mouse;
        }

        Input::Joystick * VulkanToolkit::getJoystick()
        {
            return joystick;
        }

        FileSystem::Media * VulkanToolkit::getMedia()
        {
            return media;
        }
    }
}